#pragma once

#include "sql/operator/operator.h"
#include "rc.h"

class FilterStmt;

class Key {
public:
  Key() = default;
  ~Key() = default;

  //需要深拷贝
  Key(const Key &other)
  {
    key_ = TempTuple(other.key_);
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

class HashAggregateOperator : public Operator {
public:
  HashAggregateOperator(
      const std::vector<Field> &query_fields, const std::vector<Field> &group_by_keys, const FilterStmt *having_stmt)
      : query_fields_(query_fields), group_by_keys_(group_by_keys), having_stmt_(having_stmt)
  {
    for (const Field &field : query_fields_) {
      field_meta_.push_back(const_cast<FieldMeta *>(field.meta()));
    }
  }

  virtual ~HashAggregateOperator() = default;

  RC open() override;
  RC next() override;
  RC close() override;

  int tuple_cell_num() const
  {
    return query_fields_.size();
  }

  RC tuple_cell_spec_at(int index, const TupleCellSpec *&spec) const;

  Tuple *current_tuple() override;

protected:
  bool having(const Tuple *tuple, const FilterStmt *having_stmt) const;

private:
  std::vector<Field> query_fields_;
  std::vector<FieldMeta *> field_meta_;  //聚合结果的fieldmeta
  const FilterStmt *having_stmt_ = nullptr;
  std::vector<Field> group_by_keys_;
  std::map<Key, TempTuple *> tuples_;
  std::map<Key, std::map<int, int>> valid_count_;  //记录每个group上的各个avg结果的除数，只计算非空值的数量
  bool first_fetch_ = true;                        //标记是否取第一个元组
  std::map<Key, TempTuple *>::iterator iter_;
};