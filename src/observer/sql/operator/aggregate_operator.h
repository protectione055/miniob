#pragma once

#include "sql/operator/operator.h"
#include "rc.h"

class AggregateOperator : public Operator {
public:
  AggregateOperator(const std::vector<Field> query_fields) : query_fields_(query_fields)
  {
    for (const Field &field : query_fields_) {
      field_meta_.push_back(const_cast<FieldMeta *>(field.meta()));
    }
    tuple_.set_schema(field_meta_);
  }

  virtual ~AggregateOperator() = default;

  RC open() override;
  RC next() override;
  RC close() override;

  int tuple_cell_num() const
  {
    return tuple_.cell_num();
  }

  RC tuple_cell_spec_at(int index, const TupleCellSpec *&spec) const;

  Tuple *current_tuple() override;

private:
  std::vector<Field> query_fields_;
  std::vector<FieldMeta *> field_meta_;
  TempTuple tuple_;
};
