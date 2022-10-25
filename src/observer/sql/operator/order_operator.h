//
// Created by TanJianrong on 2022/10/24.
//

#pragma once

#include "sql/operator/operator.h"
#include "rc.h"

class OrderOperator : public Operator
{
public:
  OrderOperator(std::vector<std::tuple<FieldExpr, int>> order_fields){
    order_fields_.swap(order_fields);
    if(!order_fields_.empty()) skip = false;
  }

  virtual ~OrderOperator() = default;

  RC open() override;
  RC next() override;
  RC close() override;

  Tuple * current_tuple() override;
private:
  bool test(Tuple *tuple);
  bool order_compare(const Tuple *a, const Tuple *b);
private:
  std::vector<std::tuple<FieldExpr, int>> order_fields_;
  std::vector<Tuple *> tuple_list;
  bool load_data = false; //是否已读取上一节点所有tuple，用于判断节点当前是初始状态还是结束状态
  bool skip = true; //是否需要进行 Order By
};
