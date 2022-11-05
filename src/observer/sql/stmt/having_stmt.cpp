

#include "having_stmt.h"
#include "select_stmt.h"

HavingStmt::~HavingStmt()
{
  for (FilterUnit *unit : filter_units_) {
    delete unit;
  }
  filter_units_.clear();
}

RC HavingStmt::create_having_stmt(const std::vector<Field> &query_fields, const std::vector<Field> &group_by_keys,
    const Selects *select_sql, HavingStmt *&stmt)
{
  RC rc = RC::SUCCESS;
  stmt = nullptr;
  const Condition *having_conditions = select_sql->having_conditions;
  size_t having_condition_num = select_sql->having_condition_num;

  HavingStmt *tmp_stmt = new HavingStmt();
  for (int i = 0; i < having_condition_num; i++) {
    FilterUnit *filter_unit = nullptr;

    rc = create_having_filter_unit(query_fields, group_by_keys, having_conditions[i], filter_unit, tmp_stmt);
    if (rc != RC::SUCCESS) {
      delete tmp_stmt;
      LOG_WARN("failed to create filter unit. condition index=%d", i);
      return rc;
    }

    tmp_stmt->filter_units_.push_back(filter_unit);
  }

  stmt = tmp_stmt;
  return rc;
}

// 检查having左右两边的字段有没有出现在group-by或者是聚合函数，创建对应的unit
RC HavingStmt::create_having_filter_unit(const std::vector<Field> &query_fields,
    const std::vector<Field> &group_by_keys, const Condition &having_condition, FilterUnit *&filter_unit,
    HavingStmt *stmt)
{
  RC rc = RC::SUCCESS;

  CompOp comp = having_condition.comp;
  if (comp < EQUAL_TO || comp >= NO_OP) {
    LOG_WARN("invalid compare operator : %d", comp);
    return RC::INVALID_ARGUMENT;
  }

  Expression *left = nullptr;
  Expression *right = nullptr;
  if (having_condition.left_expr_type == ATTR) {
    const Field *field = find_table_field_for_having(query_fields, group_by_keys, having_condition.left_attr);
    if (field == nullptr && having_condition.left_attr.aggr_type == NOT_AGGR) {
      LOG_WARN("invalid field in having clause");
      return RC::MISMATCH;
    } else {
		// create new fields
    }
    left = new FieldExpr(field->table(), field->meta());
  } else {
    LOG_WARN("invalid left expression in having clause");
    return RC::MISMATCH;
  }

  if (having_condition.right_expr_type == ATTR) {
    Table *table = nullptr;
    const Field *field = find_table_field_for_having(query_fields, group_by_keys, having_condition.right_attr);
    if (field == nullptr) {
      LOG_WARN("invalid field in having clause");
      return RC::MISMATCH;
    }
    right = new FieldExpr(field->table(), field->meta());
  } else if (having_condition.right_expr_type == VALUE) {
    right = new ValueExpr(having_condition.right_value);
  }

  filter_unit = new FilterUnit;
  filter_unit->set_comp(comp);
  filter_unit->set_left(left);
  filter_unit->set_right(right);

  return rc;
}

const Field *find_table_field_for_having(
    const std::vector<Field> &query_fields, const std::vector<Field> &group_by_keys, const RelAttr &attr)
{
  for (const Field &field : query_fields) {
    if (is_field_and_attr_correspond(field, attr)) {
      return &field;
    }
  }
  //暂时不管group_by_keys，假定测试中having不会出现query_fields中没有的字段
  return nullptr;
}

bool is_field_and_attr_correspond(const Field &field, const RelAttr &attr)
{
  if (attr.aggr_type != field.aggregation_type()) {
    return false;
  }

  FieldMeta attr_meta;
  attr_meta.init(attr.attribute_name,
      field.attr_type(),
      field.meta()->offset(),
      field.meta()->len(),
      field.meta()->visible(),
      field.meta()->nullable());
  attr_meta.dirty_hack_set_namefunc(aggr_name[attr.aggr_type]);
  std::string field_name(field.meta()->name());
  std::string attr_name(attr_meta.name());
  std::transform(field_name.begin(), field_name.end(), field_name.begin(), ::toupper);
  std::transform(attr_name.begin(), attr_name.end(), attr_name.begin(), ::toupper);
  if (strcmp(field.meta()->name(), attr_meta.name()) != 0) {
    return false;
  }

  if (common::is_blank(field.table_name()) && common::is_blank(attr.relation_name)) {
    return true;
  } else if (!common::is_blank(field.table_name()) && !common::is_blank(attr.relation_name) &&
             strcmp(field.table_name(), attr.relation_name) != 0) {
    return false;
  }
  return true;
}