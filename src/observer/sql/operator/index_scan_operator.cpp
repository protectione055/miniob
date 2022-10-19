/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2022/07/08.
//

#include "sql/operator/index_scan_operator.h"
#include "storage/index/index.h"

IndexScanOperator::IndexScanOperator(const Table *table, const Index *index,
		    const std::vector<TupleCell> &left_cells, bool left_inclusive,
		    const std::vector<TupleCell> &right_cells, bool right_inclusive)
  : table_(table), index_(index),
    left_inclusive_(left_inclusive), right_inclusive_(right_inclusive),
    left_cells_(left_cells), right_cells_(right_cells)
{}

RC IndexScanOperator::open()
{
  if (nullptr == table_ || nullptr == index_) {
    return RC::INTERNAL;
  }

  const char **left_keys = nullptr, **right_keys = nullptr;
  int *left_lens = nullptr, *right_lens = nullptr;
  // ugly duplicate code snippet
  if(left_cells_.size() >= 0) {
    left_keys = new const char*[left_cells_.size()];
    left_lens = new int[left_cells_.size()];
    for(int i=0;i<left_cells_.size();i++) {
      left_keys[i] = const_cast<char*>(left_cells_[i].data());
      left_lens[i] = left_cells_[i].length();
    }
  }
  if(right_cells_.size() >= 0) {
    right_keys = new const char*[right_cells_.size()];
    right_lens = new int[right_cells_.size()];
    for(int i=0;i<right_cells_.size();i++) {
      right_keys[i] = const_cast<char*>(right_cells_[i].data());
      right_lens[i] = right_cells_[i].length();
    }
  }
  
  IndexScanner *index_scanner = const_cast<Index*>(index_)->create_scanner(left_keys, left_lens , left_inclusive_,
                                                       right_keys, right_lens, right_inclusive_);
  if (nullptr == index_scanner) {
    LOG_WARN("failed to create index scanner");
    return RC::INTERNAL;
  }

  record_handler_ = table_->record_handler();
  if (nullptr == record_handler_) {
    LOG_WARN("invalid record handler");
    index_scanner->destroy();
    return RC::INTERNAL;
  }
  index_scanner_ = index_scanner;

  tuple_.set_schema(table_, table_->table_meta().field_metas());
  
  return RC::SUCCESS;
}

RC IndexScanOperator::next()
{
  RID rid;
  RC rc = index_scanner_->next_entry(&rid);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  return record_handler_->get_record(&rid, &current_record_);
}

RC IndexScanOperator::close()
{
  index_scanner_->destroy();
  index_scanner_ = nullptr;
  return RC::SUCCESS;
}

Tuple * IndexScanOperator::current_tuple()
{
  tuple_.set_record(&current_record_);
  return &tuple_;
}
