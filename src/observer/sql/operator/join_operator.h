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
// Created by WangYunlai on 2021/6/10.
//

#pragma once

#include "common/lang/string.h"
#include "sql/parser/parse.h"
#include "sql/operator/operator.h"
#include "sql/stmt/filter_stmt.h"
#include "rc.h"

enum JoinState{
  BUILD_TABLE,
  SCAN_BUCKET,
  GET_NEW_TUPLE
};

// TODO fixme
class JoinOperator : public Operator
{
public:
  JoinOperator(Operator *left, Operator *right, std::vector<const FilterUnit *> join_conds)
  {
    left_ = left;
    right_ = right;
    join_conds_.swap(join_conds);
  }

  ~JoinOperator()
  {
    for (auto i : result_table_)
    {
      delete i;
    }
    for (auto i : hash_table_) {
      for(auto j : i.second) {
        delete j;
      }
    }
    delete left_;
    delete right_;
  }

  RC open() override;
  RC next() override;
  RC close() override;

  Tuple * current_tuple() override;
  
  static Operator * create_join_tree(std::unordered_map<std::string, Operator*> table_operator_map, FilterStmt *join_conds);
  bool do_predicate(Tuple *tuple);
public:
  static std::unordered_map<std::string, std::string> fathers;

private:
  Operator *left_ = nullptr;
  Operator *right_ = nullptr;
  JoinState join_state_ = BUILD_TABLE;
  std::vector<const FilterUnit *> join_conds_;
  std::unordered_map<std::string, std::vector<Tuple *>> hash_table_;
  std::vector<Tuple *> result_table_;
  std::vector<Tuple *> cur_bucket_;
  Tuple *right_tuple_ = nullptr;
  int pos_ = 0;

  bool do_nestloop_ = true;
  std::vector<Tuple *> left_tuples_;
};
