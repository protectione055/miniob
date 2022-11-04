#include "subquery_expression.h"

RC SubQueryExpr::execute_subquery(SelectStmt *select_stmt)
{
  // 创建子查询计划
  RC rc = RC::SUCCESS;
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
    sub_query->append_tuple(root->current_tuple());
  }

  root->close();

  if (rc != RC::RECORD_EOF) {
    LOG_ERROR("unknown error when executing subquery");
    return rc;
  }
  return RC::SUCCESS;
}

RC SubQueryExpr::get_value(const Tuple &tuple, TupleCell &cell) const
{
  if (associated_query_) {
    //修改condition中的值，构建新的查询语句
    FilterStmt *new_filter_stmt = nullptr;
    SelectStmt *new_select_stmt = new SelectStmt();
    //执行新查询
  }

  if (select_stmt_->do_aggregate() && sub_query_results_.size() > 0) {
    return sub_query_results_[0]->cell_at(0, cell);
  }
  return RC::SUCCESS;
}