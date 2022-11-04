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
// Created by Wangyunlai on 2022/07/05.
//

#include "sql/expr/tuple.h"

void FieldExpr::set_associated_value(TupleCell &cell)
{
  if (cell.attr_type() == NULL) {
    associated_query_cell_.set_null();
    return;
  }
  associated_query_cell_.set_type(cell.attr_type());
  associated_query_cell_.set_data((char *)cell.data());
  associated_query_cell_.set_length(cell.length());
}

RC FieldExpr::get_value(const Tuple &tuple, TupleCell &cell) const
{
  RC rc = rc = tuple.find_cell(field_, cell);
  if (rc == RC::NOTFOUND && associated_query_cell_.attr_type() != UNDEFINED) {
    cell = associated_query_cell_;
    return RC::SUCCESS;
  }
  return rc;
}

RC ValueExpr::get_value(const Tuple &tuple, TupleCell & cell) const
{
  cell = tuple_cell_;
  return RC::SUCCESS;
}