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

  const FieldMeta *field_meta = table->table_meta().field(update.attribute_name);
  if (field_meta == nullptr) {
    LOG_WARN("no such field. db=%s, table_name=%s, attribute_name=%s", db->name(), table_name, update.attribute_name);
    return RC::SCHEMA_FIELD_NOT_EXIST;
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

  // check fields type
  AttrType field_type = field_meta->type();
  Value v = update.value;
  if(v.type != field_type) {
    if(try_typecast(&v, v, field_type) != RC::SUCCESS) {
      LOG_WARN("field type mismatch. table=%s, field=%s, field type=%d, value_type=%d", 
              table_name, field_meta->name(), field_type, v.type);
      return RC::SCHEMA_FIELD_TYPE_MISMATCH;
    }
  }

  UpdateStmt *update_stmt = new UpdateStmt();
  update_stmt->table_ = table;
  update_stmt->value_ = v;
  update_stmt->attribute_name_ = update.attribute_name;
  update_stmt->filter_stmt_ = filter_stmt;
  stmt = update_stmt;
  return RC::SUCCESS;
}
