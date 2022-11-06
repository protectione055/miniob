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

#pragma once

#include "rc.h"
#include "sql/stmt/stmt.h"

class Table;
class FilterStmt;

class UpdateStmt : public Stmt
{
public:

  UpdateStmt() = default;

  StmtType type() const override { return StmtType::UPDATE; }
public:
  static RC create(Db *db, const Updates &update_sql, Stmt *&stmt);

public:
  Table *table() const {return table_;}
  const char **attributes() { return attribute_names_; }
  const Value *values() const { return values_; }
  int attribute_num() const { return attribute_num_; }
  FilterStmt *filter_stmt() const { return filter_stmt_; }

private:
  static RC execute_subquery_get_value(Db *db, Selects *select_sql, Value &value);

  Table *table_ = nullptr;
  int attribute_num_;
  const char **attribute_names_ = nullptr;
  Value *values_;
  FilterStmt *filter_stmt_ = nullptr;
};

