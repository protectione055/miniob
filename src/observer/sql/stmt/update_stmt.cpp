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

#include "sql/stmt/update_stmt.h"
#include "sql/stmt/filter_stmt.h"
#include "sql/stmt/select_stmt.h"
#include "sql/planner/planner.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "storage/common/db.h"
#include "storage/common/table.h"
#include "sql/stmt/typecast.h"

RC UpdateStmt::create(Db *db, const Updates &update, Stmt *&stmt)
{
  if (db == nullptr) {
    LOG_WARN("invalid argument. db is null");
    return RC::INVALID_ARGUMENT;
  }

  const char *table_name = update.relation_name;

  if (table_name == nullptr) {
    LOG_WARN("invalid argument. relation name is null.");
    return RC::INVALID_ARGUMENT;
  }

  // does not support updating multiple tables at once
  // only support updating one attribute
  Table *table = db->find_table(update.relation_name);

  if (table == nullptr) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  // memory leaks go brrrrrrrr
  Value *values = new Value[MAX_NUM];
  char **attribute_names = new char*[MAX_NUM];
  for(int i=0;i<update.attribute_num;i++) {
    attribute_names[i] = strdup(update.attribute_names[i]);
    const FieldMeta *field_meta = table->table_meta().field(attribute_names[i]);
    if (field_meta == nullptr) {
      LOG_WARN("no such field. db=%s, table_name=%s, attribute_name=%s", db->name(), table_name, attribute_names[i]);
      return RC::SCHEMA_FIELD_NOT_EXIST;
    }

    Value v;
    // execute subquery and replace it with actual value
    if (update.marks[i] == SUB_QUERY) {
      Selects *select_sql = update.selects[i];
      if (select_sql->attr_num > 1) {
        LOG_WARN("too much columns in subquery");
        return RC::MISMATCH;
      }
      RC rc = execute_subquery_get_value(db, select_sql, v);
      if (rc != RC::SUCCESS) {
        LOG_WARN("subquery execution failed");
        return rc;
      }
    } else {
      v = update.values[i];
    }

    // check fields type
    AttrType field_type = field_meta->type();

    if(v.type != NULLS && v.type != field_type) {
      if(try_typecast(&v, v, field_type) != RC::SUCCESS) {
        LOG_WARN("field type mismatch. table=%s, field=%s, field type=%d, value_type=%d", 
                table_name, field_meta->name(), field_type, v.type);
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
    }
    values[i] = v;
  }

  // create filter statement in `where` statement
  std::unordered_map<std::string, Table *> table_map;
  table_map[update.relation_name] = table;
  FilterStmt *filter_stmt = nullptr;
  RC rc = FilterStmt::create(db, table, &table_map,
          update.conditions, update.condition_num, filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
    return rc;
  }

  UpdateStmt *update_stmt = new UpdateStmt();
  update_stmt->table_ = table;
  update_stmt->values_ = values;
  update_stmt->attribute_num_ = update.attribute_num;
  update_stmt->attribute_names_ = (const char**)attribute_names;
  update_stmt->filter_stmt_ = filter_stmt;
  stmt = update_stmt;
  return RC::SUCCESS;
}

// 构造并执行子查询，检查是否只有一行一列，从Tuple获取Value
RC UpdateStmt::execute_subquery_get_value(Db *db, Selects *select_sql, Value &value)
{
  RC rc = RC::SUCCESS;
  Query query;
  query.flag = SCF_SELECT;
  query.sstr.selection = *select_sql;
  Stmt *select_stmt = nullptr;
  rc = Stmt::create_stmt(db, query, select_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create SelectStmt for update sbuqery");
    return rc;
  }

  Planner planner(select_stmt);
  Operator *root = nullptr;
  rc = planner.create_executor(root);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create executor for update sbuqery");
    return rc;
  }
  rc = root->open();
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open executor for update sbuqery");
    return rc;
  }

  int tuple_count = 0;
  Tuple *tuple = nullptr;
  while (root->next() == RC::SUCCESS) {
    if (tuple_count > 0) {
      LOG_WARN("more than one row was returneds in update subquery");
      return RC::INVALID_ARGUMENT;
    }
    tuple = root->current_tuple();
    tuple_count++;
  }

  if (tuple->cell_num() > 1) {
    LOG_WARN("too many column in update subquery result tuple. cell_num=%d", tuple->cell_num());
    return RC::INVALID_ARGUMENT;
  }

  TupleCell tuple_cell;
  tuple->cell_at(0, tuple_cell);
  value.type = tuple_cell.attr_type();
  value.data = (void *)(tuple_cell.data());  // Not safe, do this for now
  return rc;
}
