#include "subquery_expression.h"

RC SubQueryExpr::init_with_value_list(const FieldExpr *left_fieldmeta, const Value *value_list, size_t value_num)
{
  assert(left_fieldmeta->type() == ExprType::FIELD);
  RC rc = RC::SUCCESS;
  std::vector<const FieldMeta *> schema = {left_fieldmeta->field().meta()};
  AttrType expect_attr_type = left_fieldmeta->field().attr_type();
  for (size_t i = 0; i < value_num; i++) {
    TempTuple *new_tuple = new TempTuple();
    new_tuple->set_schema(schema);
    if (value_list[i].type != NULLS) {
      rc = new_tuple->cell_value_add(0, value_list[i]);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to initialize new tuple from value list");
        return rc;
      }
    }
    append_tuple(new_tuple);
  }
  return rc;
}

RC SubQueryExpr::init_with_subquery_stmt(Stmt *select_stmt)
{
  RC rc = RC::SUCCESS;
  assert(select_stmt->type() == StmtType::SELECT);
  select_stmt_ = (SelectStmt *)select_stmt;

  // collect tables in `where` statement if this query is a `sub-query`
  // 子查询中出现与父查询关联的表时，需要走完全不同的执行逻辑
  bool is_associated = false;
  for (FilterUnit *filter_unit : select_stmt_->filter_stmt()->filter_units()) {
    if (!filter_unit->have_subquery()) {
      continue;
    }
    if (filter_unit->left()->type() == ExprType::FIELD) {
      FieldExpr *left_field_expr = (FieldExpr *)filter_unit->left();
      if (common::is_blank(left_field_expr->table_name())) {
        LOG_WARN("Cannot find the table corresponding to the attribute");
        return RC::MISMATCH;
      }
      size_t i;
      for (i = 0; i < select_stmt_->tables().size(); i++) {
        Table *table = select_stmt_->tables()[i];
        if (strcmp(table->table_meta().name(), left_field_expr->table_name()) == 0) {
          break;
        }
      }
      if (i == select_stmt_->tables().size()) {
        is_associated = true;
      }
    }

    if (filter_unit->right()->type() == ExprType::FIELD) {
      FieldExpr *right_field_expr = (FieldExpr *)filter_unit->right();
      if (common::is_blank(right_field_expr->table_name())) {
        LOG_WARN("Cannot find the table corresponding to the attribute");
        return RC::MISMATCH;
      }
      size_t i;
      for (i = 0; i < select_stmt_->tables().size(); i++) {
        Table *table = select_stmt_->tables()[i];
        if (strcmp(table->table_meta().name(), right_field_expr->table_name()) == 0) {
          break;
        }
      }
      if (i == select_stmt_->tables().size()) {
        is_associated = true;
      }
    }
  }

  if (is_associated) {
    status_ = ASSOCIATED;
    return RC::SUCCESS;
  }

  rc = create_and_execute_simple_subquery(select_stmt_);
  if (rc == RC::SUCCESS) {
    status_ = READY;
  }

  return rc;
}

RC SubQueryExpr::create_and_execute_simple_subquery(SelectStmt *select_stmt)
{
  RC rc = RC::SUCCESS;
  Planner planner(select_stmt);
  Operator *root = nullptr;
  planner.create_executor(root);

  rc = root->open();
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open subquery");
    return rc;
  }

  while ((rc = root->next()) == RC::SUCCESS) {
    append_tuple(root->current_tuple());
  }

  root->close();

  if (rc != RC::RECORD_EOF) {
    LOG_ERROR("unknown error when executing subquery");
    return rc;
  }
  return RC::SUCCESS;
}

RC SubQueryExpr::init_and_execute_associated_query(const Tuple &outer_query_tuple)
{
  assert(status_ == ASSOCIATED);

  for (TempTuple *tuple : sub_query_results_) {
    delete tuple;
  }
  sub_query_results_.clear();

  // 构造新的filter_stmt并执行。将filter_stmt中存在关联的属性替换为tuple中的实际值。
  for (const FilterUnit *filter_unit : select_stmt_->filter_stmt()->filter_units()) {
    Expression *left_expr = filter_unit->left();
    Expression *right_expr = filter_unit->right();

    if (left_expr->type() == ExprType::FIELD) {
      FieldExpr *left_attr_expr = (FieldExpr *)left_expr;
      Field left_field = left_attr_expr->field();
      if (!is_field_specified_in_rel_list(select_stmt_, left_field)) {
        set_associated_value_for_attr_expr(left_attr_expr, outer_query_tuple);
      }
    }

    if (right_expr->type() == ExprType::FIELD) {
      FieldExpr *right_attr_expr = (FieldExpr *)right_expr;
      Field right_field = right_attr_expr->field();
      if (!is_field_specified_in_rel_list(select_stmt_, right_field)) {
        set_associated_value_for_attr_expr(right_attr_expr, outer_query_tuple);
      }
    }
  }

  // 重构后的select_stmt对外表现为无关联的子查询
  RC rc = create_and_execute_simple_subquery(select_stmt_);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create and execute associated subquery");
    return rc;
  }

  return rc;
}

RC SubQueryExpr::get_value(const Tuple &tuple, TupleCell &cell) const
{
  if (select_stmt_ && select_stmt_->do_aggregate() && sub_query_results_.size() > 0) {
    return sub_query_results_[0]->cell_at(0, cell);
  }
  return RC::SUCCESS;
}

bool is_field_specified_in_rel_list(SelectStmt *select_stmt, Field &field)
{
  for (Table *table : select_stmt->tables()) {
    if (field.table() == table) {
      return true;
    }
  }
  return false;
}

RC set_associated_value_for_attr_expr(FieldExpr *attr_expr, const Tuple &outer_query_tuple)
{
  Field field = attr_expr->field();
  if (!is_field_associated_with_outer_query(outer_query_tuple, field)) {
    LOG_WARN("unknown right field in sub-query");
    return RC::NOTFOUND;
  }
  TupleCell outer_tuple_cell;
  outer_query_tuple.find_cell(field, outer_tuple_cell);
  attr_expr->set_associated_value(outer_tuple_cell);
  return RC::SUCCESS;
}

bool is_field_associated_with_outer_query(const Tuple &outer_query_tuple, Field &field)
{
  for (size_t i = 0; i < outer_query_tuple.cell_num(); i++) {
    const TupleCellSpec *outer_cell_spec;
    outer_query_tuple.cell_spec_at(i, outer_cell_spec);
    if (outer_cell_spec->expression()->type() == ExprType::FIELD) {
      FieldExpr *outer_field_expr = (FieldExpr *)outer_cell_spec->expression();
      if (strcmp(field.table_name(), outer_field_expr->table_name()) == 0) {
        return true;
      }
    }
  }
  return false;
}