/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its
affiliates. All rights reserved. miniob is licensed under Mulan PSL v2. You can
use this software according to the terms and conditions of the Mulan PSL v2. You
may obtain a copy of Mulan PSL v2 at: http://license.coscl.org.cn/MulanPSL2 THIS
SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2022/07/05.
//

#pragma once

#include "common/log/log.h"
#include "storage/common/field_meta.h"
#include "storage/common/table.h"

class Field {
   public:
    Field() = default;
    Field(const Table* table, const FieldMeta* field)
        : table_(table),
          field_(field),
          field_size_(field->len()),
          aggr_type_(NOTAGGR) {}

    Field(const Table* table, const FieldMeta* field, const RelAttr& rel_attr)
        : Field(table, field) {
        aggr_type_ = rel_attr.aggre_type;
        switch (rel_attr.aggre_type) {
            case MIN:
            case MAX:
            case SUM:
            case AVG:
                if (rel_attr.aggre_type == CHARS) {
                    LOG_ERROR("aggregation of strings is not supported");
                    if (rel_attr.aggre_type == SUM ||
                        rel_attr.aggre_type == AVG) {
                        // return RC::MISMATCH;
                    }
                    aggr_attr_type_ = CHARS;
                    // field_size_ = sizeof(char*);
                } else if (rel_attr.aggre_type == INTS) {
                    field_size_ = sizeof(int);
                    aggr_attr_type_ = INTS;
                    if (rel_attr.aggre_type == AVG) {
                        // return RC::MISMATCH;
                        field_size_ = sizeof(double);
                        aggr_attr_type_ = FLOATS;
                    }
                } else if (rel_attr.aggre_type == FLOATS) {
                    field_size_ = sizeof(double);
                    aggr_attr_type_ = FLOATS;
                }
                break;
            case COUNT:
                field_size_ = sizeof(int);
                aggr_attr_type_ = INTS;
                break;
            default:
                break;
        }
    }

    size_t field_size() const {
        if (aggr_type_ != NOTAGGR) {
            return field_size_;
        }
        return field_size_;
    }

    const Table* table() const { return table_; }
    const FieldMeta* meta() const { return field_; }

    AttrType attr_type() const {
        if (aggr_type_ != NOTAGGR) {
            return aggr_attr_type_;
        }
        return field_->type();
    }

    const char* table_name() const { return table_->name(); }
    const char* field_name() const {
        if (aggr_type_ == NOTAGGR) {
            return field_->name();
        } else {
            if (field_ == nullptr) {
                // count(*)
                return "count";
            }
            std::string field_name;
            std::string attr_name(field_->name());
            switch (aggr_type_) {
                case MIN:
                    field_name = "MIN(" + attr_name + ")";
                    break;
                case MAX:
                    field_name = "MAX(" + attr_name + ")";
                    break;
                case COUNT:
                    field_name = "COUNT(" + attr_name + ")";
                    break;
                case AVG:
                    field_name = "AVG(" + attr_name + ")";
                    break;
                case SUM:
                    field_name = "SUM(" + attr_name + ")";
                    break;
            }
            return field_name.c_str();
        }
    }

    void set_table(const Table* table) { this->table_ = table; }
    void set_field(const FieldMeta* field) { this->field_ = field; }

   private:
    const Table* table_ = nullptr;
    const FieldMeta* field_ = nullptr;  //聚合字段信息
    size_t field_size_;                 //聚合结果长度
    AttrType aggr_attr_type_;           //聚合结果类型
    AggrType aggr_type_;                //聚合操作类型
};