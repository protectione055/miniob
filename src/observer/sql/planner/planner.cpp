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
  std::queue<Operator *> oper_queue;
  assert(tables.size() == 1);  // 在单表查询下测试
  if (tables.size() == 1) {
    // 单表查询尝试使用索引扫描
    
    scan_oper = try_to_create_index_scan_operator(select_stmt->filter_stmt());
    if (nullptr == scan_oper) {
      scan_oper = new TableScanOperator(select_stmt->tables()[0]);
    }
    oper_queue.push(scan_oper);
  } else {
    // 多表查询
    for (Table *table : tables) {
      scan_oper = new TableScanOperator(table);
      oper_queue.push(scan_oper);
    }
  }

  //创建join-tree
  //   while (oper_queue.size() > 1) {
  // JoinOperator *join_oper = new JoinOperator();
  // Operator *left = oper_queue.front();
  // Operator *right = oper_queue.front();
  // join_oper->add_child(left);
  // join_oper->add_child(right);
  // oper_queue.push(join_oper);
  //   }
  scan_oper = oper_queue.front();

  PredicateOperator *pred_oper = new PredicateOperator(select_stmt->filter_stmt());
  pred_oper->add_child(scan_oper);

  ProjectOperator *project_oper = new ProjectOperator();
  if (select_stmt->do_aggregate()) {
    HashAggregateOperator *aggregate_oper =
        new HashAggregateOperator(select_stmt->query_fields(), select_stmt->group_keys(), select_stmt->having_stmt());
    aggregate_oper->add_child(pred_oper);
    project_oper->add_child(aggregate_oper);
    
  } else {
    project_oper->add_child(pred_oper);
    
  }

  // 给project_operator设定投影关系
  for (const Field &field : select_stmt->query_fields()) {
    project_oper->add_projection(field.table(), field.meta());
  }

  root = project_oper;
  return rc;
}