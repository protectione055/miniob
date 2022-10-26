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
#include "sql/stmt/filter_stmt.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "storage/common/db.h"
#include "storage/common/table.h"

const char *const aggr_name[] = {
    "NOT_AGGR",
    "MIN",
    "MAX",
    "SUM",
    "COUNT",
    "AVG",
};

// 通过列名查找所属的table
RC find_table(const std::vector<Table *> &tables, Table *&table, const RelAttr &rel_attr)
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
  return RC::SUCCESS;
}

//检查非聚合字段是否出现在group by子句中
RC check_field_in_group(bool is_aggr, const FieldMeta *field_meta, std::vector<Field> &group_by_keys)
{
  if (!is_aggr) {
    return RC::SUCCESS;
  }
  for (Field &key : group_by_keys) {
    if (strcmp(key.field_name(), field_meta->name()) == 0) {
      return RC::SUCCESS;
    }
  }
  return RC::SCHEMA_FIELD_NAME_ILLEGAL;
}

SelectStmt::~SelectStmt()
{
  if (nullptr != filter_stmt_) {
    delete filter_stmt_;
    filter_stmt_ = nullptr;
  }
}

static RC wildcard_fields(
    Table *table, std::vector<Field> &field_metas, bool is_aggr, std::vector<Field> &group_by_keys, size_t &attr_offset)
{
  const TableMeta &table_meta = table->table_meta();
  const int field_num = table_meta.field_num();
  for (int i = table_meta.sys_field_num(); i < field_num; i++) {
    const FieldMeta *cur_fieldmeta = table_meta.field(i);
    if (is_aggr) {
      // 如果是聚合查询，必须检查字段是否出现在group by子句中
      if (!check_field_in_group(is_aggr, table_meta.field(i), group_by_keys)) {
        return RC::SCHEMA_FIELD_NAME_ILLEGAL;
      }
      FieldMeta *new_fieldmeta = new FieldMeta();
      new_fieldmeta->init(cur_fieldmeta->name(), cur_fieldmeta->type(), attr_offset, cur_fieldmeta->len(), true);
      field_metas.push_back(Field(table, new_fieldmeta, NOT_AGGR, cur_fieldmeta));
      attr_offset += table_meta.field(i)->len();
    } else {
      field_metas.push_back(Field(table, table_meta.field(i)));
    }
  }
  return RC::SUCCESS;
}
  return RC::SUCCESS;
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
  for (size_t i = 0; i < select_sql.relation_num; i++) {
    const char *table_name = select_sql.relations[i];
    if (nullptr == table_name) {
      LOG_WARN("invalid argument. relation name is null. index=%d", i);
      return RC::INVALID_ARGUMENT;
    }

    Table *table = db->find_table(table_name);
    if (nullptr == table) {
      LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }

    tables.push_back(table);
    table_map.insert(std::pair<std::string, Table *>(table_name, table));
  }

  // collect group by keys
  std::vector<Field> group_by_keys;
  if (select_sql.is_aggr) {
    for (int i = 0; i < select_sql.group_by_key_num; i++) {
      const RelAttr &group_by_attr = select_sql.group_by_keys[i];
      Table *table;
      if (common::is_blank(group_by_attr.relation_name)) {
        rc = find_table(tables, table, group_by_attr);
        if (rc != RC::SUCCESS || table == nullptr) {
          LOG_WARN("failed to find table for group-by key %s", group_by_attr.attribute_name);
          return rc;
        }
      }
      const FieldMeta *field_meta = table->table_meta().field(group_by_attr.attribute_name);
      if (nullptr == field_meta) {
        LOG_WARN("no such field. field=%s.%s.%s", db->name(), table->name(), group_by_attr.attribute_name);
        return RC::SCHEMA_FIELD_MISSING;
      }
      group_by_keys.push_back(Field(table, field_meta, NOT_AGGR, nullptr));
    }
  }

  // collect query fields in `select` statement
  std::vector<Field> query_fields;
  size_t attr_offset = 0;
  for (int i = select_sql.attr_num - 1; i >= 0; i--) {
    const RelAttr &relation_attr = select_sql.attributes[i];

    if (common::is_blank(relation_attr.relation_name) && 0 == strcmp(relation_attr.attribute_name, "*")) {
      if (relation_attr.aggr_type != NOT_AGGR) {
        // select count(*) from t;
        // 只有COUNT操作支持*
        if (relation_attr.aggr_type != COUNT) {
          return RC::MISMATCH;
        }
        size_t attr_len = sizeof(int);
        FieldMeta *field_meta = new FieldMeta;
        field_meta->init("COUNT(*)", INTS, attr_offset, attr_len, true);
        query_fields.push_back(Field(nullptr, field_meta, relation_attr.aggr_type, nullptr));
        attr_offset += attr_len;
      } else {
        for (Table *table : tables) {
          // select * from t;
          rc = wildcard_fields(table, query_fields, select_sql.is_aggr, group_by_keys, attr_offset);
          if (rc != RC::SUCCESS) {
            LOG_WARN("invalid field name while field is *.");
            return rc;
          }
        }
      }

    } else if (!common::is_blank(relation_attr.relation_name)) {  // select ID DOT ID from t;
      const char *table_name = relation_attr.relation_name;
      const char *field_name = relation_attr.attribute_name;

      if (0 == strcmp(table_name, "*")) {
        if (0 != strcmp(field_name, "*") || select_sql.is_aggr) {
          LOG_WARN("invalid field name while table is *. attr=%s", field_name);
          return RC::SCHEMA_FIELD_MISSING;
        }
        for (Table *table : tables) {
          wildcard_fields(table, query_fields, false, group_by_keys, attr_offset);
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
          wildcard_fields(table, query_fields, select_sql.is_aggr, group_by_keys, attr_offset);
        } else {
          // select t.a from t;
          const FieldMeta *field_meta = table->table_meta().field(field_name);
          if (nullptr == field_meta) {
            LOG_WARN("no such field. field=%s.%s.%s", db->name(), table->name(), field_name);
            return RC::SCHEMA_FIELD_MISSING;
          }

          if (select_sql.is_aggr) {
            // select min/max/sum/avg/count(t.a), b from t group by b;
            if (relation_attr.aggr_type == SUM && field_meta->type() == CHARS) {
              return RC::MISMATCH;
            }
            FieldMeta *aggr_field_meta = new FieldMeta;
            size_t attr_len;
            AttrType attr_type;
            char *aggr_field_name;
            if (relation_attr.aggr_type != NOT_AGGR) {
              aggr_field_name = new char[10 + strlen(relation_attr.attribute_name)];
              sprintf(aggr_field_name, "%s(%s)", aggr_name[relation_attr.aggr_type], relation_attr.attribute_name);
            } else {
              aggr_field_name = relation_attr.attribute_name;
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
            aggr_field_meta->init(aggr_field_name, attr_type, attr_offset, attr_len, true);
            query_fields.push_back(Field(table, aggr_field_meta, relation_attr.aggr_type, field_meta));
            attr_offset += attr_len;
          } else {
            query_fields.push_back(Field(table, field_meta));
          }
        }
      }
    } else {
      // 字段没有指定表名
      if (tables.size() != 1) {
        LOG_WARN("invalid. I do not know the attr's table. attr=%s", relation_attr.attribute_name);
        return RC::SCHEMA_FIELD_MISSING;
      }

      Table *table = tables[0];
      const FieldMeta *field_meta = table->table_meta().field(relation_attr.attribute_name);
      if (nullptr == field_meta) {
        LOG_WARN("no such field. field=%s.%s.%s", db->name(), table->name(), relation_attr.attribute_name);
        return RC::SCHEMA_FIELD_MISSING;
      }
      if (select_sql.is_aggr) {
        // select min/max/sum/avg/count(a), b from t group by b;
        if (relation_attr.aggr_type == SUM && field_meta->type() == CHARS) {
          return RC::MISMATCH;
        }
        FieldMeta *aggr_field_meta = new FieldMeta();
        size_t attr_len;
        AttrType attr_type;
        char *aggr_field_name;
        if (relation_attr.aggr_type != NOT_AGGR) {
          aggr_field_name = new char[10 + strlen(relation_attr.attribute_name)];
          sprintf(aggr_field_name, "%s(%s)", aggr_name[relation_attr.aggr_type], relation_attr.attribute_name);
        } else {
          aggr_field_name = strdup(relation_attr.attribute_name);
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
        aggr_field_meta->init(aggr_field_name, attr_type, attr_offset, attr_len, true);
        query_fields.push_back(Field(table, aggr_field_meta, relation_attr.aggr_type, field_meta));
        attr_offset += attr_len;
      } else {
        // select a from t;
        query_fields.push_back(Field(table, field_meta));
      }
    }
  }

  LOG_INFO("got %d tables in from stmt and %d fields in query stmt", tables.size(), query_fields.size());

  Table *default_table = nullptr;
  if (tables.size() == 1) {
    default_table = tables[0];
  }

  // create filter statement in `where` statement
  FilterStmt *filter_stmt = nullptr;
  rc = FilterStmt::create(db, default_table, &table_map, select_sql.conditions, select_sql.condition_num, filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
    return rc;
  }

  // create filter statement in `having` statement
  FilterStmt *having_stmt = nullptr;
  //   if (select_sql.is_aggr) {
  //     RC rc = HavingStmt::create(
  //         db, &table_map, select_sql.having_conditions, select_sql.having_condition_num, having_stmt);
  //     if (rc != RC::SUCCESS) {
  //       LOG_WARN("cannot construct filter stmt");
  //       return rc;
  //     }
  //   }

  // everything alright
  SelectStmt *select_stmt = new SelectStmt();
  select_stmt->tables_.swap(tables);
  select_stmt->query_fields_.swap(query_fields);
  select_stmt->filter_stmt_ = filter_stmt;
  select_stmt->do_aggr_ = select_sql.is_aggr;  // 告知执行器生成aggregate_operator
  select_stmt->having_stmt_ = having_stmt;
  select_stmt->group_keys_.swap(group_by_keys);
  stmt = select_stmt;
  return RC::SUCCESS;
}