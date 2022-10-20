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
// Created by Meiyi & wangyunlai.wyl on 2021/5/19.
//

#include "storage/index/bplus_tree_index.h"
#include "common/log/log.h"

BplusTreeIndex::~BplusTreeIndex() noexcept
{
  close();
}

RC BplusTreeIndex::create(const char *file_name, const IndexMeta &index_meta, const std::vector<const FieldMeta*> &field_metas)
{
  if (inited_) {
    LOG_WARN("Failed to create index due to the index has been created before. file_name:%s, index:%s, field:%s",
        file_name,
        index_meta.name(),
        index_meta.field_text());
    return RC::RECORD_OPENNED;
  }

  Index::init(index_meta, field_metas);

  AttrType *types = new AttrType[field_metas.size()];
  int *attr_lens = new int[field_metas.size()];

  for(int i=0;i<field_metas.size();i++) {
    types[i] = field_metas[i]->type();
    attr_lens[i] = field_metas[i]->len();
  }

  // copying of types and attr_lens assumed.
  RC rc = index_handler_.create(file_name, field_metas.size(), types, attr_lens);
  delete []types;
  delete []attr_lens;

  if (RC::SUCCESS != rc) {
    LOG_WARN("Failed to create index_handler, file_name:%s, index:%s, field:%s, rc:%s",
        file_name,
        index_meta.name(),
        index_meta.field_text(),
        strrc(rc));
    return rc;
  }

  inited_ = true;
  LOG_INFO(
      "Successfully create index, file_name:%s, index:%s, field:%s", file_name, index_meta.name(), index_meta.field_text());
  return RC::SUCCESS;
}

RC BplusTreeIndex::drop()
{
  index_handler_.drop();
  return RC::SUCCESS;
}

RC BplusTreeIndex::open(const char *file_name, const IndexMeta &index_meta, const std::vector<const FieldMeta*> &field_metas)
{
  if (inited_) {
    LOG_WARN("Failed to open index due to the index has been initedd before. file_name:%s, index:%s, field:%s",
        file_name,
        index_meta.name(),
        index_meta.field_text());
    return RC::RECORD_OPENNED;
  }

  Index::init(index_meta, field_metas);

  RC rc = index_handler_.open(file_name);
  if (RC::SUCCESS != rc) {
    LOG_WARN("Failed to open index_handler, file_name:%s, index:%s, field:%s, rc:%s",
        file_name,
        index_meta.name(),
        index_meta.field_text(),
        strrc(rc));
    return rc;
  }

  inited_ = true;
  LOG_INFO(
      "Successfully open index, file_name:%s, index:%s, field:%s", file_name, index_meta.name(), index_meta.field_text());
  return RC::SUCCESS;
}

RC BplusTreeIndex::close()
{
  if (inited_) {
    LOG_INFO("Begin to close index, index:%s, field:%s",
        index_meta_.name(), index_meta_.field_text());
    index_handler_.close();
    inited_ = false;
  }
  LOG_INFO("Successfully close index.");
  return RC::SUCCESS;
}

// need to delete[] return value after use
const char **BplusTreeIndex::generate_keys_from_record(const char *record) {
  const char **res = new const char*[field_metas_.size()];
  int i = 0;
  for(auto &field_meta : field_metas_) {
    res[i] = record + field_meta.offset();
    i++;
  }
  return res;
}

RC BplusTreeIndex::insert_entry(const char *record, const RID *rid)
{
  const char **user_keys = generate_keys_from_record(record);
  RC rc = index_handler_.insert_entry(user_keys, rid);
  delete[] user_keys;
  return rc;
}

RC BplusTreeIndex::delete_entry(const char *record, const RID *rid)
{
  const char **user_keys = generate_keys_from_record(record);
  RC rc = index_handler_.delete_entry(user_keys, rid);
  delete[] user_keys;
  return rc;
}

IndexScanner *BplusTreeIndex::create_scanner(const char *left_keys[], const int left_lens[], bool left_inclusive,
					     const char *right_keys[], const int right_lens[], bool right_inclusive)
{
  BplusTreeIndexScanner *index_scanner = new BplusTreeIndexScanner(index_handler_);
  RC rc = index_scanner->open(left_keys, left_lens, left_inclusive, right_keys, right_lens, right_inclusive);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open index scanner. rc=%d:%s", rc, strrc(rc));
    delete index_scanner;
    return nullptr;
  }
  return index_scanner;
}

RC BplusTreeIndex::sync()
{
  return index_handler_.sync();
}

////////////////////////////////////////////////////////////////////////////////
BplusTreeIndexScanner::BplusTreeIndexScanner(BplusTreeHandler &tree_handler) : tree_scanner_(tree_handler)
{}

BplusTreeIndexScanner::~BplusTreeIndexScanner() noexcept
{
  tree_scanner_.close();
}

RC BplusTreeIndexScanner::open(const char *left_keys[], const int left_lens[], bool left_inclusive,
                               const char *right_keys[], const int right_lens[], bool right_inclusive)
{
  return tree_scanner_.open(left_keys, left_lens, left_inclusive, right_keys, right_lens, right_inclusive);
}

RC BplusTreeIndexScanner::next_entry(RID *rid)
{
  return tree_scanner_.next_entry(rid);
}

RC BplusTreeIndexScanner::destroy()
{
  delete this;
  return RC::SUCCESS;
}
