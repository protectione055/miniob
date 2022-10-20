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
// Created by Meiyi & Wangyunlai.wyl on 2021/5/18.
//

#include "storage/common/index_meta.h"
#include "storage/common/field_meta.h"
#include "storage/common/table_meta.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "rc.h"
#include "json/json.h"

const static Json::StaticString FIELD_NAME("name");
const static Json::StaticString FIELD_FIELD_NAME("field_name");

RC IndexMeta::init(const char *name, const std::vector<const FieldMeta*> &fields)
{
  if (common::is_blank(name)) {
    LOG_ERROR("Failed to init index, name is empty.");
    return RC::INVALID_ARGUMENT;
  }

  field_text_ = "";
  name_ = name;
  fields_ = new char*[fields.size()];
  for (int i=0;i<fields.size();i++) {
    fields_[i] = new char[128];
    strcpy(fields_[i], fields[i]->name());
    field_text_ += fields_[i];
    if(i != fields.size() - 1) field_text_ += ",";
  }
  num_fields_ = fields.size();
  return RC::SUCCESS;
}

void IndexMeta::to_json(Json::Value &json_value) const
{
  Json::Value arr_fields;
  for (int i=0;i<num_fields_;i++) {
    arr_fields.append(fields_[i]);
  }

  json_value[FIELD_NAME] = name_;
  json_value[FIELD_FIELD_NAME] = arr_fields;
}

RC IndexMeta::from_json(const TableMeta &table, const Json::Value &json_value, IndexMeta &index)
{
  const Json::Value &name_value = json_value[FIELD_NAME];
  const Json::Value &field_value = json_value[FIELD_FIELD_NAME];
  if (!name_value.isString()) {
    LOG_ERROR("Index name is not a string. json value=%s", name_value.toStyledString().c_str());
    return RC::GENERIC_ERROR;
  }

  if (!field_value.isArray()) {
    LOG_ERROR("Field names of index [%s] is not an array. json value=%s",
        name_value.asCString(),
        field_value.toStyledString().c_str());
    return RC::GENERIC_ERROR;
  }

  std::vector<const FieldMeta*> fields;
  for (auto iter = field_value.begin(); iter != field_value.end(); iter++) {
    if (!iter->isString()) {
      LOG_ERROR("Field name [%d] of index [%s] is not a string. json value=%s",
          iter.index(),
          name_value.asCString(),
          field_value.toStyledString().c_str());
      return RC::GENERIC_ERROR;
    }
    
    const FieldMeta *field = table.field(iter->asCString());
    if (field == nullptr) {
      LOG_ERROR("Deserialize index [%s]: no such field: %s", name_value.asCString(), iter->asCString());
      return RC::SCHEMA_FIELD_MISSING;
    }
    fields.push_back(field);
  }

  return index.init(name_value.asCString(), std::move(fields));
}

const char *IndexMeta::name() const
{
  return name_.c_str();
}

const char **IndexMeta::fields() const
{
  return const_cast<const char**>(fields_);
}

int IndexMeta::num_fields() const
{
  return num_fields_;
}

void IndexMeta::desc(std::ostream &os) const
{
  os << "index name=" << name_ << ", fields=[";
  for(int i=0;i<num_fields_;i++) {
    os << fields_[i];
    if(i != num_fields_ - 1) os << ",";
  }
  os << "]";
}