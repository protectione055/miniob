#pragma once

#include "sql/operator/operator.h"
#include "rc.h"

class FilterStmt;

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