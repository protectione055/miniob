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
// Created by Wangyunlai on 2021/5/14.
//

#pragma once

#include <memory>
#include <vector>

#include "common/log/log.h"
#include "sql/parser/parse.h"
#include "sql/expr/tuple_cell.h"
#include "sql/expr/expression.h"
#include "storage/record/record.h"
#include "sql/stmt/typecast.h"

class Table;

class TupleCellSpec
{
public:
  TupleCellSpec() = default;
  TupleCellSpec(Expression *expr) : expression_(expr)
  {}

  ~TupleCellSpec()
  {
    // isn't really safe to delete expression_
    // memory leak it is :D
  }

  void set_alias(const char *alias)
  {
    this->alias_ = alias;
  }
  const char *alias() const
  {
    return alias_;
  }

  Expression *expression() const
  {
    return expression_;
  }

private:
  const char *alias_ = nullptr;
  Expression *expression_ = nullptr;
};

class Tuple
{
public:
  Tuple() = default;
  virtual ~Tuple() = default;

  virtual int cell_num() const = 0; 
  virtual RC  cell_at(int index, TupleCell &cell) const = 0;
  virtual RC  find_cell(const Field &field, TupleCell &cell) const = 0;

  virtual RC  cell_spec_at(int index, const TupleCellSpec *&spec) const = 0;

  virtual bool is_null_at(int index) const
  {
    return false;
  }
};

class RowTuple : public Tuple
{
public:
  RowTuple() = default;
  virtual ~RowTuple()
  {
    for (TupleCellSpec *spec : speces_) {
      delete spec;
    }
    speces_.clear();
  }
  
  void set_record(Record *record)
  {
    this->record_ = record;
  }

  void set_schema(const Table *table, const std::vector<FieldMeta> *fields)
  {
    table_ = table;
    this->speces_.reserve(fields->size());
    for (const FieldMeta &field : *fields) {
      speces_.push_back(new TupleCellSpec(new FieldExpr(table, &field)));
    }
    nullmap_meta_ = table->table_meta().field("__nullmap");
  }

  int cell_num() const override
  {
    return speces_.size();
  }

  RC cell_at(int index, TupleCell &cell) const override
  {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      LOG_WARN("invalid argument. index=%d", index);
      return RC::INVALID_ARGUMENT;
    }

    const TupleCellSpec *spec = speces_[index];
    FieldExpr *field_expr = (FieldExpr *)spec->expression();
    const FieldMeta *field_meta = field_expr->field().meta();
    if(is_null_at(index)) {
      cell.set_null();
    } else {
      cell.set_type(field_meta->type());
      cell.set_data(this->record_->data() + field_meta->offset());
      cell.set_length(field_meta->len());
    }
    return RC::SUCCESS;
  }

  RC find_cell(const Field &field, TupleCell &cell) const override
  {
    const char *table_name = field.table_name();
    if (0 != strcmp(table_name, table_->name())) {
      return RC::NOTFOUND;
    }

    const char *field_name = field.field_name();
    for (size_t i = 0; i < speces_.size(); ++i) {
      const FieldExpr * field_expr = (const FieldExpr *)speces_[i]->expression();
      const Field &this_field = field_expr->field();
      if (0 == strcmp(field_name, this_field.field_name())) {
        if (strcmp(field.alias(), this_field.alias()) == 0) {
          return cell_at(i, cell);
        }
      }
    }
    return RC::NOTFOUND;
  }

  RC cell_spec_at(int index, const TupleCellSpec *&spec) const override
  {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      LOG_WARN("invalid argument. index=%d", index);
      return RC::INVALID_ARGUMENT;
    }
    spec = speces_[index];
    return RC::SUCCESS;
  }

  Record &record()
  {
    return *record_;
  }

  const Record &record() const
  {
    return *record_;
  }

  virtual bool is_null_at(int index) const override
  {
    if(nullmap_meta_ == nullptr) return false;
    int nullmap = (int)this->record_->data()[nullmap_meta_->offset()]; // 32bit null bitmap
    return (nullmap >> index)&1;
  }
private:


  Record *record_ = nullptr;
  const Table *table_ = nullptr;
  std::vector<TupleCellSpec *> speces_;
  const FieldMeta *nullmap_meta_ = nullptr;
};

/*
class CompositeTuple : public Tuple
{
public:
  int cell_num() const override; 
  RC  cell_at(int index, TupleCell &cell) const = 0;
private:
  int cell_num_ = 0;
  std::vector<Tuple *> tuples_;
};
*/

class ProjectTuple : public Tuple
{
public:
  ProjectTuple() = default;
  virtual ~ProjectTuple()
  {
    for (TupleCellSpec *spec : speces_) {
      delete spec;
    }
    speces_.clear();
  }

  void set_tuple(Tuple *tuple)
  {
    this->tuple_ = tuple;
  }

  void add_cell_spec(TupleCellSpec *spec)
  {
    speces_.push_back(spec);
  }
  int cell_num() const override
  {
    return speces_.size();
  }

  RC cell_at(int index, TupleCell &cell) const override
  {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      return RC::GENERIC_ERROR;
    }
    if (tuple_ == nullptr) {
      return RC::GENERIC_ERROR;
    }

    const TupleCellSpec *spec = speces_[index];
    return spec->expression()->get_value(*tuple_, cell);
  }

  RC find_cell(const Field &field, TupleCell &cell) const override
  {
    return tuple_->find_cell(field, cell);
  }
  RC cell_spec_at(int index, const TupleCellSpec *&spec) const override
  {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      return RC::NOTFOUND;
    }
    spec = speces_[index];
    return RC::SUCCESS;
  }
  virtual bool is_null_at(int index) const override
  {
    return tuple_->is_null_at(index);
  }
private:
  std::vector<TupleCellSpec *> speces_;
  Tuple *tuple_ = nullptr;
};

// 查询结束即销毁的临时tuple
// TODO: This class is highly similar to RowTuple. It's basically an enhanced version
// of RowTuple, and shares a lot of similar functionalities. Maybe we should merge them?   
// 
// TempTuple doesn't have __trx like a RowRecord does
// And __nullmap isn't stored as a field, instead it's just a variable in memory
class TempTuple : public Tuple {
public:
  TempTuple() {};
  virtual ~TempTuple()
  {
    for (TupleCellSpec *spec : speces_) {
      // delete spec;

      // we messed up and lost track of how and where we share
      // our pointers, so it's not safe to delete here
      // oh well, memory leak again hhhh
    }
    speces_.clear();
    if (data_ != nullptr) {
      free(data_); // realloc-ed
      data_ = nullptr;
    }
  }

  TempTuple(const TempTuple &t) {
    append_tuple(t);
  }

  TempTuple &operator=(const TempTuple &t) {
    append_tuple(t);
    return *this;
  }

  TempTuple(const Tuple &t) {
    append_tuple(t);
  }

  TempTuple(const Tuple &a, const Tuple &b){
    append_tuple(a);
    append_tuple(b);
  }
  
  // 元组比较
  int compare(const Tuple &other) const {
    if (&other == this) {
      return 0;
    }
    for (size_t i = 0; i < speces_.size(); i++) {
      TupleCell this_cell, other_cell;
      this->cell_at(i, this_cell);
      other.cell_at(i, other_cell);
      int cmp_res = this_cell.compare(other_cell);
      if (cmp_res != 0) {
        return cmp_res;
      }
    }
    return 0;
  }

  // all fields are cleared to null by default
  void set_schema(const std::vector<const FieldMeta *> fields)
  {
    speces_.clear();
    this->speces_.reserve(fields.size());
    nullmap_ = 0;
    data_len_ = 0;
    for (const FieldMeta *field : fields) {
      data_len_ += field->len();
      speces_.push_back(new TupleCellSpec(new FieldExpr(nullptr, field)));
    }
    data_ = (char*)realloc(data_, data_len_);
    memset(data_, 0, data_len_);
    for(size_t i=0;i<fields.size();i++) {
      set_null(i, true);
    }
  }

  int cell_num() const override { return speces_.size(); }

  RC cell_at(int index, TupleCell &cell) const override
  {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      LOG_WARN("invalid argument. index=%d", index);
      return RC::INVALID_ARGUMENT;
    }

    const TupleCellSpec *spec = speces_[index];
    FieldExpr *field_expr = (FieldExpr *)spec->expression();
    const FieldMeta *field_meta = field_expr->field().meta();
    if(is_null_at(index)) { // check null
      cell.set_null();
    } else {
      cell.set_type(field_meta->type());
      cell.set_data(data_ + field_meta->offset());
      cell.set_length(field_meta->len());
    }
    return RC::SUCCESS;
  }

  // pass nullptr as data to set null, otherwise clears null
  // todo: check nullability
  RC cell_set(int index, const char *data) {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      LOG_WARN("cell_set: invalid argument. index=%d", index);
      return RC::INVALID_ARGUMENT;
    }
    if(data == nullptr) {
      set_null(index, true);
    } else {
      set_null(index, false);
      const FieldMeta *field_meta = field_by_index(index);
      memcpy(&data_[field_meta->offset()], data, field_meta->len());
    }
    return RC::SUCCESS;
  }

  // convenience method, for aggregating.
  // *sets cell to v if null*
  RC cell_value_add(int index, Value v) {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      LOG_WARN("cell_value_add: invalid argument. index=%d", index);
      return RC::INVALID_ARGUMENT;
    }
    const FieldMeta *field_meta = field_by_index(index);
    AttrType attr_type = field_meta->type();
    char *data = &data_[field_meta->offset()];
    if(is_null_at(index)) {
      set_null(index, false);
    }
    if (attr_type != v.type) {
      RC rc = try_typecast(&v, v, attr_type);
      if (rc != RC::SUCCESS) {
        LOG_ERROR("failed to typecast from type %d to %d", v.type, attr_type);
        return rc;
      }
    }

    switch (field_meta->type()) {
      case INTS:
        *(int *)data += *(int *)v.data;
        break;
      case FLOATS:
        *(float *)data += *(float *)v.data;
        break;
      default:
        LOG_WARN("cast unknown type to float, god knows what's gonna happen..");
        *(float *)data += *(float *)v.data;
        break;
    }
    return RC::SUCCESS;
  }

  // convenience method, for aggregating.
  // *sets cell to delta if null*
  RC cell_int_add(int index, int delta) {
    return cell_value_add(index, Value{INTS, (void*)&delta});
  }

  RC find_cell(const Field &field, TupleCell &cell) const override
  {
    for (size_t i = 0; i < speces_.size(); i++) {
      const Field &cur_field = ((const FieldExpr *)speces_[i]->expression())->field();
      if (strcmp(field.field_name(), cur_field.field_name()) == 0) {
        if(field.table() && cur_field.table() && field.table_name() != cur_field.table_name()) continue;
        return cell_at(i, cell);
      }
    }
    return RC::NOTFOUND;
  }

  RC cell_spec_at(int index, const TupleCellSpec *&spec) const override
  {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      LOG_WARN("invalid argument. index=%d", index);
      return RC::INVALID_ARGUMENT;
    }
    spec = speces_[index];
    return RC::SUCCESS;
  }

  const char *data() const { return data_; }
  void reset_data() { memset(data_, 0, data_len_); }

  virtual bool is_null_at(int index) const override {
    return ((nullmap_ >> index)&1);
  }

private:

  void append_tuple(const Tuple &t) {
    size_t new_data_len = data_len_; 
    size_t old_field_num = speces_.size();
    bool *is_invisible = new bool[t.cell_num()]; 
    size_t *field_lens = new size_t[t.cell_num()]; 
    for (int i = 0; i < t.cell_num(); i++) {
      const TupleCellSpec *cell_spec;
      t.cell_spec_at(i, cell_spec);
      Field field = ((FieldExpr *)cell_spec->expression())->field();
      const FieldMeta *meta = field.meta();
      is_invisible[i] = !meta->visible();
      if(is_invisible[i]) { // skip invisible fields (eg. sys_fields)
        continue;
      }
      FieldMeta *new_meta = new FieldMeta();
      new_meta->init(meta->name(), meta->type(), new_data_len, meta->len(), meta->visible(), meta->nullable());
      field_lens[i] = new_meta->len();
      new_data_len += field_lens[i];
      speces_.push_back(new TupleCellSpec(new FieldExpr(field.table(), new_meta)));
    }

    if (new_data_len > data_len_) { // resize data_, then copy new data over
      data_ = (char*)realloc(data_, new_data_len);

      int idxoffset = 0; // offset caused by the absence of invisible fields
      for (int i = 0; i < t.cell_num(); i++) {
        if(is_invisible[i]) { // skip invisible fields (eg. sys_fields)
          idxoffset++;
          continue;
        }
        TupleCell cell;
        t.cell_at(i, cell);
        if(cell.attr_type() == NULLS) {
          set_null(old_field_num + i - idxoffset, true);
        } else {
          cell_set(old_field_num + i - idxoffset, cell.data());
        }
      }
    }
    data_len_ = new_data_len;
    delete[] is_invisible;
    delete[] field_lens;
  }

  const FieldMeta *field_by_index(int index) {
    const TupleCellSpec *spec = speces_[index];
    FieldExpr *field_expr = (FieldExpr *)spec->expression();
    return field_expr->field().meta();
  }

  // clears underlying data to 0 whenever null state changes
  void set_null(int index, bool is_null) {
    const FieldMeta *field_meta = field_by_index(index);
    memset(data_ + field_meta->offset(), 0, field_meta->len());
    if(is_null) {
      nullmap_ |= 1<<index;
    } else {
      nullmap_ &= ~(1<<index);
    }
  }

  std::vector<TupleCellSpec *> speces_;
  char *data_ = nullptr;
  size_t data_len_ = 0;
  int nullmap_ = 0;
};

class Key {
public:
  Key() = default;
  ~Key() = default;

  //需要深拷贝
  Key(const Key &other) : key_(other.key_)
  {}

  Key &operator=(const Key &other)
  {
    if (this != &other) {
      key_ = TempTuple(other.key_);
    }
    return *this;
  }

  RC init(std::vector<Field> &group_by_keys, const Tuple &tuple)
  {
    RC rc = RC::SUCCESS;

    // 构造key schema
    std::vector<const FieldMeta *> key_fieldmetas;
    size_t offset = 0;
    for (const Field &field : group_by_keys) {
      const FieldMeta *field_meta = field.meta();
      FieldMeta *key_meta = new FieldMeta();
      key_meta->init(field_meta->name(), field_meta->type(), offset, field_meta->len(), true, field_meta->nullable());
      key_fieldmetas.push_back(key_meta);
      offset += key_meta->len();
    }
    key_.set_schema(key_fieldmetas);

    // 拷贝数据到key
    for (int i = 0; i < key_.cell_num(); i++) {
      TupleCell tuple_cell;
      rc = tuple.find_cell(group_by_keys[i], tuple_cell);
      if (rc != RC::SUCCESS) {
        LOG_ERROR("failed to find cell in tuple");
        return rc;
      }
      rc = key_.cell_set(i, tuple_cell.data());
      if (rc != RC::SUCCESS) {
        LOG_ERROR("failed to set cell in key");
        return rc;
      }
    }
    return rc;
  }

  bool operator<(const Key &other) const
  {
    return (key_.compare(other.key_) < 0);
  }

private:
  TempTuple key_;
};