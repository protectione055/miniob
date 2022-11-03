#include "common/log/log.h"
#include "sql/operator/hash_aggregate_operator.h"
#include "sql/operator/predicate_operator.h"
#include "storage/record/record.h"
#include "storage/common/table.h"
#include "sql/stmt/filter_stmt.h"

RC HashAggregateOperator::open()
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

  // 从子节点获取元组进行聚合
  bool have_tuple = false;
  bool do_avg = false;
  for (rc = children_[0]->next(); rc == RC::SUCCESS; rc = children_[0]->next()) {
    Tuple *child_tuple = children_[0]->current_tuple();
    have_tuple = true;
    bool is_new_group = false;
    Key cur_group_key;
    rc = cur_group_key.init(group_by_keys_, *child_tuple);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("failed to initiate key");
      return rc;
    }
    TempTuple *cur_group_tuple = nullptr;
    if (key_tuples_.find(cur_group_key) == key_tuples_.end()) {
      //对新的group进行初始化
      TempTuple *new_result_tuple = new TempTuple();
      std::vector<const FieldMeta *> group_fieldmetas;
      for (const Field &field : query_fields_) {
        group_fieldmetas.push_back(field.meta());
      }
      new_result_tuple->set_schema(group_fieldmetas);
      key_tuples_[cur_group_key] = new_result_tuple;
      valid_count_[cur_group_key] = std::map<int, int>();
      is_new_group = true;
    }

    Key test_key(cur_group_key);
    assert(!(test_key < cur_group_key));

    // 找到当前group
    if (key_tuples_.find(cur_group_key) == key_tuples_.end()) {
      assert(false);
    }
    cur_group_tuple = key_tuples_[cur_group_key];

    //执行聚合
    for (int i = 0; i < query_fields_.size(); i++) {
      Field &field = query_fields_[i];
      FieldMeta *field_meta = field_meta_[i]; // for query_fields
      const FieldMeta *subfield_meta = field.sub_fieldmeta();
      TupleCell cur_cell;
      cur_group_tuple->cell_at(i, cur_cell);
      TupleCell child_cell;
      const char *child_data;

      if (field.table() != nullptr) {
        child_tuple->find_cell(Field(field.table(), subfield_meta), child_cell);
        child_data = child_cell.data();
      }

      AggrType aggr_type = field.aggregation_type();
      if (aggr_type == NOT_AGGR) {
        // 查找该字段是否在group by子句中
        if (is_new_group) {
          for (Field &g_field : group_by_keys_) {
            rc = RC::GENERIC_ERROR;
            if (0 == strcmp(g_field.table_name(), field.table_name()) &&
                0 == strcmp(g_field.meta()->name(), field.sub_fieldmeta()->name())) {
              //假设字符串是定长
              assert(field_meta->len() == child_cell.length());
              cur_group_tuple->cell_set(i, child_data);
              rc = RC::SUCCESS;
              break;
            }
          }
          if (rc != RC::SUCCESS) {
            LOG_WARN("illegal field in aggregation");
            return rc;
          }
        }
      } else {
        switch (aggr_type) {
          case COUNT:
            if(is_new_group) {
              cur_group_tuple->cell_int_add(i, 0); // init null to 0
            }
            if (subfield_meta == nullptr) {
              // count(*)
              cur_group_tuple->cell_int_add(i, 1);
            } else {
              // count(a)
              if (child_cell.attr_type() != NULLS) {
                cur_group_tuple->cell_int_add(i, 1);
              }
            }
            break;
          case MIN:
          case MAX:
            if(child_cell.attr_type() == NULLS) {
              // do nothing
            } else if (is_new_group || aggr_type == MIN && cur_cell.compare(child_cell) > 0 ||
                aggr_type == MAX && cur_cell.compare(child_cell) < 0) {
              // 这里假设字符串是定长
              if (cur_cell.attr_type() != NULLS && cur_cell.length() != child_cell.length()) {
                LOG_WARN("mismatch cell length: cur_cell=%d, child_cell=%d", cur_cell.length(), child_cell.length());
                return RC::GENERIC_ERROR;
              }
              cur_group_tuple->cell_set(i, child_data);
            }
            break;
          case SUM:
          case AVG:
            // assert(cur_cell.attr_type() != CHARS && child_cell.attr_type() != CHARS);
            if (child_cell.attr_type() != NULLS) {
              rc = cur_group_tuple->cell_value_add(i, Value{child_cell.attr_type(), (void*)child_cell.data()});
              if (rc != RC::SUCCESS) {
                LOG_WARN("failed to add when doing %s", field.field_name());
                return rc;
              }
              //记录group在该字段上的有效元组数
              if (aggr_type == AVG) {
                do_avg = true;
                if (valid_count_[cur_group_key].find(i) == valid_count_[cur_group_key].end()) {
                  valid_count_[cur_group_key][i] = 1;
                } else {
                  valid_count_[cur_group_key][i]++;
                }
              }
            }
            break;
          default:
            break;
        }
      }
    }
  }

  // 计算avg结果
  if (do_avg) {
    for (auto &row : valid_count_) {
      TempTuple *group_tuple = key_tuples_[row.first];
      std::map<int, int> &idx_cnt_map = row.second;
      for (auto &col : idx_cnt_map) {
        int index = col.first;
        int valid_num = col.second;
        TupleCell cur_cell;
        rc = group_tuple->cell_at(index, cur_cell);
        if (rc != RC::SUCCESS) {
          LOG_WARN("can't find cell at %d", index);
          return rc;
        }
        
        float new_data = (*(float *)cur_cell.data())/(float)valid_num;
        group_tuple->cell_set(index, (char*)&new_data);
      }
    }
  }

  return RC::SUCCESS;
}

RC HashAggregateOperator::next()
{
  if (first_fetch_) {
    first_fetch_ = false;
    iter_ = key_tuples_.begin();
  } else {
    iter_++;
  }
  while (iter_ != key_tuples_.end() && !having(iter_->second, having_stmt_)) {
    iter_++;
  }
  if (iter_ == key_tuples_.end()) {
    return RC::RECORD_EOF;
  }
  return RC::SUCCESS;
}

RC HashAggregateOperator::close()
{
  children_[0]->close();
  delete having_stmt_;
  for (auto &kvpair : key_tuples_) {
    delete kvpair.second;
  }
  return RC::SUCCESS;
}

Tuple *HashAggregateOperator::current_tuple()
{
  assert(iter_ != key_tuples_.end());
  Tuple *current_tuple = iter_->second;
  return current_tuple;
}

RC HashAggregateOperator::tuple_cell_spec_at(int index, const TupleCellSpec *&spec) const
{
  return iter_->second->cell_spec_at(index, spec);
}

// 处理having条件
bool HashAggregateOperator::having(const Tuple *tuple, const FilterStmt *having_stmt) const
{
  if (having_stmt == nullptr || having_stmt->filter_units().empty()) {
    return true;
  }

  for (const FilterUnit *filter_unit : having_stmt->filter_units()) {
    Expression *left_expr = filter_unit->left();
    Expression *right_expr = filter_unit->right();
    CompOp comp = filter_unit->comp();
    TupleCell left_cell;
    TupleCell right_cell;
    left_expr->get_value(*tuple, left_cell);
    right_expr->get_value(*tuple, right_cell);
   
    if(PredicateOperator::compare_tuple_cell(comp, left_cell, right_cell) == false) {
      return false;
    }
  }
  return true;
}