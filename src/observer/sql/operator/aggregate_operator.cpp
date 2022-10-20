#include "sql/operator/aggregate_operator.h"
#include "common/log/log.h"
#include "sql/parser/parse_defs.h"
#include "storage/common/table.h"
#include "storage/record/record.h"

RC AggregateOperator::open() {
    if (children_.size() != 1) {
        LOG_WARN("project operator must has 1 child");
        return RC::INTERNAL;
    }

    Operator* child = children_[0];
    RC rc = child->open();
    if (rc != RC::SUCCESS) {
        LOG_WARN("failed to open child operator: %s", strrc(rc));
        return rc;
    }

    Record* record = new Record();
    char* data = nullptr;
    // 构造结果tuple
    size_t tuple_size = 0;
    std::vector<FieldMeta> tuple_meta;
    for (auto& aggr_field : aggr_fields_) {
        tuple_meta.push_back(FieldMeta(aggr_field.field_name(),
                                       aggr_field->attr_type, tuple_size,
                                       aggr_field.field_size(), true));
        tuple_size += aggr_field.field_size();
    }
    tuple_.set_schema(aggr_field.table(), &tuple_meta);
    data = (char*)malloc(tuple_size);
    record->set_data(data);
    tuple_.set_record(record);

    return RC::SUCCESS;
}

RC AggregateOperator::next() {
    // 从子节点获取tuple进行聚合，结果保存在tuple_
    char* data = tuple_.record()->data();
    for (RC rc = children_[0]->next(); rc == RC::SUCCESS;
         rc = children_[0]->next()) {
        Tuple* tuple_from_child = children_[0]->current_tuple();
        size_t offset = 0;
        for (int i = 0; i < aggr_fields_.size(0); i++) {
            // TODO(zzm): 获取子节点所有tuple
            const Field& field = aggr_fields[i];
            if (field.meta() == nullptr) {
                // select count(*)
				//用cell
                int* cell_int = (int*)(data + offset);
                *cell_int++;
                offset += field.field_size();
            } else {
                switch (field.aggr_type_) {
                    case MIN:
                    case MAX:
                        if (field.aggr_attr_type_ == INTS) {
                            int* cell_int = (int*)(data + offset);
                        }
                }
            }
        }
    }
    return rc;
}

void AggregateOperator::add_field(Field field) {
    field_.push_back(field);
}

RC AggregateOperator::close() {
    children_[0]->close();
    return RC::SUCCESS;
}
Tuple* AggregateOperator::current_tuple() {
    return &tuple_;
}

RC AggregateOperator::tuple_cell_spec_at(int index,
                                         const TupleCellSpec*& spec) const {
    return tuple_.cell_spec_at(index, spec);
}