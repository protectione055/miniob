//
// Created by TanJianrong on 2022/10/28.
//

#include "common/log/log.h"
#include "sql/operator/join_operator.h"
#include "storage/record/record.h"
#include "storage/common/table.h"
#include "sql/expr/tuple.h"
#include <algorithm>

RC JoinOperator::open()
{
  if (left_ == nullptr || right_ == nullptr) {
    LOG_WARN("child of join operator must not be nullptr");
    return RC::INTERNAL;
  }
  RC rc = left_->open();
  if (rc != RC::SUCCESS)
    return rc;
  return right_->open();
}

RC JoinOperator::next()
{
  while (true) {
    switch (join_state_) {
      case BUILD_TABLE: {
        while (RC::SUCCESS == left_->next()) {
          Tuple *tuple = left_->current_tuple();
          if (nullptr == tuple) {
            LOG_WARN("failed to get tuple from operator");
            return RC::INTERNAL;
          }
          TempTuple *new_tuple = new TempTuple(*tuple);
          Expression *left_expr = join_cond_->left();
          TupleCell left_cell;
          left_expr->get_value(*new_tuple, left_cell);
          const char *key = left_cell.data();
          hash_table_[key].push_back(new_tuple);
        }
        join_state_ = GET_NEW_TUPLE;
        break;
      }

      case SCAN_BUCKET: {
        if (cur_bucket_.empty()) {
          pos_ = 0;
          Expression *right_expr = join_cond_->right();
          TupleCell right_cell;
          right_expr->get_value(*right_tuple_, right_cell);
          const char *key = right_cell.data();
          cur_bucket_.swap(hash_table_[key]);
        }
        //生成hash_tuple
        if (pos_ != cur_bucket_.size()) {
          TempTuple *hash_tuple = new TempTuple(cur_bucket_[pos_++], right_tuple_);
          result_table_.push_back(hash_tuple);
        }
        //扫描完后删除
        delete right_tuple_;
        right_tuple_ = nullptr;
        join_state_ = GET_NEW_TUPLE;
        break;
      }
      case GET_NEW_TUPLE: {
        RC rc = right_->next();
        if (rc != RC::SUCCESS)
          return rc;

        Tuple *tuple = right_->current_tuple();
        if (nullptr == tuple) {
          LOG_WARN("failed to get tuple from operator");
          return RC::INTERNAL;
        }
        right_tuple_ = new TempTuple(*tuple);

        cur_bucket_.clear();
        join_state_ = SCAN_BUCKET;
        break;
      }
      default:
        LOG_ERROR("Unmatch join state!");
        return RC::INTERNAL;
        break;
    }
  }
  return RC::SUCCESS;
}

RC JoinOperator::close()
{
  RC rc = left_->close();
  if (rc != RC::SUCCESS)
    return rc;
  return right_->close();
}

Tuple *JoinOperator::current_tuple()
{
  return result_table_.back();
}

Operator *JoinOperator::create_join_tree(
    std::unordered_map<std::string, Operator *> table_operator_map, FilterStmt *join_conds)
{
  Operator *root = nullptr;
  if (table_operator_map.size() == 1) {
    for (auto iter : table_operator_map) {
      root = iter.second;
    }
  }
  for (const FilterUnit *join_cond : join_conds->filter_units()) {
    FieldExpr *left_expr = (FieldExpr *)join_cond->left();
    FieldExpr *right_expr = (FieldExpr *)join_cond->right();
    root = new JoinOperator(
        table_operator_map[left_expr->table_name()], table_operator_map[right_expr->table_name()], join_cond);
    table_operator_map[left_expr->table_name()] = root;
    table_operator_map[right_expr->table_name()] = root;
  }
  return root;
}