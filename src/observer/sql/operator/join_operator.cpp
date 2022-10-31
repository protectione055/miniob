//
// Created by TanJianrong on 2022/10/28.
//

#include "common/log/log.h"
#include "sql/operator/join_operator.h"
#include "storage/record/record.h"
#include "storage/common/table.h"
#include "sql/expr/tuple.h"
#include <algorithm>

std::unordered_map<std::string, std::string> JoinOperator::fathers;

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
  if (do_nestloop_) {
  //   while (true) {
  //     switch (join_state_) {
  //       case BUILD_TABLE: {
  //         while (RC::SUCCESS == left_->next()) {
  //           Tuple *tuple = left_->current_tuple();
  //           if (nullptr == tuple) {
  //             LOG_WARN("failed to get tuple from operator");
  //             return RC::INTERNAL;
  //           }
  //           TempTuple *new_tuple = new TempTuple(*tuple);
  //           Expression *left_expr = join_cond_->left();
  //           TupleCell left_cell;
  //           left_expr->get_value(*new_tuple, left_cell);
  //           const char *key = left_cell.data();
  //           hash_table_[key].push_back(new_tuple);
  //         }
  //         join_state_ = GET_NEW_TUPLE;
  //         break;
  //       }
  //       case SCAN_BUCKET: {
  //         if (cur_bucket_.empty()) {
  //           pos_ = 0;
  //           Expression *right_expr = join_cond_->right();
  //           TupleCell right_cell;
  //           right_expr->get_value(*right_tuple_, right_cell);
  //           const char *key = right_cell.data();
  //           cur_bucket_.assign(hash_table_[key].begin(), hash_table_[key].end());
  //         }
  //         //生成hash_tuple
  //         if (pos_ != cur_bucket_.size()) {
  //           Tuple *hash_tuple = new TempTuple(*cur_bucket_[pos_++], *right_tuple_);
  //           result_table_.push_back(hash_tuple);
  //           return RC::SUCCESS;
  //         }
  //         //扫描完后删除
  //         delete right_tuple_;
  //         right_tuple_ = nullptr;
  //         join_state_ = GET_NEW_TUPLE;
  //         break;
  //       }
  //       case GET_NEW_TUPLE: {
  //         RC rc = right_->next();
  //         if (rc != RC::SUCCESS)
  //           return rc;
  //         Tuple *tuple = right_->current_tuple();
  //         if (nullptr == tuple) {
  //           LOG_WARN("failed to get tuple from operator");
  //           return RC::INTERNAL;
  //         }
  //         right_tuple_ = new TempTuple(*tuple);
  //         cur_bucket_.clear();
  //         join_state_ = SCAN_BUCKET;
  //         break;
  //       }
  //       default:
  //         LOG_ERROR("Unmatched join state!");
  //         return RC::INTERNAL;
  //         break;
  //     }
  //   }
  // } else {
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
            left_tuples_.push_back(new_tuple);
          }
          join_state_ = GET_NEW_TUPLE;
          break;
        }
        case SCAN_BUCKET: {
          //生成hash_tuple
          while (pos_ != left_tuples_.size()) {
            Tuple *left_tuple_ = left_tuples_[pos_++];
            if(do_predicate(left_tuple_)){
              Tuple *hash_tuple = new TempTuple(*left_tuple_, *right_tuple_);
              result_table_.push_back(hash_tuple);
              return RC::SUCCESS;
            }
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
          pos_ = 0;
          join_state_ = SCAN_BUCKET;
          break;
        }
        default:
          LOG_ERROR("Unmatched join state!");
          return RC::INTERNAL;
          break;
      }
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

bool JoinOperator::do_predicate(Tuple *tuple)
{
  if (join_conds_.empty()) {
    return true;
  }

  for (const FilterUnit *filter_unit : join_conds_) {
    Expression *left_expr = filter_unit->left();
    Expression *right_expr = filter_unit->right();
    CompOp comp = filter_unit->comp();
    TupleCell left_cell;
    TupleCell right_cell;
    left_expr->get_value(*tuple, left_cell);
    right_expr->get_value(*right_tuple_, right_cell);

    bool filter_result = false;
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

    if (!filter_result) {
      return false;
    }
  }
  return true;
}


std::string find_father(std::string son)
{
  if(son.compare(JoinOperator::fathers[son]) != 0){
    JoinOperator::fathers[son] = find_father(JoinOperator::fathers[son]);
  }
  return JoinOperator::fathers[son];
}

void merge(std::string a, std::string b)
{
  JoinOperator::fathers[find_father(b)] = a;
  JoinOperator::fathers[b] = a;
}

Operator *JoinOperator::create_join_tree(
    std::unordered_map<std::string, Operator *> table_operator_map, FilterStmt *join_conds)
{
  Operator *root = nullptr;
  if (table_operator_map.size() == 1) {
    for (auto iter : table_operator_map) {
      return iter.second;
    }
  }
  for (auto iter : table_operator_map){
    fathers[iter.first] = iter.first;
  }

  for (const FilterUnit *join_cond : join_conds->filter_units()) {
    FieldExpr *left_expr = (FieldExpr *)join_cond->left();
    FieldExpr *right_expr = (FieldExpr *)join_cond->right();
    const char *left_table = left_expr->table_name();
    const char *right_table = right_expr->table_name();
    if(find_father(left_table).compare(find_father(right_table)) == 0){
      continue;
    }

    std::vector<const FilterUnit *> conds;
    for (const FilterUnit *iter : join_conds->filter_units()) {
      FieldExpr *l_expr = (FieldExpr *)iter->left();
      FieldExpr *r_expr = (FieldExpr *)iter->right();
      const char *l_table = l_expr->table_name();
      const char *r_table = r_expr->table_name();
      if (strcmp(left_table, l_table) == 0 && strcmp(right_table, r_table) == 0){
        conds.push_back(iter);
      } 
      else if (strcmp(left_table, r_table) == 0 
            && strcmp(right_table, l_table) == 0){
        FilterUnit *new_cond = new FilterUnit;
        iter->commutation(new_cond);
        conds.push_back(new_cond);
      }
    }

    root = new JoinOperator(
        table_operator_map[find_father(left_table)], 
        table_operator_map[find_father(right_table)], 
        conds);
    merge(left_table, right_table);
    table_operator_map[find_father(left_table)] = root;
  }
  
  //剩余的operator进行笛卡尔积
  root = nullptr;
  for (auto iter : table_operator_map){
    const std::string table_name = iter.first;

    if (table_name.compare(fathers[table_name])==0){
      if (root == nullptr){
        root = table_operator_map[table_name];
      } else {
        Operator *oper = iter.second;
        std::vector<const FilterUnit *> conds;
        root = new JoinOperator(root, oper, conds);
      }
    }
  }
  return root;
}