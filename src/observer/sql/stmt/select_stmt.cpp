/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2022/6/6.
//

#include "sql/stmt/select_stmt.h"

SelectStmt::~SelectStmt()
{
  while (!push_down_filter_stmts_.empty()) {
    delete push_down_filter_stmts_.back();
    push_down_filter_stmts_.pop_back();
  }

  delete join_keys_;
  join_keys_ = nullptr;
}

RC SelectStmt::create(Db *db, const Selects &select_sql, Stmt *&stmt)
{
  RC rc = RC::SUCCESS;
  if (nullptr == db) {
    LOG_WARN("invalid argument. db is null");
    return RC::INVALID_ARGUMENT;
  }

  // collect tables in `from` statement
  std::vector<Table *> tables;
  std::unordered_map<std::string, Table *> table_map;
  std::map<std::string, std::string> table_name_alias_map;  //表的别名与原名的映射
  rc = collect_tables_in_from_statement(db, select_sql, tables, table_map, table_name_alias_map);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to collect tables in from statement");
    return rc;
  }

  // collect group by keys
  std::vector<Field> group_by_keys;
  rc = collect_groupby_keys(db, select_sql, tables, table_map, group_by_keys);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to collect tables in group-by statement");
    return rc;
  }

  // collect query fields in `select` statement
  std::vector<Field> query_fields;
  size_t attr_offset = 0;
  for (int i = select_sql.attr_num - 1; i >= 0; i--) {
    const RelAttr &relation_attr = select_sql.attributes[i];
    // attr_offset会在collect_rel_attr_into_query_fields中增加
    rc = collect_rel_attr_into_query_fields(
        relation_attr, db, select_sql, tables, table_map, attr_offset, group_by_keys, query_fields);
    if (rc != RC::SUCCESS) {
      LOG_WARN("cannot collect rel_attr");
      return rc;
    }
  }

  // create expression for projection
  std::vector<Expression *> query_exprs;
  std::vector<Field> expr_aggr;
  for (Field field : query_fields) {
    const Table *table = field.table();
    const FieldMeta *field_meta = field.meta();

    if (field_meta->is_expr()){
      Expression *expr = nullptr;
      expr = ComplexExpr::create_complex_expr(field_meta->name(), table_map, tables[0], expr_aggr, attr_offset);
      query_exprs.push_back(expr);
    } else {
      query_exprs.push_back(new FieldExpr(table, field_meta));
    }
  }
  // 若expression里存在聚合，此处补上
  for (Field field : expr_aggr) {
    query_fields.push_back(field);
  }

  LOG_INFO("got %d tables in from stmt and %d fields in query stmt", tables.size(), query_fields.size());

  // collect order fields in `Order By` statement
  std::vector<std::tuple<FieldExpr, int>> order_fields;
  rc = collect_fields_in_orderby_stmt(db, select_sql, tables, table_map, order_fields);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to collect tables in order-by statement");
    return rc;
  }

  // create filter statement in `where` statement
  FilterStmt *reserved_filter_stmt = nullptr;
  std::vector<FilterStmt *> push_down_filter_stmts;
  rc = create_filter_for_where_stmt(db, select_sql, tables, table_map, reserved_filter_stmt, push_down_filter_stmts);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to collect tables in where statement");
    return rc;
  }

  // 此处创建expression相关conds列表 (default_table默认tables[0])
  FilterStmt *expr_stmt = nullptr;
  rc = FilterStmt::create(db, tables[0], &table_map, select_sql.expr_conds, select_sql.expr_cond_num, expr_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct join filter stmt");
    return rc;
  }

  // 创建join相关conds列表
  FilterStmt *join_keys = nullptr;
  rc = FilterStmt::create(
      db, nullptr, &table_map, select_sql.join_conds, select_sql.join_cond_num, join_keys, select_sql.is_subquery);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct join filter stmt");
    return rc;
  }

  // create filter statement in `having` statement
  HavingStmt *having_stmt = nullptr;
  if (select_sql.is_aggr && select_sql.having_condition_num > 0) {
    rc = init_and_create_having_stmt(
        db, select_sql, tables, table_map, group_by_keys, query_fields, attr_offset, having_stmt);
    if (rc != RC::SUCCESS) {
      LOG_WARN("cannot collect having fields stmt");
      return rc;
    }
  }

  // everything alright
  SelectStmt *select_stmt = new SelectStmt();
  select_stmt->tables_.swap(tables);
  select_stmt->query_fields_.swap(query_fields);
  select_stmt->query_exprs_.swap(query_exprs);
  select_stmt->order_fields_.swap(order_fields);
  select_stmt->filter_stmt_ = reserved_filter_stmt;
  select_stmt->push_down_filter_stmts_.swap(push_down_filter_stmts);
  select_stmt->join_keys_ = join_keys;
  select_stmt->do_aggr_ = select_sql.is_aggr;  // 告知执行器生成aggregate_operator
  if (!expr_aggr.empty()) select_stmt->do_aggr_ = true;
  select_stmt->having_stmt_ = having_stmt;
  select_stmt->group_keys_.swap(group_by_keys);
  select_stmt->expr_stmt_ = expr_stmt;
  select_stmt->table_name_alias_map_ = table_name_alias_map;
  stmt = select_stmt;
  return RC::SUCCESS;
}

//  传入RelAttr，将对应的Field加入到query_fields
RC collect_rel_attr_into_query_fields(const RelAttr &relation_attr, Db *db, const Selects &select_sql,
    std::vector<Table *> tables, std::unordered_map<std::string, Table *> table_map, size_t &attr_offset,
    std::vector<Field> group_by_keys, std::vector<Field> &query_fields, bool visible)
{
  RC rc = RC::SUCCESS;
  if (common::is_blank(relation_attr.relation_name) && 0 == strcmp(relation_attr.attribute_name, "*")) {
    rc = process_attr_with_star(select_sql, relation_attr, attr_offset, tables, group_by_keys, query_fields, visible);
  } else if (!common::is_blank(relation_attr.relation_name)) {  // select ID DOT ID from t;
    char *table_name = relation_attr.relation_name;
    char *field_name = relation_attr.attribute_name;
    rc = process_attr_with_dot(select_sql,
        relation_attr,
        tables,
        table_map,
        table_name,
        field_name,
        group_by_keys,
        attr_offset,
        query_fields,
        visible);
  } else if (relation_attr.is_complex) { // select expression from t, 这种情况下relation_name为空，expression存放在attribute_name
      FieldMeta *field_meta = new FieldMeta;
      field_meta->init(relation_attr.attribute_name, FLOATS, attr_offset, sizeof(float), true, /* nullable */false);
      field_meta->set_expr();
      query_fields.push_back(Field(nullptr, field_meta, NOT_AGGR, nullptr, relation_attr.alias));
    } else {
    rc = process_simple_attr(select_sql, relation_attr, tables, group_by_keys, attr_offset, query_fields, visible);
  }

  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to execute collect_rel_attr_into_query_fields");
  }
  return rc;
}

RC process_attr_with_star(const Selects &select_sql, const RelAttr &relation_attr, size_t &attr_offset,
    const std::vector<Table *> &tables, std::vector<Field> &group_by_keys, std::vector<Field> &query_fields,
    bool visible)
{
  RC rc = RC::SUCCESS;
  if (relation_attr.aggr_type != NOT_AGGR) {
    // select count(*) from t;
    // 只有COUNT操作支持*通配符
    if (relation_attr.aggr_type != COUNT) {
      return RC::MISMATCH;
    }
    size_t attr_len = sizeof(int);
    FieldMeta *field_meta = new FieldMeta;
    field_meta->init("COUNT(*)", INTS, attr_offset, attr_len, visible, /* nullable */ false);
    query_fields.push_back(Field(nullptr, field_meta, relation_attr.aggr_type, nullptr, relation_attr.alias));
    attr_offset += attr_len;
  } else {
    for (Table *table : tables) {
      // select * from t;
      rc = wildcard_fields(table, query_fields, select_sql.is_aggr, group_by_keys, attr_offset, relation_attr.alias);
      if (rc != RC::SUCCESS) {
        LOG_WARN("invalid field name while field is *.");
        return rc;
      }
    }
  }
  return rc;
}

RC process_attr_with_dot(const Selects &select_sql, const RelAttr &relation_attr, std::vector<Table *> &tables,
    std::unordered_map<std::string, Table *> &table_map, const char *table_name, const char *field_name,
    std::vector<Field> &group_by_keys, size_t &attr_offset, std::vector<Field> &query_fields, bool visible)
{
  RC rc = RC::SUCCESS;
  if (0 == strcmp(table_name, "*")) {
    if (0 != strcmp(field_name, "*") || select_sql.is_aggr) {
      LOG_WARN("invalid field name while table is *. attr=%s", field_name);
      return RC::SCHEMA_FIELD_MISSING;
    }
    for (Table *table : tables) {
      wildcard_fields(table, query_fields, false, group_by_keys, attr_offset, relation_attr.alias);
    }
  } else {
    auto iter = table_map.find(table_name);
    if (iter == table_map.end()) {
      LOG_WARN("no such table in from list: %s", table_name);
      return RC::SCHEMA_FIELD_MISSING;
    }

    Table *table = iter->second;
    if (0 == strcmp(field_name, "*")) {
      // select t.* from t;
      wildcard_fields(table, query_fields, select_sql.is_aggr, group_by_keys, attr_offset, relation_attr.alias);
    } else {
      // select t.a from t;
      const FieldMeta *field_meta = table->table_meta().field(field_name);
      if (nullptr == field_meta) {
        LOG_WARN("no such field. field=%s.%s", table->name(), field_name);
        return RC::SCHEMA_FIELD_MISSING;
      }
      rc = create_query_field(
          table, select_sql, field_meta, relation_attr, group_by_keys, attr_offset, query_fields, visible);
      if (rc != RC::SUCCESS) {
        return rc;
      }
    }
  }
  return RC::SUCCESS;
}

RC process_simple_attr(const Selects &select_sql, const RelAttr &relation_attr, const std::vector<Table *> &tables,
    std::vector<Field> &group_by_keys, size_t &attr_offset, std::vector<Field> &query_fields, bool visible)
{
  RC rc = RC::SUCCESS;
  Table *table = nullptr;
  rc = find_table_by_attr_name(tables, table, relation_attr);
  if (rc != RC::SUCCESS) {
    LOG_WARN("invalid. I do not know the attr's table. attr=%s", relation_attr.attribute_name);
    return rc;
  }

  const FieldMeta *field_meta = table->table_meta().field(relation_attr.attribute_name);
  if (nullptr == field_meta) {
    LOG_WARN("no such field. field=%s.%s", table->name(), relation_attr.attribute_name);
    return RC::SCHEMA_FIELD_MISSING;
  }
  rc = create_query_field(
      table, select_sql, field_meta, relation_attr, group_by_keys, attr_offset, query_fields, visible);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  return RC::SUCCESS;
}

// 通过列名查找所属的table
RC find_table_by_attr_name(const std::vector<Table *> &tables, Table *&table, const RelAttr &rel_attr)
{
  table = nullptr;
  for (Table *t : tables) {
    if (!table && t->table_meta().field(rel_attr.attribute_name)) {
      table = t;
    } else if (table && t->table_meta().field(rel_attr.attribute_name)) {
      LOG_WARN("invalid. I do not know the attr's table. attr=%s", rel_attr.attribute_name);
      return RC::SCHEMA_FIELD_MISSING;
    }
  }
  if (table == nullptr) {
    return RC::SCHEMA_FIELD_MISSING;
  }
  return RC::SUCCESS;
}

//检查非聚合字段是否出现在group by子句中
RC check_field_in_group(bool is_aggr, const FieldMeta *field_meta, const std::vector<Field> &group_by_keys)
{
  if (!is_aggr) {
    return RC::SUCCESS;
  }
  for (const Field &key : group_by_keys) {
    if (strcmp(key.field_name(), field_meta->name()) == 0) {
      return RC::SUCCESS;
    }
  }
  return RC::SCHEMA_FIELD_NAME_ILLEGAL;
}

RC wildcard_fields(Table *table, std::vector<Field> &field_metas, bool is_aggr, std::vector<Field> &group_by_keys,
    size_t &attr_offset, const char *alias)
{
  const TableMeta &table_meta = table->table_meta();
  const int field_num = table_meta.field_num();
  for (int i = table_meta.sys_field_num(); i < field_num; i++) {
    const FieldMeta *cur_fieldmeta = table_meta.field(i);
    if (is_aggr) {
      //为count(*)创建Field
      if (!check_field_in_group(is_aggr, table_meta.field(i), group_by_keys)) {
        return RC::SCHEMA_FIELD_NAME_ILLEGAL;
      }
      FieldMeta *new_fieldmeta = new FieldMeta();
      new_fieldmeta->init(cur_fieldmeta->name(),
          cur_fieldmeta->type(),
          attr_offset,
          cur_fieldmeta->len(),
          true,
          cur_fieldmeta->nullable());
      field_metas.push_back(Field(table, new_fieldmeta, NOT_AGGR, cur_fieldmeta, alias));
      attr_offset += table_meta.field(i)->len();
    } else {
      field_metas.push_back(Field(table, table_meta.field(i), ""));
    }
  }
  return RC::SUCCESS;
}

RC create_query_field(Table *table, const Selects &select_sql, const FieldMeta *field_meta,
    const RelAttr &relation_attr, std::vector<Field> &group_by_keys, size_t &attr_offset,
    std::vector<Field> &query_fields, bool visible)
{
  RC rc = RC::SUCCESS;
  if (select_sql.is_aggr) {
    // select min/max/sum/avg/count(a), b from t group by b;
    if (relation_attr.aggr_type == SUM && field_meta->type() == CHARS) {
      return RC::MISMATCH;
    }
    FieldMeta *aggr_field_meta = new FieldMeta();
    size_t attr_len;
    AttrType attr_type;
    char *aggr_field_name;
    aggr_field_name = strdup(relation_attr.attribute_name);
    if (relation_attr.aggr_type == NOT_AGGR) {
      rc = check_field_in_group(true, field_meta, group_by_keys);
      if (rc != RC::SUCCESS) {
        LOG_WARN("invalid field name. attr=%s", field_meta->name());
        return rc;
      }
    }
    switch (relation_attr.aggr_type) {
      case NOT_AGGR:
      case MIN:
      case MAX:
      case SUM:
        attr_len = field_meta->len();
        attr_type = field_meta->type();
        break;
      case AVG:
        attr_len = sizeof(float);
        attr_type = FLOATS;
        break;
      case COUNT:
        attr_len = sizeof(int);
        attr_type = INTS;
        break;
    }
    aggr_field_meta->init(aggr_field_name, attr_type, attr_offset, attr_len, visible, field_meta->nullable());
    if (relation_attr.aggr_type != NOT_AGGR) {
      aggr_field_meta->dirty_hack_set_namefunc(aggr_name[relation_attr.aggr_type]);
    }
    query_fields.push_back(Field(table, aggr_field_meta, relation_attr.aggr_type, field_meta, relation_attr.alias));
    attr_offset += attr_len;
  } else {
    // select a from t;
    query_fields.push_back(Field(table, field_meta, relation_attr.alias));
  }
  return rc;
}

RC collect_tables_in_from_statement(Db *db, const Selects &select_sql, std::vector<Table *> &tables,
    std::unordered_map<std::string, Table *> &table_map, std::map<std::string, std::string> &alias_map)
{
  // 保证表的顺序与解析时一样
  for (int i = select_sql.relation_num - 1; i >= 0; i--) {
    const char *table_name = select_sql.relations[i].relation_name;
    const char *alias = select_sql.relations[i].alias;
    if (nullptr == table_name) {
      LOG_WARN("invalid argument. relation name is null. index=%d", i);
      return RC::INVALID_ARGUMENT;
    }
    if (alias && alias_map.find(alias) != alias_map.end()) {
      LOG_WARN("duplicated alias name");
      return RC::INVALID_ARGUMENT;
    }

    Table *table = db->find_table(table_name);
    if (nullptr == table) {
      LOG_WARN("no such table. db=%s, table_name=%s", table_name);
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }

    tables.push_back(table);
    table_map.insert(std::pair<std::string, Table *>(table_name, table));
    if (alias) {
      alias_map.insert(std::pair<std::string, std::string>(table_name, alias));
    }
  }
  return RC::SUCCESS;
}

RC collect_fields_in_orderby_stmt(Db *db, const Selects &select_sql, std::vector<Table *> &tables,
    std::unordered_map<std::string, Table *> &table_map, std::vector<std::tuple<FieldExpr, int>> &order_fields)
{
  for (int i = select_sql.order_num - 1; i >= 0; i--) {
    const OrderAttr &order_attr = select_sql.orders[i];

    if (!common::is_blank(order_attr.relation_name)) {
      const char *table_name = order_attr.relation_name;
      const char *field_name = order_attr.attribute_name;

      auto iter = table_map.find(table_name);
      if (iter == table_map.end()) {
        LOG_WARN("no such table in from list: %s", table_name);
        return RC::SCHEMA_FIELD_MISSING;
      }

      Table *table = iter->second;
      const FieldMeta *field_meta = table->table_meta().field(field_name);
      if (nullptr == field_meta) {
        LOG_WARN("no such field. field=%s.%s.%s", db->name(), table->name(), field_name);
        return RC::SCHEMA_FIELD_MISSING;
      }
      order_fields.push_back(std::make_tuple(FieldExpr(table, field_meta), order_attr.is_asc));

    } else {
      if (tables.size() != 1) {
        LOG_WARN("invalid. I do not know the attr's table. attr=%s", order_attr.attribute_name);
        return RC::SCHEMA_FIELD_MISSING;
      }

      Table *table = tables[0];
      const FieldMeta *field_meta = table->table_meta().field(order_attr.attribute_name);
      if (nullptr == field_meta) {
        LOG_WARN("no such field. field=%s.%s.%s", db->name(), table->name(), order_attr.attribute_name);
        return RC::SCHEMA_FIELD_MISSING;
      }

      order_fields.push_back(std::make_tuple(FieldExpr(table, field_meta), order_attr.is_asc));
    }
  }
  return RC::SUCCESS;
}

RC create_filter_for_where_stmt(Db *db, const Selects &select_sql, std::vector<Table *> &tables,
    std::unordered_map<std::string, Table *> &table_map, FilterStmt *&reserved_filter_stmt,
    std::vector<FilterStmt *> &push_down_filter_stmts)
{
  RC rc = RC::SUCCESS;
  if (tables.size() == 1) {
    Table *default_table = nullptr;
    default_table = tables[0];
    rc = FilterStmt::create(db,
        default_table,
        &table_map,
        select_sql.conditions,
        select_sql.condition_num,
        reserved_filter_stmt,
        select_sql.is_subquery);
    if (rc != RC::SUCCESS) {
      LOG_WARN("cannot construct filter stmt");
      return rc;
    }
    reserved_filter_stmt->set_filter_mode(select_sql.condition_mode);
  } else {  //分开每个table对应的filter，两边都是常数的filter存放在所有table中
    FilterStmt *filter_stmt;
    char *space = new char[select_sql.condition_num]();
    common::Bitmap bitmap(space, select_sql.condition_num);
    for (size_t i = 0; i < select_sql.relation_num; i++) {
      rc = FilterStmt::create_push_down_filters(db,
          tables[i],
          &table_map,
          select_sql.conditions,
          select_sql.condition_num,
          filter_stmt,
          bitmap,
          select_sql.condition_mode);
      if (rc != RC::SUCCESS) {
        LOG_WARN("cannot construct filter stmt");
        return rc;
      }
      push_down_filter_stmts.push_back(filter_stmt);
    }
    // 没有被下推的conditions记录到filter_stmt_
    Condition *remaining_conditions = new Condition[select_sql.condition_num];
    size_t remaining_num = 0;
    reserved_filter_stmt = nullptr;
    for (size_t i = 0; i < select_sql.condition_num; i++) {
      if (bitmap.get_bit(i)) {
        continue;
      }
      if (rc != RC::SUCCESS) {
        LOG_WARN("cannot construct filter stmt");
        return rc;
      }
      remaining_conditions[remaining_num++] = select_sql.conditions[i];
    }
    if (remaining_num > 0) {
      rc = FilterStmt::create(
          db, nullptr, &table_map, remaining_conditions, remaining_num, reserved_filter_stmt, select_sql.is_subquery);
      if (rc != RC::SUCCESS) {
        LOG_WARN("cannot construct filter stmt");
        return rc;
      }
      reserved_filter_stmt->set_filter_mode(select_sql.condition_mode);
    }
    delete[] space;
  }
  return RC::SUCCESS;
}

RC collect_groupby_keys(Db *db, const Selects &select_sql, std::vector<Table *> &tables,
    std::unordered_map<std::string, Table *> &table_map, std::vector<Field> &group_by_keys)
{
  RC rc = RC::SUCCESS;
  if (select_sql.is_aggr) {
    for (int i = 0; i < select_sql.group_by_key_num; i++) {
      const RelAttr &group_by_attr = select_sql.group_by_keys[i];
      Table *table;
      if (common::is_blank(group_by_attr.relation_name)) {
        rc = find_table_by_attr_name(tables, table, group_by_attr);
        if (rc != RC::SUCCESS || table == nullptr) {
          LOG_WARN("failed to find table for group-by key %s", group_by_attr.attribute_name);
          return rc;
        }
      } else {
        table = db->find_table(group_by_attr.relation_name);
      }
      const FieldMeta *field_meta = table->table_meta().field(group_by_attr.attribute_name);
      if (nullptr == field_meta) {
        LOG_WARN("no such field. field=%s.%s.%s", db->name(), table->name(), group_by_attr.attribute_name);
        return RC::SCHEMA_FIELD_MISSING;
      }
      group_by_keys.push_back(Field(table, field_meta, ""));
    }
  }
  return rc;
}

// 从having_condition中取出所有字段，聚合时合并到query_fields中
RC init_and_create_having_stmt(Db *db, const Selects &select_sql, const std::vector<Table *> &tables,
    const std::unordered_map<std::string, Table *> &table_map, std::vector<Field> &group_by_keys,
    std::vector<Field> &query_fields, size_t &attr_offset, HavingStmt *&having_stmt)
{
  // fields in having clause will be marked as invisible, projection operator will ignore invisible fields
  RC rc = RC::SUCCESS;
  if (!select_sql.is_aggr || select_sql.having_condition_num == 0) {
    having_stmt = nullptr;
    return RC::SUCCESS;
  }
  std::vector<Field> having_fields;
  size_t offset = 0;
  for (size_t i = 0; i < select_sql.having_condition_num; i++) {
    const Condition &having_condition = select_sql.having_conditions[i];
    // left
    if (having_condition.left_expr_type == ATTR) {
      RelAttr left_rel_attr = having_condition.left_attr;
      if (find_rel_attr(select_sql.attributes, left_rel_attr, select_sql.attr_num)) {
        continue;
      } else {
        rc = collect_rel_attr_into_query_fields(
            left_rel_attr, db, select_sql, tables, table_map, attr_offset, group_by_keys, query_fields, false);
        if (rc != RC::SUCCESS) {
          LOG_WARN("cannot collect rel_attr");
          return rc;
        }
      }
    }  // end if

    // right
    if (having_condition.right_expr_type == ATTR) {
      RelAttr right_rel_attr = having_condition.right_attr;
      if (find_rel_attr(select_sql.attributes, right_rel_attr, select_sql.attr_num)) {
        continue;
      } else {
        rc = collect_rel_attr_into_query_fields(
            right_rel_attr, db, select_sql, tables, table_map, attr_offset, group_by_keys, query_fields, false);
        if (rc != RC::SUCCESS) {
          LOG_WARN("cannot collect rel_attr");
          return rc;
        }
      }
    }
  }  // end if

  rc = HavingStmt::create_having_stmt(query_fields, group_by_keys, &select_sql, having_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
  }
  return rc;
}

//从attr_list中查找，判断target_rel_attr是否出现过
bool find_rel_attr(const RelAttr *rel_attr_collection, RelAttr target_rel_attr, size_t collections_size)
{
  for (size_t i = 0; i < collections_size; i++) {
    if (rel_attr_collection[i].aggr_type != target_rel_attr.aggr_type) {
      continue;
    }
    if (strcmp(rel_attr_collection[i].attribute_name, target_rel_attr.attribute_name) != 0) {
      continue;
    }
    const char *cur_alias = rel_attr_collection[i].alias;
    const char *target_alias = target_rel_attr.alias;
    if ((cur_alias || target_alias) && strcmp(cur_alias, target_alias) != 0) {
      continue;
    }
    // Now that rel_attr_colletion[i] and target_rel_attr have the same attribute_name, they must have the same table
    // name, otherwise we can't determine which table it belongs to.
    if (common::is_blank(rel_attr_collection[i].relation_name) ||
        !common::is_blank(target_rel_attr.relation_name) &&
            0 == strcmp(rel_attr_collection[i].relation_name, target_rel_attr.relation_name)) {
      return true;
    }
  }
  return false;
}