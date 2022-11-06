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
#include "sql/stmt/filter_stmt.h"
#include "sql/stmt/having_stmt.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "storage/common/db.h"
#include "storage/common/table.h"
#include "common/lang/bitmap.h"

#define MAX_NUM 20

class FieldMeta;
class FilterStmt;
class Db;
class Table;

const char *const aggr_name[] = {
    "NOT_AGGR",
    "MIN",
    "MAX",
    "SUM",
    "COUNT",
    "AVG",
};

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
  const std::vector<Expression *> &query_exprs() const { return query_exprs_; }
  const std::vector<std::tuple<FieldExpr, int>> &order_fields() const { return order_fields_; }

  FilterStmt *filter_stmt() const
  {
    return filter_stmt_;
  }

  void set_filter_stmt(FilterStmt *filter_stmt)
  {
    filter_stmt_ = filter_stmt;
  }

  FilterStmt *push_down_filter_stmts(int index) const
  {
    if (push_down_filter_stmts_.size() == 0) {
      return nullptr;
    }
    return push_down_filter_stmts_[index];
  }

  FilterStmt *join_keys() const
  {
    return join_keys_;
  }
  FilterStmt *expr_stmt() const { return expr_stmt_; }

  HavingStmt *having_stmt() const
  {
    return having_stmt_;
  }

  const std::vector<Field> &group_keys() const
  {
    return group_keys_;
  }

  const bool do_aggregate() const {return do_aggr_;}

  const std::string table_alias(const std::string table_name) const
  {
    auto iter = table_name_alias_map_.find(table_name);
    if (iter == table_name_alias_map_.end()) {
      return "";
    }
    return iter->second;
  }

private:
  std::vector<Field> query_fields_;
  std::vector<Expression *> query_exprs_;
  std::vector<std::tuple<FieldExpr, int>> order_fields_;
  std::vector<Table *> tables_;
  FilterStmt *filter_stmt_;
  std::vector<FilterStmt *> push_down_filter_stmts_;
  FilterStmt *join_keys_ = nullptr;
  bool do_aggr_ = false;
  HavingStmt *having_stmt_ = nullptr;
  FilterStmt *expr_stmt_ = nullptr;
  std::vector<Field> group_keys_;
  std::map<std::string, std::string> table_name_alias_map_;
};

bool find_rel_attr(const RelAttr *rel_attr_collection, RelAttr target_rel_attr, size_t collections_size);
RC init_and_create_having_stmt(Db *db, const Selects &select_sql, const std::vector<Table *> &tables,
    const std::unordered_map<std::string, Table *> &table_map, std::vector<Field> &group_by_keys,
    std::vector<Field> &query_fields, size_t &attr_offset, HavingStmt *&having_stmt);
RC collect_groupby_keys(Db *db, const Selects &select_sql, std::vector<Table *> &tables,
    std::unordered_map<std::string, Table *> &table_map, std::vector<Field> &group_by_keys);
RC create_filter_for_where_stmt(Db *db, const Selects &select_sql, std::vector<Table *> &tables,
    std::unordered_map<std::string, Table *> &table_map, FilterStmt *&reserved_filter_stmt,
    std::vector<FilterStmt *> &push_down_filter_stmts);
RC collect_fields_in_orderby_stmt(Db *db, const Selects &select_sql, std::vector<Table *> &tables,
    std::unordered_map<std::string, Table *> &table_map, std::vector<std::tuple<FieldExpr, int>> &order_fields);
RC collect_tables_in_from_statement(Db *db, const Selects &select_sql, std::vector<Table *> &tables,
    std::unordered_map<std::string, Table *> &table_map, std::map<std::string, std::string> &alias_map);
RC create_query_field(Table *table, const Selects &select_sql, const FieldMeta *field_meta,
    const RelAttr &relation_attr, std::vector<Field> &group_by_keys, size_t &attr_offset,
    std::vector<Field> &query_fields, bool visible);
RC wildcard_fields(Table *table, std::vector<Field> &field_metas, bool is_aggr, std::vector<Field> &group_by_keys,
    size_t &attr_offset, const char *alias);
RC check_field_in_group(bool is_aggr, const FieldMeta *field_meta, const std::vector<Field> &group_by_keys);
RC find_table_by_attr_name(const std::vector<Table *> &tables, Table *&table, const RelAttr &rel_attr);
RC process_simple_attr(const Selects &select_sql, const RelAttr &relation_attr, const std::vector<Table *> &tables,
    std::vector<Field> &group_by_keys, size_t &attr_offset, std::vector<Field> &query_fields, bool visible);
RC process_attr_with_dot(const Selects &select_sql, const RelAttr &relation_attr, std::vector<Table *> &tables,
    std::unordered_map<std::string, Table *> &table_map, const char *table_name, const char *field_name,
    std::vector<Field> &group_by_keys, size_t &attr_offset, std::vector<Field> &query_fields, bool visible);
RC process_attr_with_star(const Selects &select_sql, const RelAttr &relation_attr, size_t &attr_offset,
    const std::vector<Table *> &tables, std::vector<Field> &group_by_keys, std::vector<Field> &query_fields,
    bool visible);
RC collect_rel_attr_into_query_fields(const RelAttr &relation_attr, Db *db, const Selects &select_sql,
    std::vector<Table *> tables, std::unordered_map<std::string, Table *> table_map, size_t &attr_offset,
    std::vector<Field> group_by_keys, std::vector<Field> &query_fields, bool visible = false);