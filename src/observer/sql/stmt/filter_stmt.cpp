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

#include "rc.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "sql/stmt/select_stmt.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/common/db.h"
#include "storage/common/table.h"
#include "sql/expr/subquery_expression.h"

FilterStmt::~FilterStmt()
{
  for (FilterUnit *unit : filter_units_) {
    delete unit;
  }
  filter_units_.clear();
}

RC FilterStmt::create(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
		      const Condition *conditions, int condition_num,
		      FilterStmt *&stmt)
{
  RC rc = RC::SUCCESS;
  stmt = nullptr;

  FilterStmt *tmp_stmt = new FilterStmt();
  for (int i = 0; i < condition_num; i++) {
    FilterUnit *filter_unit = nullptr;
    
    rc = create_filter_unit(db, default_table, tables, conditions[i], filter_unit);
    
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
		       const RelAttr &attr, Table *&table, const FieldMeta *&field)
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
  if (condition.left_expr_type == ATTR) {
    Table *table = nullptr;
    const FieldMeta *field = nullptr;
    rc = get_table_and_field(db, default_table, tables, condition.left_attr, table, field);
    if (rc != RC::SUCCESS) {
      LOG_WARN("cannot find attr");
      return rc;
    }
    left_type = field->type();
    left = new FieldExpr(table, field);
  } else if (condition.left_expr_type == VALUE) {
    left_type = condition.left_value.type;
    left = new ValueExpr(condition.left_value);
  } else if (condition.left_expr_type == SUB_QUERY) {
    Stmt *stmt = nullptr;
    Query query;
    query.flag = SCF_SELECT;
    query.sstr.selection = *(Selects *)condition.left_query;
    Stmt::create_stmt(db, query, stmt);
    if (dynamic_cast<SelectStmt *>(stmt)->query_fields().size() > 1) {
      LOG_WARN("too much argument in subquery");
      return RC::INVALID_ARGUMENT;
    }
    left = new SubQueryExpr(stmt);
  }

  if (condition.right_expr_type == ATTR) {
    Table *table = nullptr;
    const FieldMeta *field = nullptr;
    rc = get_table_and_field(db, default_table, tables, condition.right_attr, table, field);
    if (rc != RC::SUCCESS) {
      LOG_WARN("cannot find attr");
      delete left;
      return rc;
    }
    right_type = field->type();
    right = new FieldExpr(table, field);
  } else if (condition.right_expr_type == VALUE) {
    right_type = condition.right_value.type;
    right = new ValueExpr(condition.right_value);
  } else if (condition.right_expr_type == SUB_QUERY) {
    Stmt *stmt = nullptr;
    Query query;
    query.flag = SCF_SELECT;
    query.sstr.selection = *(Selects *)condition.right_query;
    Stmt::create_stmt(db, query, stmt);
    if (dynamic_cast<SelectStmt *>(stmt)->query_fields().size() > 1) {
      LOG_WARN("too much argument in subquery");
      return RC::INVALID_ARGUMENT;
    }
    right = new SubQueryExpr(stmt);
  }

  filter_unit = new FilterUnit;
  filter_unit->set_comp(comp);
  filter_unit->set_left(left);
  filter_unit->set_right(right);

  return rc;
}
