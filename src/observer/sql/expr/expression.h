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

#pragma once

#include <string.h>
#include <unordered_map>
#include <vector>
#include "storage/common/field.h"
#include "sql/expr/tuple_cell.h"
#include "sql/parser/parse_defs.h"
#include "sql/stmt/typecast.h"
#include "sql/stmt/stmt.h"

class Tuple;

enum class ExprType {
  NONE,
  FIELD,
  VALUE,
  COMPLEX, 
  SUB_QUERY,
};

class Expression
{
public: 
  Expression() = default;
  virtual ~Expression() = default;
  
  virtual RC get_value(const Tuple &tuple, TupleCell &cell) const = 0;
  virtual ExprType type() const = 0;
};

class FieldExpr : public Expression
{
public:
  FieldExpr() = default;
  FieldExpr(const Table *table, const FieldMeta *field) : field_(table, field)
  {}

  virtual ~FieldExpr() = default;

  ExprType type() const override
  {
    return ExprType::FIELD;
  }

  Field &field()
  {
    return field_;
  }

  const Field &field() const
  {
    return field_;
  }

  const char *table_name() const
  {
    return field_.table_name();
  }

  const char *field_name() const
  {
    return field_.field_name();
  }

  void set_associated_value(TupleCell &cell);

  RC get_value(const Tuple &tuple, TupleCell &cell) const override;
private:
  Field field_;
  TupleCell associated_query_cell_;		//保存与外查询关联的值
};

class ValueExpr : public Expression
{
public:
  ValueExpr() = default;
  ValueExpr(const Value &value) : tuple_cell_(value.type, (char *)value.data)
  {
    if (value.type == CHARS) {
      tuple_cell_.set_length(strlen((const char *)value.data));
    } if (value.type == DATES) {
      tuple_cell_.set_length(sizeof(time_t));
    } else { // ints and floats, dirty magic number
      tuple_cell_.set_length(4);
    }
  }

  virtual ~ValueExpr() = default;

  RC get_value(const Tuple &tuple, TupleCell & cell) const override;
  ExprType type() const override
  {
    return ExprType::VALUE;
  }

  void get_tuple_cell(TupleCell &cell) const {
    cell = tuple_cell_;
  }

private:
  TupleCell tuple_cell_;
};

class ComplexExpr : public Expression
{
public:
  ComplexExpr() = default;
  ComplexExpr(Expression *left, Expression *right, MathOp op)
  :left_(left), right_(right), op_(op)
  {}

  virtual ~ComplexExpr()
  {

  }

  ExprType type() const override
  {
    return ExprType::COMPLEX;
  }

  static Expression *create_complex_expr(
    const char* expr_str, 
    std::unordered_map<std::string, Table *> &table_map, 
    Table *default_table,
    std::vector<Field> &expr_aggr,
    size_t &attr_offset);

  RC get_value(const Tuple &tuple, TupleCell &cell) const override;

private:
  Expression *left_;
  Expression *right_;
  MathOp op_;
};
