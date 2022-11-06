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
// Created by Meiyi & Wangyunlai on 2021/5/12.
//

#ifndef __OBSERVER_STORAGE_COMMON_FIELD_META_H__
#define __OBSERVER_STORAGE_COMMON_FIELD_META_H__

#include <string>

#include "rc.h"
#include "sql/parser/parse_defs.h"

namespace Json {
class Value;
}  // namespace Json

// Take care of shallow copy
class FieldMeta {
public:
  FieldMeta();
  ~FieldMeta() = default;

  RC init(const char *name, AttrType attr_type, int attr_offset, int attr_len, bool visible, bool nullable);

public:
  const char *name() const;
  AttrType type() const;
  int offset() const;
  int len() const;
  bool visible() const;
  bool nullable() const;
  bool is_expr() const;

  // 用来显示例如 AVG(table.relation) 这种格式的一个 dirty hack
  // 没有这个的话，会显示成 table.AVG(relation)
  // no time to do it properly.
  void dirty_hack_set_namefunc(const char *namefunc);
  std::string dirty_hack_name_with_tablename(const char *table_name) const;

public:
  void desc(std::ostream &os) const;
  void set_expr(){is_expr_ = true;}

public:
  void to_json(Json::Value &json_value) const;
  static RC from_json(const Json::Value &json_value, FieldMeta &field);

protected:
  std::string name_;
  std::string dirty_hack_func_;
  std::string dirty_hack_name_with_func_;
  AttrType attr_type_;
  int attr_offset_;
  int attr_len_;
  bool visible_;
  bool nullable_;
  bool is_expr_ = false;
};
#endif  // __OBSERVER_STORAGE_COMMON_FIELD_META_H__