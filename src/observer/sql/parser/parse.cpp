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
// Created by Meiyi
//

#include <mutex>
#include "sql/parser/parse.h"
#include "rc.h"
#include "common/log/log.h"

RC parse(char *st, Query *sqln);

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
void relation_attr_init(RelAttr *relation_attr, const char *relation_name, const char *attribute_name)
{
  if (relation_name != nullptr) {
    relation_attr->relation_name = strdup(relation_name);
  } else {
    relation_attr->relation_name = nullptr;
  }
  relation_attr->aggr_type = NOT_AGGR;
  relation_attr->attribute_name = strdup(attribute_name);
}
void relation_attr_destroy(RelAttr *relation_attr)
{
  free(relation_attr->relation_name);
  free(relation_attr->attribute_name);
  relation_attr->relation_name = nullptr;
  relation_attr->attribute_name = nullptr;
}
void order_attr_init(OrderAttr *order_attr, const char *relation_name, const char *attribute_name, const int is_asc)
{
  if (relation_name != nullptr) {
    order_attr->relation_name = strdup(relation_name);
  } else {
    order_attr->relation_name = nullptr;
  }
  order_attr->attribute_name = strdup(attribute_name);
  order_attr->is_asc = is_asc;
}
void order_attr_destroy(OrderAttr *order_attr)
{
  free(order_attr->relation_name);
  free(order_attr->attribute_name);
  order_attr->relation_name = nullptr;
  order_attr->attribute_name = nullptr;
}

void value_init_integer(Value *value, int v)
{
  value->type = INTS;
  value->data = malloc(sizeof(v));
  memcpy(value->data, &v, sizeof(v));
}
void value_init_float(Value *value, float v)
{
  value->type = FLOATS;
  value->data = malloc(sizeof(v));
  memcpy(value->data, &v, sizeof(v));
}
void value_init_string(Value *value, const char *v)
{
  value->type = CHARS;
  value->data = strdup(v);
}
void value_init_date(Value *value, time_t v)
{
  value->type = DATES;
  value->data = malloc(sizeof(v));
  memcpy(value->data, &v, sizeof(v));
}
void value_destroy(Value *value)
{
  value->type = UNDEFINED;
  free(value->data);
  value->data = nullptr;
}

void subquery_destroy(void **query)
{
  LOG_DEBUG("destroy subquery");
  free((Selects *)*query);
  *query = nullptr;
}

void condition_init(Condition *condition, CompOp comp, int left_is_attr, RelAttr *left_attr, Value *left_value,
    int right_is_attr, RelAttr *right_attr, Value *right_value)
{
  condition->comp = comp;
  condition->left_is_attr = left_is_attr;
  condition->right_is_attr = right_is_attr;
  if (left_is_attr) {
    condition->left_expr_type = ATTR;
    condition->left_attr = *left_attr;
  } else {
    condition->left_expr_type = VALUE;
    condition->left_value = *left_value;
  }

  if (right_is_attr) {
    condition->right_expr_type = ATTR;
    condition->right_attr = *right_attr;
  } else {
    condition->right_expr_type = VALUE;
    condition->right_value = *right_value;
  }
}

// ATTR/VALUE/SUB_QUERY
void condition_init_with_subquery(Condition *condition, CompOp comp, CondExprType left_expr_type, RelAttr *left_attr,
    Value *left_value, Selects *left_query, CondExprType right_expr_type, RelAttr *right_attr, Value *right_value,
    Selects *right_query)
{
  condition->comp = comp;
  condition->left_is_attr = 0;
  condition->right_is_attr = 0;
  condition->left_expr_type = left_expr_type;
  condition->right_expr_type = right_expr_type;
  // 确定Select结构体可以直接赋值？
  switch (left_expr_type) {
    case ATTR:
      condition->left_is_attr = 1;
      condition->left_attr = *left_attr;
      break;
    case VALUE:
      condition->left_value = *left_value;
      break;
    case SUB_QUERY:
      condition->left_query = (Selects *)malloc(sizeof(Selects));
      memcpy(condition->left_query, left_query, sizeof(Selects));
      break;
  }

  switch (right_expr_type) {
    case ATTR:
      condition->right_is_attr = 1;
      condition->right_attr = *right_attr;
      break;
    case VALUE:
      condition->right_value = *right_value;
      break;
    case SUB_QUERY:
      condition->right_query = (Selects *)malloc(sizeof(Selects));
      memcpy(condition->right_query, right_query, sizeof(Selects));
      break;
  }
}

void condition_destroy(Condition *condition)
{
  if (condition->left_expr_type == ATTR) {
    relation_attr_destroy(&condition->left_attr);
  } else if (condition->left_expr_type == VALUE){
    value_destroy(&condition->left_value);
  } else {
    subquery_destroy(&condition->left_query);
  }

  if (condition->right_expr_type == ATTR) {
    relation_attr_destroy(&condition->right_attr);
  } else if (condition->right_expr_type == VALUE) {
    value_destroy(&condition->right_value);
  } else {
    subquery_destroy(&condition->right_query);
  }
}

void attr_info_init(AttrInfo *attr_info, const char *name, AttrType type, size_t length)
{
  attr_info->name = strdup(name);
  attr_info->type = type;
  if (type == INTS)
    length = sizeof(int);
  if (type == FLOATS)
    length = sizeof(float);
  if (type == DATES)
    length = sizeof(time_t);
  attr_info->length = length;
}
void attr_info_destroy(AttrInfo *attr_info)
{
  free(attr_info->name);
  attr_info->name = nullptr;
}

void selects_init(Selects *selects, ...);
void selects_append_attribute(Selects *selects, RelAttr *rel_attr)
{
  selects->attributes[selects->attr_num++] = *rel_attr;
}
void selects_append_relation(Selects *selects, const char *relation_name)
{
  selects->relations[selects->relation_num++] = strdup(relation_name);
}
void selects_append_joincond(Selects *selects, Condition condition)
{
  selects->join_conds[selects->join_cond_num++] = condition;
}
void selects_append_conditions(Selects *selects, Condition conditions[], size_t condition_num)
{
  assert(condition_num <= sizeof(selects->conditions) / sizeof(selects->conditions[0]));
  for (size_t i = 0; i < condition_num; i++) {
    selects->conditions[i] = conditions[i];
  }
  selects->condition_num = condition_num;
}
void selects_append_groupkey(Selects *selects, RelAttr *rel_attr)
{
  rel_attr->aggr_type = NOT_AGGR;
  selects->group_by_keys[selects->group_by_key_num++] = *rel_attr;
}
void selects_append_having_conditions(Selects *selects, Condition conditions[], size_t condition_num)
{
  assert(condition_num <= sizeof(selects->having_conditions) / sizeof(selects->having_conditions[0]));
  for (size_t i = 0; i < condition_num; i++) {
    selects->having_conditions[i] = conditions[i];
  }
  selects->having_condition_num = condition_num;
}
void selects_append_orders(Selects *selects, OrderAttr *order_attr)
{
  selects->orders[selects->order_num++] = *order_attr;
}
void selects_destroy(Selects *selects)
{
  for (size_t i = 0; i < selects->attr_num; i++) {
    relation_attr_destroy(&selects->attributes[i]);
  }
  selects->attr_num = 0;

  for (size_t i = 0; i < selects->relation_num; i++) {
    free(selects->relations[i]);
    selects->relations[i] = NULL;
  }
  selects->relation_num = 0;

  for (size_t i = 0; i < selects->join_cond_num; i++) {
    condition_destroy(&selects->join_conds[i]);
  }

  for (size_t i = 0; i < selects->condition_num; i++) {
    condition_destroy(&selects->conditions[i]);
  }
  selects->condition_num = 0;

  for (size_t i = 0; i < selects->order_num; i++) {
    order_attr_destroy(&selects->orders[i]);
  }
  selects->attr_num = 0;
}

void inserts_init(Inserts *inserts, const char *relation_name)
{
  inserts->relation_name = strdup(relation_name);
}
void inserts_create_tuple(Inserts *inserts, Value values[], size_t value_num)
{
  assert(value_num <= sizeof(inserts->values[0]) / sizeof(inserts->values[0][0]));
  for (size_t i = 0; i < value_num; i++) {
    inserts->values[inserts->tuple_num][i] = values[i];
  }
  inserts->value_num[inserts->tuple_num] = value_num;
  inserts->tuple_num++;
}
void inserts_destroy(Inserts *inserts)
{
  free(inserts->relation_name);
  inserts->relation_name = nullptr;

  for (size_t i = 0; i < inserts->tuple_num; i++) {
    for (size_t j = 0; j < inserts->value_num[i]; j++) {
      value_destroy(&inserts->values[i][j]);
    }
    inserts->value_num[i] = 0;
  }
}

void deletes_init_relation(Deletes *deletes, const char *relation_name)
{
  deletes->relation_name = strdup(relation_name);
}

void deletes_set_conditions(Deletes *deletes, Condition conditions[], size_t condition_num)
{
  assert(condition_num <= sizeof(deletes->conditions) / sizeof(deletes->conditions[0]));
  for (size_t i = 0; i < condition_num; i++) {
    deletes->conditions[i] = conditions[i];
  }
  deletes->condition_num = condition_num;
}
void deletes_destroy(Deletes *deletes)
{
  for (size_t i = 0; i < deletes->condition_num; i++) {
    condition_destroy(&deletes->conditions[i]);
  }
  deletes->condition_num = 0;
  free(deletes->relation_name);
  deletes->relation_name = nullptr;
}

void updates_init(Updates *updates, const char *relation_name, Condition conditions[], size_t condition_num)
{
  updates->relation_name = strdup(relation_name);

  assert(condition_num <= sizeof(updates->conditions) / sizeof(updates->conditions[0]));
  for (size_t i = 0; i < condition_num; i++) {
    updates->conditions[i] = conditions[i];
  }
  updates->condition_num = condition_num;
}
void updates_attr_init(Updates *updates, const char *attribute_name, Value *value)
{
  updates->attribute_names[updates->attribute_num] = strdup(attribute_name);
  updates->values[updates->attribute_num] = *value;
  updates->attribute_num++;
}

void updates_destroy(Updates *updates)
{
  free(updates->relation_name);
  for(int i=0;i<updates->attribute_num;i++) {
    free(updates->attribute_names[i]);
    updates->attribute_names[i] = nullptr;
    value_destroy(&updates->values[i]);
  }
  updates->relation_name = nullptr;

  for (size_t i = 0; i < updates->condition_num; i++) {
    condition_destroy(&updates->conditions[i]);
  }
  updates->condition_num = 0;
}

void create_table_append_attribute(CreateTable *create_table, AttrInfo *attr_info)
{
  create_table->attributes[create_table->attribute_count++] = *attr_info;
}

void create_table_init_name(CreateTable *create_table, const char *relation_name)
{
  create_table->relation_name = strdup(relation_name);
}

void create_table_destroy(CreateTable *create_table)
{
  for (size_t i = 0; i < create_table->attribute_count; i++) {
    attr_info_destroy(&create_table->attributes[i]);
  }
  create_table->attribute_count = 0;
  free(create_table->relation_name);
  create_table->relation_name = nullptr;
}

void drop_table_init(DropTable *drop_table, const char *relation_name)
{
  drop_table->relation_name = strdup(relation_name);
}

void drop_table_destroy(DropTable *drop_table)
{
  free(drop_table->relation_name);
  drop_table->relation_name = nullptr;
}

void create_index_init(CreateIndex *create_index, const char *index_name, const char *relation_name)
{
  create_index->index_name = strdup(index_name);
  create_index->relation_name = strdup(relation_name);
  // create_index->attribute_count = 0;
}

void create_index_set_unique(CreateIndex *create_index, int unique) {
  create_index->unique = unique;
}

void create_index_destroy(CreateIndex *create_index)
{
  free(create_index->index_name);
  free(create_index->relation_name);
  for (int i = 0; i < create_index->attribute_count; i++) {
    free(create_index->attribute_names[i]);
    create_index->attribute_names[i] = nullptr;
  }

  create_index->index_name = nullptr;
  create_index->relation_name = nullptr;
}

void create_index_add_attr(CreateIndex *create_index, const char *attr_name)
{
  create_index->attribute_names[create_index->attribute_count++] = strdup(attr_name);
}

void drop_index_init(DropIndex *drop_index, const char *index_name)
{
  drop_index->index_name = strdup(index_name);
}

void drop_index_destroy(DropIndex *drop_index)
{
  free((char *)drop_index->index_name);
  drop_index->index_name = nullptr;
}

void desc_table_init(DescTable *desc_table, const char *relation_name)
{
  desc_table->relation_name = strdup(relation_name);
}

void desc_table_destroy(DescTable *desc_table)
{
  free((char *)desc_table->relation_name);
  desc_table->relation_name = nullptr;
}

void show_index_init(ShowIndex *show_index, const char *relation_name)
{
  show_index->relation_name = strdup(relation_name);
}

void show_index_destroy(ShowIndex *show_index)
{
  free((char *)show_index->relation_name);
  show_index->relation_name = nullptr;
}

void load_data_init(LoadData *load_data, const char *relation_name, const char *file_name)
{
  load_data->relation_name = strdup(relation_name);

  if (file_name[0] == '\'' || file_name[0] == '\"') {
    file_name++;
  }
  char *dup_file_name = strdup(file_name);
  int len = strlen(dup_file_name);
  if (dup_file_name[len - 1] == '\'' || dup_file_name[len - 1] == '\"') {
    dup_file_name[len - 1] = 0;
  }
  load_data->file_name = dup_file_name;
}

void load_data_destroy(LoadData *load_data)
{
  free((char *)load_data->relation_name);
  free((char *)load_data->file_name);
  load_data->relation_name = nullptr;
  load_data->file_name = nullptr;
}

void query_init(Query *query)
{
  query->flag = SCF_ERROR;
  memset(&query->sstr, 0, sizeof(query->sstr));
}

Query *query_create()
{
  Query *query = (Query *)malloc(sizeof(Query));
  if (nullptr == query) {
    LOG_ERROR("Failed to alloc memroy for query. size=%ld", sizeof(Query));
    return nullptr;
  }

  query_init(query);
  return query;
}

void query_reset(Query *query)
{
  switch (query->flag) {
    case SCF_SELECT: {
      selects_destroy(&query->sstr.selection);
    } break;
    case SCF_INSERT: {
      inserts_destroy(&query->sstr.insertion);
    } break;
    case SCF_DELETE: {
      deletes_destroy(&query->sstr.deletion);
    } break;
    case SCF_UPDATE: {
      updates_destroy(&query->sstr.update);
    } break;
    case SCF_CREATE_TABLE: {
      create_table_destroy(&query->sstr.create_table);
    } break;
    case SCF_DROP_TABLE: {
      drop_table_destroy(&query->sstr.drop_table);
    } break;
    case SCF_CREATE_INDEX: {
      create_index_destroy(&query->sstr.create_index);
    } break;
    case SCF_DROP_INDEX: {
      drop_index_destroy(&query->sstr.drop_index);
    } break;
    case SCF_SYNC: {

    } break;
    case SCF_SHOW_TABLES:
      break;

    case SCF_DESC_TABLE: {
      desc_table_destroy(&query->sstr.desc_table);
    } break;
    case SCF_SHOW_INDEX: {
      show_index_destroy(&query->sstr.show_index);
    } break;

    case SCF_LOAD_DATA: {
      load_data_destroy(&query->sstr.load_data);
    } break;
    case SCF_CLOG_SYNC:
    case SCF_BEGIN:
    case SCF_COMMIT:
    case SCF_ROLLBACK:
    case SCF_HELP:
    case SCF_EXIT:
    case SCF_ERROR:
      break;
  }
}

void query_destroy(Query *query)
{
  query_reset(query);
  free(query);
}
#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

////////////////////////////////////////////////////////////////////////////////

extern "C" int sql_parse(const char *st, Query *sqls);

RC parse(const char *st, Query *sqln)
{
  sql_parse(st, sqln);

  if (sqln->flag == SCF_ERROR)
    return SQL_SYNTAX;
  else
    return SUCCESS;
}