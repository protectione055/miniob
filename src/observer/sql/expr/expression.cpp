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

RC ValueExpr::get_value(const Tuple &tuple, TupleCell &cell) const
{
  cell = tuple_cell_;
  return RC::SUCCESS;
}

RC ComplexExpr::get_value(const Tuple &tuple, TupleCell &cell) const
{
  TupleCell left_cell;
  TupleCell right_cell;
  left_->get_value(tuple, left_cell);
  right_->get_value(tuple, right_cell);

  // 先默认为NULLS，孩子若有NULLS或除数为0则直接返回
  cell.set_type(NULLS);
  if (left_cell.attr_type() == NULLS || right_cell.attr_type() == NULLS) return RC::SUCCESS;

  Value left_value{left_cell.attr_type(), (void*)left_cell.data()};
  Value right_value{right_cell.attr_type(), (void*)right_cell.data()};
  if (left_cell.attr_type() != FLOATS)
    try_typecast(&left_value, Value{left_cell.attr_type(), (void*)left_cell.data()}, FLOATS);
  if (right_cell.attr_type() != FLOATS)
    try_typecast(&right_value, Value{right_cell.attr_type(), (void*)right_cell.data()}, FLOATS);

  float left, right, res;
  left  = *(float*)left_value.data;
  right = *(float*)right_value.data;
  switch (op_)
  {
  case MathOp::PLUS:
    res = left + right;
    break;
  
  case MathOp::MINUS:
    res = left - right;
    break;
  
  case MathOp::MULTIPLY:
    res = left * right;
    break;
    
  case MathOp::DIVIDE:
    if (right == 0.0) return RC::SUCCESS;
    res = left / right;
    break;
  default:
    break;
  }

  char *res_value = (char *)malloc(sizeof(float));
  memcpy(res_value, &res, sizeof(float));
  cell.set_data(res_value);
  cell.set_type(FLOATS);
  cell.set_length(sizeof(float));
  return RC::SUCCESS;
}

void get_num(const char* expr_str, int &i){
  while (expr_str[i]>='0' &&  expr_str[i]<='9') i++;
  if (expr_str[i] == '.') {
    i++;
    while (expr_str[i]>='0' &&  expr_str[i]<='9') i++;
  }
}

char *get_name(const char* expr_str, int &i){
  int start = i;
  while ((expr_str[i]>='a' &&  expr_str[i]<='z') || 
         (expr_str[i]>='A' &&  expr_str[i]<='Z') ||
         (expr_str[i]>='0' &&  expr_str[i]<='9') ||
         expr_str[i]>='_') 
    i++;
  
  char *name;
  name = new char[i - start + 1];
  name[i - start] = '\0';
  memcpy(name, expr_str+start, i - start);

  return name;
}

void get_ComplexExpr(std::vector<Expression *> &expr_stack, std::vector<char> &op_stack){
  
  Expression *left, *right, *res;
  right = expr_stack.back();
  expr_stack.pop_back();
  left = expr_stack.back();
  expr_stack.pop_back();

  char op = op_stack.back();
  op_stack.pop_back();

  switch (op)
  {
    case '+':
      res = new ComplexExpr(left, right, MathOp::PLUS);
      break;
    case 'n':
    case '-':
      res = new ComplexExpr(left, right, MathOp::MINUS);
      break;
    case '*':
      res = new ComplexExpr(left, right, MathOp::MULTIPLY);
      break;
    case '/':
      res = new ComplexExpr(left, right, MathOp::DIVIDE);
      break;
      
  default:
    LOG_ERROR("Wrong Math Operator!");
  }
  expr_stack.push_back(res);
}

Expression *ComplexExpr::create_complex_expr(const char* expr_str, std::unordered_map<std::string, Table *> &table_map, Table *table, std::vector<Field> &expr_aggr, size_t &attr_offset)
{
  std::vector<Expression *> expr_stack;
  std::vector<char> op_stack;
  
  bool new_expression = true; //用于判断负号为运算符还是negetive，如'-ID' 、'-value'
  int start, end;
  for (int i = 0; i<strlen(expr_str);){
    if (expr_str[i]>='0' &&  expr_str[i]<='9'){
      start = i;
      get_num(expr_str, i);
      char *num = new char[i - start + 1];
      num[i - start] = '\0';
      memcpy(num, expr_str+start, i - start);
      expr_stack.push_back(new ValueExpr(Value{CHARS, num}));
      new_expression = false;
      continue;
    } 

    if ((expr_str[i]>='a' &&  expr_str[i]<='z') || (expr_str[i]>='A' &&  expr_str[i]<='Z')) { //可能是ID或聚合
      
      char *attr_name, *relation_name, *aggr;
      bool is_aggr = false;
      int start = i;
      char *str = get_name(expr_str, i);
      
      if (expr_str[i] == '(') {
        is_aggr = true;
        aggr = str;
        str = get_name(expr_str, ++i);
      }

      if (expr_str[i] == '.') {
        relation_name = str;
        str = get_name(expr_str, ++i);
        table = table_map[relation_name];
        delete[] relation_name;
      }
      
      if (!is_aggr) {
        attr_name = str;
        const FieldMeta *field_meta = nullptr;
        field_meta = table->table_meta().field(attr_name);
        expr_stack.push_back(new FieldExpr(table, field_meta));
        new_expression = false;

      } else {
        int attr_len;
        AttrType attr_type;
        const FieldMeta *field_meta = nullptr;

        FieldMeta *aggr_field_meta = new FieldMeta();
        AggrType aggr_type = NOT_AGGR;
        attr_name = str;
        field_meta = table->table_meta().field(attr_name);
        
        char *aggr_name;
        i++;  //聚合，跳过 ')'
        aggr_name = new char[i - start + 1];
        aggr_name[i - start] = '\0';
        memcpy(aggr_name, expr_str+start, i - start);

        if (strcmp(aggr, "count") == 0)    aggr_type = COUNT;
        else if (strcmp(aggr, "min") == 0) aggr_type = MIN;
        else if (strcmp(aggr, "max") == 0) aggr_type = MAX;
        else if (strcmp(aggr, "sum") == 0) aggr_type = SUM;
        else if (strcmp(aggr, "avg") == 0) aggr_type = AVG;
        switch (aggr_type) {
          case NOT_AGGR:
          case MIN:
          case MAX:
          case SUM:
            attr_len = field_meta->len();
            attr_type = field_meta->type();
            break;
          case AVG:
            attr_len = sizeof(float);
            attr_type = FLOATS;
            break;
          case COUNT:
            attr_len = sizeof(int);
            attr_type = INTS;
            break;
        }

        aggr_field_meta->init(aggr_name, attr_type, attr_offset, attr_len, false, field_meta->nullable());
        expr_aggr.push_back(Field(table, aggr_field_meta, aggr_type, field_meta));
        expr_stack.push_back(new FieldExpr(table, aggr_field_meta));

        delete[] aggr;
        attr_offset += attr_len;
      }

      new_expression = false;
      delete[] str;
      continue;
    }

    if ((expr_str[i]>='a' &&  expr_str[i]<='z') || (expr_str[i]>='A' &&  expr_str[i]<='Z')) {
      
      get_name(expr_str, i);

      if (expr_str[i] == '.') {
        char *relation_name = new char[i - start + 1];
        relation_name[i - start] = '\0';
        memcpy(relation_name, expr_str+start, i - start);
        start = ++i;
        get_name(expr_str, i);
        table = table_map[relation_name];
        delete[] relation_name;
      }

      char *attr_name;
      attr_name = new char[i - start + 1];
      attr_name[i - start] = '\0';
      memcpy(attr_name, expr_str+start, i - start);
      const FieldMeta *field_meta = nullptr;
      field_meta = table->table_meta().field(attr_name);
      expr_stack.push_back(new FieldExpr(table, field_meta));
      new_expression = false;
      delete[] attr_name;
      continue;
    }

    switch (expr_str[i])
    {
    case '+':
      while(!op_stack.empty() && op_stack.back() != '(') {
        get_ComplexExpr(expr_stack, op_stack);
      }
      op_stack.push_back('+');
      break;
    case '-':
      if (new_expression) {
        expr_stack.push_back(new ValueExpr(Value{CHARS, (void *)"0"}));
        op_stack.push_back('n');
        new_expression = false;
      } else {
        while(!op_stack.empty() && op_stack.back() != '(') {
          get_ComplexExpr(expr_stack, op_stack);
        }
        op_stack.push_back('-');
      }
      break;
    case '*':
      if(!op_stack.empty() && (op_stack.back() == '*' || op_stack.back() == '/' || op_stack.back() == 'n')) {
        get_ComplexExpr(expr_stack, op_stack);
      }
      op_stack.push_back('*');
      break;
    case '/':
      if(!op_stack.empty() && (op_stack.back() == '*' || op_stack.back() == '/' || op_stack.back() == 'n')) {
        get_ComplexExpr(expr_stack, op_stack);
      }
      op_stack.push_back('/');
      break;
    case '(':
      op_stack.push_back('(');
      new_expression = true; //此处代表后面可跟 '-'
      break;
    case ')':
      while (op_stack.back() != '(') {
        get_ComplexExpr(expr_stack, op_stack);
      }
      op_stack.pop_back();
      break;
    default:
      LOG_ERROR("Wrong Math Operator!");
      return nullptr;
      break;
    }
    i++;
  }
  while (!op_stack.empty()) {
    get_ComplexExpr(expr_stack, op_stack);
  }

  return expr_stack.back();
}
