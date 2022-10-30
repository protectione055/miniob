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

class Table;

class TupleCellSpec
{
public:
  TupleCellSpec() = default;
  TupleCellSpec(Expression *expr) : expression_(expr)
  {}

  ~TupleCellSpec()
  {
    if (expression_) {
      delete expression_;
      expression_ = nullptr;
    }
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
    cell.set_type(field_meta->type());
    cell.set_data(this->record_->data() + field_meta->offset());
    cell.set_length(field_meta->len());
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
      const Field &field = field_expr->field();
      if (0 == strcmp(field_name, field.field_name())) {
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

  Record &record()
  {
    return *record_;
  }

  const Record &record() const
  {
    return *record_;
  }
private:
  Record *record_ = nullptr;
  const Table *table_ = nullptr;
  std::vector<TupleCellSpec *> speces_;
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
private:
  std::vector<TupleCellSpec *> speces_;
  Tuple *tuple_ = nullptr;
};

// 查询结束即销毁的临时tuple
class TempTuple : public Tuple {
public:
  TempTuple() = default;
  virtual ~TempTuple()
  {
    for (TupleCellSpec *spec : speces_) {
      delete spec;
    }
    speces_.clear();
    if (record_.data() != nullptr) {
      free(record_.data());
    }
  }

  TempTuple(const TempTuple &t)
  {
    size_t data_len = 0.;
    for (int i = 0; i < t.cell_num(); i++) {
      TupleCell cell;
      const TupleCellSpec *cell_spec;
      t.cell_at(i, cell);
      t.cell_spec_at(i, cell_spec);
      data_len += cell.length();
      FieldExpr *field_expr = (FieldExpr *)cell_spec->expression();
      Field field = field_expr->field();
      FieldExpr *new_field_expr = new FieldExpr(field.table(), field.meta());
      TupleCellSpec *new_spec = new TupleCellSpec(new_field_expr);
      this->speces_.push_back(new_spec);
    }
    char *data = nullptr;
    if (data_len > 0) {
      data = new char[data_len];
      memcpy(data, t.record().data(), data_len);
      record_.set_data(data);
    }
  }

  TempTuple(const Tuple *other)
  {
    std::vector<FieldMeta *> fields;
    size_t first_offset = 0;
    for (size_t i = 0; i < other->cell_num(); i++) {
      const TupleCellSpec *cell_spec = nullptr;
      other->cell_spec_at(i, cell_spec);
      FieldExpr *field_expr = (FieldExpr *)cell_spec->expression();
      const FieldMeta *field_meta = field_expr->field().meta();
      if (i == 0) {
        // 记录第一个可见字段的offset，临时tuple中不保存sys_fields
        first_offset = field_meta->offset();
      }
      FieldMeta *new_field_meta = new FieldMeta();
      new_field_meta->init(field_meta->name(),
          field_meta->type(),
          field_meta->offset() - first_offset,
          field_meta->len(),
          field_meta->visible());
      fields.push_back(new_field_meta);
    }
    this->set_schema(fields);

    size_t offset = 0;
    for (size_t i = 0; i < other->cell_num(); i++) {
      FieldMeta *field_meta = fields[i];
      TupleCell cell;
      other->cell_at(i, cell);
      memcpy(this->record_.data() + offset, cell.data(), field_meta->len());
      offset += field_meta->len();
    }
  }

  TempTuple &operator=(const TempTuple &other)
  {
    if (&other != this) {
      size_t data_len = 0.;
      for (int i = 0; i < other.cell_num(); i++) {
        TupleCell cell;
        const TupleCellSpec *cell_spec;
        other.cell_at(i, cell);
        other.cell_spec_at(i, cell_spec);
        data_len += cell.length();
        FieldExpr *field_expr = (FieldExpr *)cell_spec->expression();
        Field field = field_expr->field();
        FieldExpr *new_field_expr = new FieldExpr(field.table(), field.meta());
        TupleCellSpec *new_spec = new TupleCellSpec(new_field_expr);
        this->speces_.push_back(new_spec);
      }
      char *data = nullptr;
      if (data_len > 0) {
        data = new char[data_len];
        memcpy(data, other.record().data(), data_len);
        record_.set_data(data);
      }
    }

    return *this;
  }

  // 元组比较
  int compare(const TempTuple &other) const
  {
    assert(this->speces_.size() == other.speces_.size());
    int res = 0;
    if (&other == this) {
      return res;
    }
    for (size_t i = 0; i < speces_.size(); i++) {
      TupleCell this_cell, other_cell;
      this->cell_at(i, this_cell);
      other.cell_at(i, other_cell);
      int cmp_res = this_cell.compare(other_cell);
      if (cmp_res == 0)
        continue;
      else {
        res = cmp_res;
        break;
      }
    }
    return res;
  }

  void set_schema(const std::vector<FieldMeta *> fields)
  {
    speces_.clear();
    this->speces_.reserve(fields.size());
    size_t data_len = 0;
    for (FieldMeta *field : fields) {
      data_len += field->len();
      speces_.push_back(new TupleCellSpec(new FieldExpr(nullptr, field)));
    }
    char *data = (char *)malloc(sizeof(char) * data_len);
    memset(data, 0, sizeof(char) * data_len);
    record_.set_data(data);
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
    cell.set_type(field_meta->type());
    cell.set_data(this->record_.data() + field_meta->offset());
    cell.set_length(field_meta->len());
    return RC::SUCCESS;
  }

  RC find_cell(const Field &field, TupleCell &cell) const override
  {
    const char *field_name = field.field_name();
    for (size_t i = 0; i < speces_.size(); ++i) {
      const FieldExpr *field_expr = (const FieldExpr *)speces_[i]->expression();
      const Field &field = field_expr->field();
      if (0 == strcmp(field_name, field.field_name())) {
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

  Record &record()
  {
    return record_;
  }

  const Record &record() const
  {
    return record_;
  }

  void reset_data()
  {
    TupleCellSpec *last_cell = *(speces_.end() - 1);
    FieldExpr *expr = (FieldExpr *)last_cell->expression();
    const FieldMeta *last_meta = expr->field().meta();
    size_t data_len = last_meta->offset() + last_meta->len();
    memset(record_.data(), 0, data_len);
  }

private:
  std::vector<TupleCellSpec *> speces_;
  Record record_;
};

class Key {
public:
  Key() = default;
  ~Key() = default;

  //需要深拷贝
  Key(const Key &other)
  {
    this->key_ = other.key_;
  }

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
    std::vector<FieldMeta *> key_fieldmetas;
    size_t offset = 0;
    for (const Field &field : group_by_keys) {
      const FieldMeta *field_meta = field.meta();
      FieldMeta *key_meta = new FieldMeta();
      key_meta->init(field_meta->name(), field_meta->type(), offset, field_meta->len(), true);
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
      TupleCell key_cell;
      rc = key_.cell_at(i, key_cell);
      if (rc != RC::SUCCESS) {
        LOG_ERROR("failed to find cell in key");
        return rc;
      }
      memcpy(const_cast<char *>(key_cell.data()), tuple_cell.data(), key_cell.length());
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