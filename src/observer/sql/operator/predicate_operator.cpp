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
// Created by WangYunlai on 2022/6/27.
//

#include "common/log/log.h"
#include "sql/operator/predicate_operator.h"
#include "sql/planner/planner.h"
#include "storage/record/record.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/common/field.h"
#include "sql/expr/subquery_expression.h"

RC PredicateOperator::open()
{
  RC rc = RC::SUCCESS;
  if (children_.size() != 1) {
    LOG_WARN("predicate operator must has one child");
    return RC::INTERNAL;
  }

  rc = children_[0]->open();
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open PredicateOperator");
    return rc;
  }

  // 扫描所有FilterUnit，有子查询直接执行
  for (FilterUnit *filter_unit : filter_stmt_->filter_units()) {
    Expression *left = filter_unit->left();
    Expression *right = filter_unit->right();
    if (left->type() == ExprType::SUB_QUERY || right->type() == ExprType::SUB_QUERY) {
      assert(left->type() != right->type());
      SubQueryExpr *sub_query = nullptr;
      if (left->type() == ExprType::SUB_QUERY) {
        sub_query = dynamic_cast<SubQueryExpr *>(left);
      } else {
        sub_query = dynamic_cast<SubQueryExpr *>(right);
      }

      // 创建子查询计划
      SelectStmt *select_stmt = sub_query->select_stmt();
      Planner planner(select_stmt);
      Operator *root = nullptr;
      planner.create_executor(root);

      rc = root->open();
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to open subquery");
        return rc;
      }
      //获取子查询所有结果，保存到FilterUnit下对应的SubQueryExpr(左子式或右子式)中
      while ((rc = root->next()) == RC::SUCCESS) {
        Tuple *current_tuple = root->current_tuple();
        SubQueryExpr *sub_query = nullptr;
        if (left->type() == ExprType::SUB_QUERY) {
          sub_query = dynamic_cast<SubQueryExpr *>(left);
          sub_query->append_tuple(current_tuple);
        } else {
          sub_query = dynamic_cast<SubQueryExpr *>(right);
          sub_query->append_tuple(current_tuple);
        }
        if (filter_unit->comp() != IN && filter_unit->comp() != NOT_IN && sub_query->result_num() > 1) {
          LOG_WARN("more than one row returned by a subquery used as an expression");
          return INVALID_ARGUMENT;
        }
      }
      //返回RECORD_EOF说明下层节点已经取完，子查询执行成功
      root->close();
      if (rc == RC::RECORD_EOF) {
        rc = RC::SUCCESS;
      } else {
        LOG_ERROR("unknown error when executing subquery");
        return rc;
      }
    }
  }
  return rc;
}

RC PredicateOperator::next()
{
  RC rc = RC::SUCCESS;
  Operator *oper = children_[0];
  
  while (RC::SUCCESS == (rc = oper->next())) {
    Tuple *tuple = oper->current_tuple();
    if (nullptr == tuple) {
      rc = RC::INTERNAL;
      LOG_WARN("failed to get tuple from operator");
      break;
    }

    if (do_predicate(*tuple)) {
      return rc;
    }
  }
  return rc;
}

RC PredicateOperator::close()
{
  children_[0]->close();
  return RC::SUCCESS;
}

Tuple * PredicateOperator::current_tuple()
{
  return children_[0]->current_tuple();
}

bool PredicateOperator::do_predicate(Tuple &tuple)
{
  if (filter_stmt_ == nullptr || filter_stmt_->filter_units().empty()) {
    return true;
  }

  for (const FilterUnit *filter_unit : filter_stmt_->filter_units()) {
    Expression *left_expr = filter_unit->left();
    Expression *right_expr = filter_unit->right();
    CompOp comp = filter_unit->comp();
    TupleCell left_cell;
    TupleCell right_cell;
    // 子查询结果集为空时，有可能取到类型为Undefined的cell
    left_expr->get_value(tuple, left_cell);
    right_expr->get_value(tuple, right_cell);
    // const int compare = left_cell.compare(right_cell);

    bool filter_result = false;
    if(comp == LIKE || comp == NOT_LIKE){
      const bool like = left_cell.like(right_cell);
      switch (comp) {
      case LIKE: {
        filter_result = (like == true);
      } break;
      case NOT_LIKE: {
        filter_result = (like == false);
      } break;
      default: {
        LOG_WARN("invalid compare type: %d", comp);
      } break;
      }
    } else if (comp == IN) {
      filter_result = dynamic_cast<SubQueryExpr *>(right_expr)->in(left_cell);
    } else if (comp == NOT_IN) {
      filter_result = !dynamic_cast<SubQueryExpr *>(right_expr)->in(left_cell);
    } else {
      if (left_cell.attr_type() == UNDEFINED || right_cell.attr_type() == UNDEFINED) {
        return false;
      }
      const int compare = left_cell.compare(right_cell);
      switch (comp) {
      case EQUAL_TO: {
        filter_result = (0 == compare); 
      } break;
      case LESS_EQUAL: {
        filter_result = (compare <= 0); 
      } break;
      case NOT_EQUAL: {
        filter_result = (compare != 0);
      } break;
      case LESS_THAN: {
        filter_result = (compare < 0);
      } break;
      case GREAT_EQUAL: {
        filter_result = (compare >= 0);
      } break;
      case GREAT_THAN: {
        filter_result = (compare > 0);
      } break;
      default: {
        LOG_WARN("invalid compare type: %d", comp);
      } break;
      }
    }

    if (!filter_result) {
      return false;
    }
  }
  return true;
}

// int PredicateOperator::tuple_cell_num() const
// {
//   return children_[0]->tuple_cell_num();
// }
// RC PredicateOperator::tuple_cell_spec_at(int index, TupleCellSpec &spec) const
// {
//   return children_[0]->tuple_cell_spec_at(index, spec);
// }
