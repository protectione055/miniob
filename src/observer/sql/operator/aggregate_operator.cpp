#include "common/log/log.h"
#include "sql/operator/aggregate_operator.h"
#include "storage/record/record.h"
#include "storage/common/table.h"

const char *aggr_type_name[] = {"NOT_AGGR", "MIN", "MAX", "SUM", "COUNT", "AVG"};

RC AggregateOperator::open()
{
  if (children_.size() != 1) {
    LOG_WARN("aggregate operator must has 1 child");
    return RC::INTERNAL;
  }

  Operator *child = children_[0];
  RC rc = child->open();
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open child operator: %s", strrc(rc));
    return rc;
  }

  return RC::SUCCESS;
}

RC AggregateOperator::next()
{
  RC rc = children_[0]->next();
  tuple_.reset_data();
  bool have_tuple = false;
  bool first_shot = true;
  std::vector<int> valid_row_count(query_fields_.size(), 0);
  for (Tuple *child_tuple = children_[0]->current_tuple(); rc == RC::SUCCESS; rc = children_[0]->next()) {
    have_tuple = true;
    child_tuple = children_[0]->current_tuple();

    for (int i = 0; i < query_fields_.size(); i++) {
      Field &field = query_fields_[i];
      FieldMeta *field_meta = field_meta_[i];
      const FieldMeta *subfield_meta = field.sub_fieldmeta();
      TupleCell cur_cell;
      tuple_.cell_at(i, cur_cell);
      char *cur_data = const_cast<char *>(cur_cell.data());
      TupleCell child_cell;
      char *child_data;
      if (field.table() != nullptr) {
        child_tuple->find_cell(Field(field.table(), subfield_meta), child_cell);
        child_data = const_cast<char *>(child_cell.data());
      }
      AggrType aggr_type = field.aggregation_type();

      if (aggr_type == NOT_AGGR) {
        // 留个坑，group by时再填
        LOG_WARN("not aggregate type query got in the wrong way");
        return RC::GENERIC_ERROR;
      } else {
        switch (aggr_type) {
          case COUNT:
            if (subfield_meta == nullptr) {
              // count(*)
              *(int *)cur_data += 1;
            } else {
              // count(a)
              if (!child_tuple->is_null_at(i)) {
                cur_data = const_cast<char *>(cur_cell.data());
                *(int *)cur_data += 1;
              }
            }
            break;
          case MIN:
          case MAX:
            if (first_shot || aggr_type == MIN && cur_cell.compare(child_cell) > 0 ||
                aggr_type == MAX && cur_cell.compare(child_cell) < 0) {
              cur_data = const_cast<char *>(cur_cell.data());
              //假设字符串是定长
              if (cur_cell.length() != child_cell.length()) {
                LOG_WARN("mismatch cell length: cur_cell=%d, child_cell=%d", cur_cell.length(), child_cell.length());
                return RC::GENERIC_ERROR;
              }
              memcpy(cur_data, child_data, child_cell.length());
            }
            break;
          case SUM:
          case AVG:
            assert(cur_cell.attr_type() != CHARS && child_cell.attr_type() != CHARS);
            if (!child_tuple->is_null_at(i)) {
              valid_row_count[i]++;
              rc = cur_cell.add(child_cell);
              if (rc != RC::SUCCESS) {
                LOG_WARN("failed to add when doing %s", field.field_name());
                return rc;
              }
            }
            break;
          default:
            break;
        }
      }
      if (first_shot) {
        first_shot = false;
      }
    }
  }
  // 取完下层所有tuple最后扫一遍计算avg，只有从下层取到结果才会执行
  for (int i = 0; i < query_fields_.size() && have_tuple; i++) {
    if (query_fields_[i].aggregation_type() == AVG && valid_row_count[i] > 0) {
      TupleCell cur_cell;
      tuple_.cell_at(i, cur_cell);
      char *cur_data = const_cast<char *>(cur_cell.data());
      *(float *)cur_data /= (float)valid_row_count[i];
    }
  }
  if (have_tuple) {
    return RC::SUCCESS;
  }
  return rc;
}

RC AggregateOperator::close()
{
  children_[0]->close();
  return RC::SUCCESS;
}

Tuple *AggregateOperator::current_tuple()
{
  return &tuple_;
}

RC AggregateOperator::tuple_cell_spec_at(int index, const TupleCellSpec *&spec) const
{
  return tuple_.cell_spec_at(index, spec);
}
