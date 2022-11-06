#include "planner.h"

extern IndexScanOperator *try_to_create_index_scan_operator(FilterStmt *filter_stmt);

RC Planner::create_executor(Operator *&root)
{
  RC rc = RC::SUCCESS;

  switch (stmt_->type()) {
    case StmtType::SELECT:
      rc = create_select_plan((SelectStmt *)stmt_, root);
      break;
    default:
      break;
  }
  root_ = root;
  return rc;
}

// TODO: fix it
RC Planner::destroy_executor(Operator *&root)
{
  // do nothing
  return RC::SUCCESS;
}

RC Planner::create_select_plan(SelectStmt *select_stmt, Operator *&root)
{
  RC rc = RC::SUCCESS;
  // 创建scan节点
  const std::vector<Table *> tables = select_stmt->tables();
  Operator *scan_oper;

  std::unordered_map<std::string, Operator *> table_operator_map;
  if (select_stmt->tables().empty()) {
    LOG_WARN("invalid argument. size of tables = 0");
    return RC::INVALID_ARGUMENT;
  }

  PredicateOperator *top_pred_oper = new PredicateOperator(select_stmt->filter_stmt());
  if (select_stmt->tables().size() > 1) {
    for (int i = 0; i < select_stmt->tables().size(); i++) {
      FilterStmt *push_down_filter_stmt = select_stmt->push_down_filter_stmts(i);
      Table *table = select_stmt->tables()[i];
      Operator *scan_oper = try_to_get_scan_operator(table, push_down_filter_stmt);
      PredicateOperator *push_down_pred_oper = new PredicateOperator(push_down_filter_stmt);
      push_down_pred_oper->add_child(scan_oper);
      table_operator_map[table->table_meta().name()] = push_down_pred_oper;
    }
    Operator *join_oper = JoinOperator::create_join_tree(table_operator_map, select_stmt->join_keys());
    top_pred_oper->add_child(join_oper);
  } else {
    Operator *scan_oper = try_to_get_scan_operator(select_stmt->tables()[0], select_stmt->filter_stmt());
    top_pred_oper->add_child(scan_oper);
  }

  OrderOperator *order_oper = new OrderOperator(select_stmt->order_fields());

  ProjectOperator *project_oper = new ProjectOperator();

  if (select_stmt->do_aggregate()) {
    HashAggregateOperator *aggregate_oper =
        new HashAggregateOperator(select_stmt->query_fields(), select_stmt->group_keys(), select_stmt->having_stmt());
    aggregate_oper->add_child(top_pred_oper);
    order_oper->add_child(aggregate_oper);
  } else {
    order_oper->add_child(top_pred_oper);
  }

  project_oper->add_child(order_oper);

  // 给project_operator设定投影关系
  bool multi_table = false;
  if (select_stmt->tables().size() > 1)
    multi_table = true;
  for (const Field &field : select_stmt->query_fields()) {
    if (field.meta()->visible()) {
      project_oper->add_projection(field.table(), field.meta(), multi_table);
    }
  }

  root = project_oper;
  return rc;
}

Operator *Planner::try_to_get_scan_operator(Table *table, FilterStmt *filter_stmt)
{
  Operator *scan_oper = try_to_create_index_scan_operator(filter_stmt);
  if (nullptr == scan_oper) {
    scan_oper = new TableScanOperator(table);
  }
  return scan_oper;
}