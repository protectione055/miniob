#include "expression.h"
#include "sql/stmt/select_stmt.h"
#include "sql/planner/planner.h"
#include "sql/expr/tuple.h"

class SubQueryExpr : public Expression {
public:
  SubQueryExpr() = default;
  SubQueryExpr(Stmt *select_stmt, bool is_associated)
  {
    assert(select_stmt->type() == StmtType::SELECT);
    select_stmt_ = (SelectStmt *)select_stmt;
    associated_query_ = is_associated;
    // for (const Field &field : select_stmt_->query_fields()) {
    //   sub_query_fieldmetas_.push_back(const_cast<FieldMeta *>(field.meta()));
    // }
    // // 测试时假定子查询只产生一列
    // assert(sub_query_fieldmetas_.size() == 1);
  }
  ~SubQueryExpr() = default;

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

  void append_tuple(Tuple *tuple)
  {
    TempTuple *new_tuple = new TempTuple(tuple);
    sub_query_results_.push_back(new_tuple);
  }

  SelectStmt *select_stmt() const
  {
    return select_stmt_;
  }

  size_t result_num() const
  {
    return sub_query_results_.size();
  }

  bool is_associated_query() const
  {
    return associated_query_;
  }

private:
  RC execute_subquery(SelectStmt *select_stmt);

  SelectStmt *select_stmt_ = nullptr;  //子查询树
                                       //   std::vector<FieldMeta *> sub_query_fieldmetas_;  // 子查询select的字段
  //   std::multimap<Key, TempTuple *> sub_query_result_;  // 子查询的结果，子查询是聚合查询时只会有一条结果
  bool associated_query_ = false;
  std::vector<TempTuple *> sub_query_results_;
  //   std::map<Key, TempTuple *>::iterator iter_;    // 子查询结果迭代器
};