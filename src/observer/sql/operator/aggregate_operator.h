#pragma once

#include "rc.h"
#include "sql/operator/operator.h"

//定义聚合算子
class AggregateOperator : public Operator {
   public:
    AggregateOperator(const std::vector<Field>& fields)
        : aggr_fields_(fields) {}

    virtual ~AggregateOperator() = default;

    RC open() override;
    RC next() override;
    RC close() override;

    int tuple_cell_num() const { return tuple_.cell_num(); }

    void add_field(const Field& field);

    RC tuple_cell_spec_at(int index, const TupleCellSpec*& spec) const;

    AggrType aggregate_oper_at(int index) const;

    Tuple* current_tuple() override;

   private:
    std::vector<Field> aggr_fields_;  //聚合相关的列
    TempTuple tuple_;
};
