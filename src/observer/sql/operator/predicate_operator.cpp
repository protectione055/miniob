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

    if (filter_stmt_ == nullptr || filter_stmt_->filter_units().empty()) {
      return rc;
    }

    for (const FilterUnit *filter_unit : filter_stmt_->filter_units()) {
      if (filter_unit->have_subquery()) {
        rc = init_subquery(filter_unit, *tuple);
        if (rc != RC::SUCCESS) {
          LOG_ERROR("failed to initialize subquery");
          return rc;
        }
      }
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

Tuple *PredicateOperator::current_tuple()
{
  return children_[0]->current_tuple();
}

bool PredicateOperator::do_predicate(Tuple &tuple)
{
  if (filter_stmt_ == nullptr || filter_stmt_->filter_units().empty()) {
    return true;
  }
  for (const FilterUnit *filter_unit : filter_stmt_->filter_units()) {
    if (!filter_unit->have_subquery()) {
      Expression *left_expr = filter_unit->left();
      Expression *right_expr = filter_unit->right();
      CompOp comp = filter_unit->comp();
      TupleCell left_cell;
      TupleCell right_cell;
      left_expr->get_value(tuple, left_cell);
      right_expr->get_value(tuple, right_cell);
      if (PredicateOperator::compare_tuple_cell(comp, left_cell, right_cell) == false) {
        return false;
      }
    } else if (evaluate_subquery(filter_unit, tuple) == false) {
      return false;
    }
  }
  return true;
}

bool PredicateOperator::compare_tuple_cell(CompOp comp, TupleCell left_cell, TupleCell right_cell) {
    bool filter_result = false;
    if (comp == LIKE || comp == NOT_LIKE) {
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
    } else if(comp == IS_NULL || comp == IS_NOT_NULL) {
      if(right_cell.attr_type() != NULLS) {
        LOG_ERROR("IS or IS NOT isn't followed by NULL!");
      }
      return left_cell.attr_type() == NULLS ? comp == IS_NULL : comp == IS_NOT_NULL;
    } else {
      if(left_cell.attr_type() == NULLS || right_cell.attr_type() == NULLS) {
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

    return filter_result;
}

bool PredicateOperator::evaluate_subquery(const FilterUnit *filter_unit, Tuple &tuple)
{
  Expression *left_expr = filter_unit->left();
  Expression *right_expr = filter_unit->right();
  CompOp comp = filter_unit->comp();
  TupleCell left_cell;
  TupleCell right_cell;
  if (comp != EXISTS && comp != NOT_EXISTS) {
    left_expr->get_value(tuple, left_cell);
  }
  right_expr->get_value(tuple, right_cell);
  switch (comp) {
    case IN:
      return ((SubQueryExpr *)right_expr)->in(left_cell);
      break;
    case NOT_IN:
      return ((SubQueryExpr *)right_expr)->not_in(left_cell);
      break;
    case EXISTS:
      return ((SubQueryExpr *)right_expr)->exists();
    case NOT_EXISTS:
      return ((SubQueryExpr *)right_expr)->not_exists();
    default:
      if (left_cell.attr_type() == UNDEFINED || right_cell.attr_type() == UNDEFINED) {
        return false;
      }
      return compare_tuple_cell(comp, left_cell, right_cell);
      break;
  }
  return false;
}

RC PredicateOperator::init_subquery(const FilterUnit *filter_unit, Tuple &tuple)
{
  Expression *left_expr = filter_unit->left();
  Expression *right_expr = filter_unit->right();
  SubQueryExpr *left_subquery_expr = nullptr;
  SubQueryExpr *right_subquery_expr = nullptr;
  RC rc = RC::SUCCESS;

  if (left_expr && left_expr->type() == ExprType::SUB_QUERY) {
    left_subquery_expr = (SubQueryExpr *)left_expr;
    if (left_subquery_expr->status() == ASSOCIATED) {
      left_subquery_expr->init_and_execute_associated_query(tuple);
    }
  }
  if (right_expr->type() == ExprType::SUB_QUERY) {
    right_subquery_expr = (SubQueryExpr *)right_expr;
    if (right_subquery_expr->status() == ASSOCIATED) {
      right_subquery_expr->init_and_execute_associated_query(tuple);
    }
  }

  if (filter_unit->comp() != IN && filter_unit->comp() != NOT_IN && filter_unit->comp() != EXISTS &&
      filter_unit->comp() != NOT_EXISTS) {
    if ((left_subquery_expr && left_subquery_expr->result_num() > 1) ||
        (right_subquery_expr && right_subquery_expr->result_num() > 1)) {
      LOG_ERROR("more than one row returned by a subquery used as an expression");
      return RC::GENERIC_ERROR;
    }
  }
  return rc;
}

// int PredicateOperator::tuple_cell_num() const
// {
//   return children_[0]->tuple_cell_num();
// }
// RC PredicateOperator::tuple_cell_spec_at(int index, TupleCellSpec &spec) const
// {
//   return children_[0]->tuple_cell_spec_at(index, spec);
// }
