#pragma once

#include "expression.h"
#include "sql/stmt/select_stmt.h"
#include "sql/planner/planner.h"
#include "sql/expr/tuple.h"

typedef enum { UNINITIALIZED, READY, ASSOCIATED } SubqueryStatus;

class SubQueryExpr : public Expression {
public:
  SubQueryExpr() = default;
  ~SubQueryExpr() = default;

  RC init_with_subquery_stmt(Stmt *select_stmt);

  RC init_with_value_list(const FieldExpr *left_fieldmeta, const Value *value_list, size_t value_num);

  RC init_and_execute_associated_query(const Tuple &outer_query_tuple);

  RC get_value(const Tuple &tuple, TupleCell &cell) const override;

  ExprType type() const override
  {
    return ExprType::SUB_QUERY;
  }

  // 用于in运算，判断子查询结果中是否存在给定的key
  bool in(TupleCell &cell) const
  {
    for (TempTuple *tuple : sub_query_results_) {
      assert(tuple->cell_num() == 1);
      TupleCell cmp_cell;
      tuple->cell_at(0, cmp_cell);
      if (cmp_cell.compare(cell) == 0) {
        return true;
      }
    }
    return false;
  }

  size_t result_num() const
  {
    return sub_query_results_.size();
  }

  SubqueryStatus status() const
  {
    return status_;
  }

private:
  RC create_and_execute_simple_subquery(SelectStmt *select_stmt);

  void append_tuple(Tuple *tuple)
  {
    TempTuple *new_tuple = new TempTuple(*tuple);
    sub_query_results_.push_back(new_tuple);
  }

  SelectStmt *select_stmt_ = nullptr;  //子查询树
  SubqueryStatus status_ = UNINITIALIZED;
  std::vector<TempTuple *> sub_query_results_;
  //   std::map<Key, TempTuple *>::iterator iter_;    // 子查询结果迭代器
  //   std::vector<FieldMeta *> sub_query_fieldmetas_;  // 子查询select的字段
  //   std::multimap<Key, TempTuple *> sub_query_result_;  // 子查询的结果，子查询是聚合查询时只会有一条结果
};

bool is_field_specified_in_rel_list(SelectStmt *select_stmt, Field &field);
RC set_associated_value_for_attr_expr(FieldExpr *attr_expr, const Tuple &outer_query_tuple);
bool is_field_associated_with_outer_query(const Tuple &outer_query_tuple, Field &field);