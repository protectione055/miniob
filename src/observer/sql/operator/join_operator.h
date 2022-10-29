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
  JoinOperator(Operator *left, Operator *right, const FilterUnit *join_cond)
  {
    left_ = left;
    right_ = right;
    join_cond_ = join_cond;
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
  }

  RC open() override;
  RC next() override;
  RC close() override;

  Tuple * current_tuple() override;
  
  static Operator * create_join_tree(std::unordered_map<std::string, Operator*> table_operator_map, FilterStmt *join_conds);

private:
  Operator *left_ = nullptr;
  Operator *right_ = nullptr;
  bool hashtable_created_ = false;
  JoinState join_state_ = BUILD_TABLE;
  FilterUnit *join_cond_ = nullptr;
  std::unordered_map<std::string, std::vector<Tuple *>> hash_table_;
  std::vector<Tuple *> result_table_;
  std::vector<Tuple *> cur_bucket_;
  TempTuple *right_tuple_ = nullptr;
  int pos_ = 0;
};
