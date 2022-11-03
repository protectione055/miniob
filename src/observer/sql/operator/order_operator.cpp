//
// Created by WangYunlai on 2022/07/01.
//

#include "common/log/log.h"
#include "sql/operator/order_operator.h"
#include "storage/record/record.h"
#include "storage/common/table.h"
#include <algorithm>

std::vector<std::tuple<FieldExpr, int>> OrderOperator::order_fields_;

RC OrderOperator::open()
{
  if (children_.size() != 1) {
    LOG_WARN("order operator must has one child");
    return RC::INTERNAL;
  }
  
  return children_[0]->open();
}

RC OrderOperator::next()
{
  RC rc = RC::SUCCESS;
  Operator *oper = children_[0];
  if(!skip){
    if(!load_data){
      while (RC::SUCCESS == oper->next()) {
        Tuple *tuple = oper->current_tuple();
        if (nullptr == tuple) {
          rc = RC::INTERNAL;
          LOG_WARN("failed to get tuple from operator");
          break;
        }
        //由于子节点取到的tuple可能在同一物理位置，next后被覆盖，因此对其进行深拷贝
        TempTuple *new_tuple = new TempTuple(*tuple);
        tuple_list.push_back(new_tuple);
      }
      load_data = true; 
      std::stable_sort(tuple_list.begin(), tuple_list.end(), OrderOperator::order_compare);  
      return rc;
    }
    tuple_list.pop_back();
    if(tuple_list.empty()) return RC::RECORD_EOF;
  }else{
    while (RC::SUCCESS == (rc = oper->next())) {
      Tuple *tuple = oper->current_tuple();
      if (nullptr == tuple) {
        rc = RC::INTERNAL;
        LOG_WARN("failed to get tuple from operator");
        break;
      }
      return rc;
    }
  }
  return rc;
}

RC OrderOperator::close()
{
  children_[0]->close();
  return RC::SUCCESS;
}

Tuple * OrderOperator::current_tuple()
{
  if(!skip)
    return tuple_list.back();
  else
    return children_[0]->current_tuple();
}

// bool OrderOperator::test(Tuple *tuple){
//     RowTuple *row_tuple = static_cast<RowTuple *>(tuple);
//     for(int i=0; i<order_fields_.size(); ++i){
//         FieldExpr expr = std::get<0>(order_fields_[i]);
//         int is_asc = std::get<1>(order_fields_[i]);
//         TupleCell cell;
//         expr.get_value(*row_tuple, cell);
//         LOG_INFO("test!");
//     }
//     return 1;
// }

bool OrderOperator::order_compare(const Tuple *A, const Tuple *B)
{
  for (int i = 0; i < order_fields_.size(); ++i) {
    FieldExpr expr = std::get<0>(order_fields_[i]);
    int is_asc = std::get<1>(order_fields_[i]);
    TupleCell A_cell;
    TupleCell B_cell;
    expr.get_value(*A, A_cell);
    expr.get_value(*B, B_cell);
    if (A_cell.attr_type() == NULLS && B_cell.attr_type() == NULLS) {
      continue;
    }
    int compare;
    // nulls are considered smaller than anything when sorting
    if(A_cell.attr_type() == NULLS) {
      compare = -1;
    } else if(B_cell.attr_type() == NULLS) {
      compare = 1;
    } else {
      compare = A_cell.compare(B_cell);
    }
    if ((is_asc && compare > 0) || (!is_asc && compare < 0)) {
      return true;
    } else if ((is_asc && compare < 0) || (!is_asc && compare > 0)) {
      return false;
    }
  }
  return false;
}