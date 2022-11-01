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
// Created by Wangyunlai on 2022/6/5.
//

#pragma once

#include <vector>

#include "rc.h"
#include "sql/stmt/stmt.h"
#include "storage/common/field.h"
#include "sql/expr/expression.h"

#define MAX_NUM 20

class FieldMeta;
class FilterStmt;
class Db;
class Table;

class SelectStmt : public Stmt
{
public:

  SelectStmt() = default;
  ~SelectStmt() override;

  StmtType type() const override { return StmtType::SELECT; }
public:
  static RC create(Db *db, const Selects &select_sql, Stmt *&stmt);

public:
  const std::vector<Table *> &tables() const { return tables_; }
  const std::vector<Field> &query_fields() const { return query_fields_; }
  const std::vector<std::tuple<FieldExpr, int>> &order_fields() const { return order_fields_; }
  FilterStmt *filter_stmts(int index) const { return filter_stmts_[index]; }
  FilterStmt *join_stmt() const { return join_stmt_; }
  FilterStmt *having_stmt() const
  {
    return having_stmt_;
  }
  const std::vector<Field> &group_keys() const
  {
    return group_keys_;
  }
  const bool do_aggregate() const {return do_aggr_;}

private:
  std::vector<Field> query_fields_;
  std::vector<std::tuple<FieldExpr, int>> order_fields_;
  std::vector<Table *> tables_;
  std::vector<FilterStmt *>filter_stmts_;
  FilterStmt *join_stmt_ = nullptr;
  bool do_aggr_ = false;
  FilterStmt *having_stmt_ = nullptr;
  std::vector<Field> group_keys_;
};
