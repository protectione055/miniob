#include "common/log/log.h"
#include "sql/operator/hash_aggregate_operator.h"
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
    Tuple *cur_group_tuple = nullptr;
    if (tuples_.find(cur_group_key) == tuples_.end()) {
      //对新的group进行初始化
      TempTuple *new_result_tuple = new TempTuple();
      std::vector<FieldMeta *> group_fieldmetas;
      for (const Field &field : query_fields_) {
        group_fieldmetas.push_back(const_cast<FieldMeta *>(field.meta()));
      }
      new_result_tuple->set_schema(group_fieldmetas);
      tuples_[cur_group_key] = new_result_tuple;
      valid_count_[cur_group_key] = std::map<int, int>();
      is_new_group = true;
    }

    Key test_key(cur_group_key);
    assert(!(test_key < cur_group_key));

    // 找到当前group
    if (tuples_.find(cur_group_key) == tuples_.end()) {
      assert(false);
    }
    cur_group_tuple = tuples_[cur_group_key];

    //执行聚合
    for (int i = 0; i < query_fields_.size(); i++) {
      Field &field = query_fields_[i];
      FieldMeta *field_meta = field_meta_[i];
      const FieldMeta *subfield_meta = field.sub_fieldmeta();
      TupleCell cur_cell;
      cur_group_tuple->cell_at(i, cur_cell);
      char *cur_data = const_cast<char *>(cur_cell.data());
      TupleCell child_cell;
      char *child_data;

      if (field.table() != nullptr) {
        child_tuple->find_cell(Field(field.table(), subfield_meta), child_cell);
        child_data = const_cast<char *>(child_cell.data());
      }

      AggrType aggr_type = field.aggregation_type();
      if (aggr_type == NOT_AGGR) {
        // 查找该字段是否在group by子句中
        if (is_new_group) {
          for (Field &g_field : group_by_keys_) {
            rc = RC::GENERIC_ERROR;
            if (0 == strcmp(g_field.table_name(), field.table_name()) &&
                0 == strcmp(g_field.meta()->name(), field.sub_fieldmeta()->name())) {
              cur_data = const_cast<char *>(cur_cell.data());
              //假设字符串是定长
              assert(cur_cell.length() == child_cell.length());
              memcpy(cur_data, child_data, child_cell.length());
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
            if (is_new_group || aggr_type == MIN && cur_cell.compare(child_cell) > 0 ||
                aggr_type == MAX && cur_cell.compare(child_cell) < 0) {
              cur_data = const_cast<char *>(cur_cell.data());
              // 这里假设字符串是定长
              if (cur_cell.length() != child_cell.length()) {
                LOG_WARN("mismatch cell length: cur_cell=%d, child_cell=%d", cur_cell.length(), child_cell.length());
                return RC::GENERIC_ERROR;
              }
              memcpy(cur_data, child_data, child_cell.length());
            }
            break;
          case SUM:
          case AVG:
            // assert(cur_cell.attr_type() != CHARS && child_cell.attr_type() != CHARS);
            if (!child_tuple->is_null_at(i)) {
              rc = cur_cell.add(child_cell);
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
    // 遍历groups
    for (auto &i : valid_count_) {
      // get <Key, std::map<int, int>> pairs
      TempTuple *group_tuple = tuples_[i.first];
      std::map<int, int> &idx_cnt_map = i.second;
      for (auto &j : idx_cnt_map) {
        // 遍历每个group中的avg累加结果
        int index = j.first;
        int valid_num = j.second;
        TupleCell cur_cell;
        rc = group_tuple->cell_at(index, cur_cell);
        if (rc != RC::SUCCESS) {
          LOG_WARN("can't find cell at %d", index);
          return rc;
        }
        char *cur_data = const_cast<char *>(cur_cell.data());
        *(float *)cur_data /= (float)valid_num;
      }
    }
  }

  return RC::SUCCESS;
}

RC HashAggregateOperator::next()
{
  if (first_fetch_) {
    first_fetch_ = false;
    iter_ = tuples_.begin();
  }
  while (iter_ != tuples_.end() && !having(iter_->second, having_stmt_)) {
    iter_++;
  }
  if (iter_ == tuples_.end()) {
    return RC::RECORD_EOF;
  }
  return RC::SUCCESS;
}

RC HashAggregateOperator::close()
{
  children_[0]->close();
  delete having_stmt_;
  for (auto &kvpair : tuples_) {
    delete kvpair.second;
  }
  return RC::SUCCESS;
}

Tuple *HashAggregateOperator::current_tuple()
{
  assert(iter_ != tuples_.end());
  Tuple *current_tuple = iter_->second;
  iter_++;
  return current_tuple;
}

RC HashAggregateOperator::tuple_cell_spec_at(int index, const TupleCellSpec *&spec) const
{
  return iter_->second->cell_spec_at(index, spec);
}

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
    const int compare = left_cell.compare(right_cell);

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
    } else {
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

    if (!filter_result) {
      return false;
    }
  }
  return true;
}