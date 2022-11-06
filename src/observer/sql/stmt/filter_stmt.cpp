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
// Created by Wangyunlai on 2022/5/22.
//
#include <algorithm>

#include "rc.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "sql/stmt/select_stmt.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/common/db.h"
#include "storage/common/table.h"
#include "common/lang/bitmap.h"
#include "sql/expr/subquery_expression.h"

FilterStmt::~FilterStmt()
{
  for (FilterUnit *unit : filter_units_) {
    delete unit;
  }
  filter_units_.clear();
}

RC FilterStmt::create(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
    const Condition *conditions, int condition_num, FilterStmt *&stmt, bool is_subquery)
{
  RC rc = RC::SUCCESS;
  stmt = nullptr;

  FilterStmt *tmp_stmt = new FilterStmt();
  for (int i = 0; i < condition_num; i++) {
    FilterUnit *filter_unit = nullptr;

    rc = create_filter_unit(db, default_table, tables, conditions[i], filter_unit, is_subquery);

    if (rc != RC::SUCCESS) {
      delete tmp_stmt;
      LOG_WARN("failed to create filter unit. condition index=%d", i);
      return rc;
    }
    tmp_stmt->filter_units_.push_back(filter_unit);
  }

  stmt = tmp_stmt;
  return rc;
}

RC get_table_and_field(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
    const RelAttr &attr, Table *&table, const FieldMeta *&field, bool is_subquery)
{
  if (common::is_blank(attr.relation_name)) {
    // table = default_table;
    for (auto table_info : *tables) {
      if (!table && table_info.second->table_meta().field(attr.attribute_name)) {
        table = table_info.second;
      } else if (table && table_info.second->table_meta().field(attr.attribute_name)) {
        LOG_WARN("invalid. I do not know the attr's table. attr=%s", attr.attribute_name);
        return SCHEMA_FIELD_MISSING;
      }
    }
  } else if (nullptr != tables) {
    auto iter = tables->find(std::string(attr.relation_name));
    if (iter != tables->end()) {
      table = iter->second;
    }
  } else {
    table = db->find_table(attr.relation_name);
  }

  if (is_subquery && table == nullptr) {
    table = db->find_table(attr.relation_name);
  }

  if (nullptr == table) {
    LOG_WARN("No such table: attr.relation_name: %s", attr.relation_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  field = table->table_meta().field(attr.attribute_name);
  if (nullptr == field) {
    LOG_WARN("no such field in table: table %s, field %s", table->name(), attr.attribute_name);
    table = nullptr;
    return RC::SCHEMA_FIELD_NOT_EXIST;
  }

  return RC::SUCCESS;
}

RC FilterStmt::create_filter_unit(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
    const Condition &condition, FilterUnit *&filter_unit, bool is_subquery)
{
  RC rc = RC::SUCCESS;
  
  CompOp comp = condition.comp;
  if (comp < EQUAL_TO || comp >= NO_OP) {
    LOG_WARN("invalid compare operator : %d", comp);
    return RC::INVALID_ARGUMENT;
  }

  // 检查条件表达式是否合法
  switch (condition.comp) {
    case IN:
    case NOT_IN:
      // IN/NOT_IN运算符中，左子式只能有一行结果，右子式必须是子查询
      if (condition.left_expr_type == SUB_QUERY && !((Selects *)condition.left_query)->is_aggr ||
          condition.right_expr_type != SUB_QUERY && condition.right_expr_type != VALUE_LIST) {
        return RC::INVALID_ARGUMENT;
      }
      break;
    default:
      break;
  }

  Expression *left = nullptr;
  Expression *right = nullptr;
  AttrType left_type, right_type;
  if (condition.left_expr_type == ATTR) {
    if (!condition.left_attr.is_complex) {
      Table *table = nullptr;
      const FieldMeta *field = nullptr;
      rc = get_table_and_field(db, default_table, tables, condition.left_attr, table, field, is_subquery);
      if (rc != RC::SUCCESS) {
        LOG_WARN("cannot find attr");
        return rc;
      }
      left_type = field->type();
      left = new FieldExpr(table, field);
    } else {
      std::vector<Field> expr_aggr; // shit
      size_t attr_offset;
      left = ComplexExpr::create_complex_expr(condition.left_attr.attribute_name, *tables, default_table, expr_aggr, attr_offset);
    }
  } else if (condition.left_expr_type == VALUE) {
    left_type = condition.left_value.type;
    left = new ValueExpr(condition.left_value);
  } else if (condition.left_expr_type == SUB_QUERY) {
    Stmt *stmt = nullptr;
    Query query;
    query.flag = SCF_SELECT;
    query.sstr.selection = *(Selects *)condition.left_query;
    Stmt::create_stmt(db, query, stmt);
    if (rc != RC::SUCCESS || dynamic_cast<SelectStmt *>(stmt)->query_fields().size() > 1) {
      LOG_WARN("invalid argument in subquery");
      return RC::INVALID_ARGUMENT;
    }
    left = new SubQueryExpr();
    rc = ((SubQueryExpr *)left)->init_with_subquery_stmt(stmt);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to init_with_subquery_stmt");
    }
  }

  if (condition.right_expr_type == ATTR) {
    if (!condition.right_attr.is_complex) {
      Table *table = nullptr;
      const FieldMeta *field = nullptr;
      rc = get_table_and_field(db, default_table, tables, condition.right_attr, table, field, is_subquery);
      if (rc != RC::SUCCESS) {
        LOG_WARN("cannot find attr");
        delete left;
        return rc;
      }
      right_type = field->type();
      right = new FieldExpr(table, field);
    } else {
      std::vector<Field> expr_aggr; // shit
      size_t attr_offset;
      right = ComplexExpr::create_complex_expr(condition.right_attr.attribute_name, *tables, default_table, expr_aggr, attr_offset);
    }
  } else if (condition.right_expr_type == VALUE) {
    right_type = condition.right_value.type;
    right = new ValueExpr(condition.right_value);
  } else if (condition.right_expr_type == SUB_QUERY) {
    Stmt *stmt = nullptr;
    Query query;
    query.flag = SCF_SELECT;
    query.sstr.selection = *(Selects *)condition.right_query;
    rc = Stmt::create_stmt(db, query, stmt);
    if (rc != RC::SUCCESS || dynamic_cast<SelectStmt *>(stmt)->query_fields().size() > 1) {
      LOG_WARN("invalid argument in subquery");
      return RC::INVALID_ARGUMENT;
    }
    right = new SubQueryExpr();
    rc = ((SubQueryExpr *)right)->init_with_subquery_stmt(stmt);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to init_with_subquery_stmt");
    }
  } else if (condition.right_expr_type == VALUE_LIST) {
    assert(condition.left_expr_type == ATTR);
    right = new SubQueryExpr();
    rc = ((SubQueryExpr *)right)->init_with_value_list((FieldExpr *)left, condition.values, condition.value_num);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to init_with_value_list");
    }
  }

  filter_unit = new FilterUnit;
  filter_unit->set_comp(comp);
  filter_unit->set_left(left);
  filter_unit->set_right(right);

  return rc;
}

// 将conditions中可以下推的谓语下推到scan_operator上
RC FilterStmt::create_push_down_filters(Db *db, Table *table, std::unordered_map<std::string, Table *> *tables,
    const Condition *conditions, int condition_num, FilterStmt *&stmt, common::Bitmap &bitmap, CondMode mode)
{
  RC rc = RC::SUCCESS;
  stmt = nullptr;

  FilterStmt *tmp_stmt = new FilterStmt();
  tmp_stmt->set_filter_mode(mode);
  for (int i = 0; i < condition_num; i++) {
    FilterUnit *filter_unit = nullptr;
    // 子查询不进行下推
    if (conditions[i].left_expr_type == SUB_QUERY || conditions[i].right_expr_type == SUB_QUERY) {
      continue;
    }
    rc = create_table_filter_unit(db, table, tables, conditions[i], filter_unit);
    if (rc != RC::SUCCESS) {
      if (rc == RC::MISMATCH) continue;
      delete tmp_stmt;
      LOG_WARN("failed to create filter unit. condition index=%d", i);
      return rc;
    }
    bitmap.set_bit(i);  // 标记第i个谓词已经被下推
    tmp_stmt->filter_units_.push_back(filter_unit);
  }

  stmt = tmp_stmt;
  return RC::SUCCESS;
}

RC FilterStmt::create_table_filter_unit(Db *db, Table *table, std::unordered_map<std::string, Table *> *tables,
    const Condition &condition, FilterUnit *&filter_unit)
{
  RC rc = RC::SUCCESS;
  
  CompOp comp = condition.comp;
  if (comp < EQUAL_TO || comp >= NO_OP) {
    LOG_WARN("invalid compare operator : %d", comp);
    return RC::INVALID_ARGUMENT;
  }

  Expression *left = nullptr;
  Expression *right = nullptr;
  AttrType left_type, right_type;
  if (condition.left_is_attr) {
    if(strcmp(condition.left_attr.relation_name, table->name())!=0) return RC::MISMATCH;
    Table *temp = nullptr;
    const FieldMeta *field = nullptr;
    rc = get_table_and_field(db, nullptr, tables, condition.left_attr, temp, field, false);
    if (rc != RC::SUCCESS) {
      LOG_WARN("cannot find attr");
      return rc;
    }
    left_type = field->type();
    left = new FieldExpr(table, field);
  } else {
    left_type = condition.left_value.type;
    left = new ValueExpr(condition.left_value);
  }

  if (condition.right_is_attr) {
    if (strcmp(condition.right_attr.relation_name, table->name()) != 0)
      return RC::MISMATCH;
    Table *temp = nullptr;
    const FieldMeta *field = nullptr;
    rc = get_table_and_field(db, nullptr, tables, condition.right_attr, temp, field, false);
    if (rc != RC::SUCCESS) {
      LOG_WARN("cannot find attr");
      delete left;
      return rc;
    }
    right_type = field->type();
    right = new FieldExpr(table, field);
  } else {
    right_type = condition.right_value.type;
    right = new ValueExpr(condition.right_value);
  }

  filter_unit = new FilterUnit;
  filter_unit->set_comp(comp);
  filter_unit->set_left(left);
  filter_unit->set_right(right);

  return rc;
}