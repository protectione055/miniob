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

#ifndef __OBSERVER_SQL_PARSER_PARSE_DEFS_H__
#define __OBSERVER_SQL_PARSER_PARSE_DEFS_H__

#include <stddef.h>
#include <time.h>

#define MAX_NUM 20
#define MAX_REL_NAME 20
#define MAX_ATTR_NAME 20
#define MAX_ERROR_MESSAGE 20
#define MAX_DATA 50

//聚合操作类型
typedef enum {
  NOT_AGGR,  //非聚合
  MIN,       //最小值
  MAX,       //最大值
  SUM,       //求和
  COUNT,     //计数
  AVG		 //求平均
} AggrType;

//属性结构体
typedef struct {
  char *relation_name;   // relation name (may be NULL) 表名
  char *attribute_name;  // attribute name              属性名
  AggrType aggr_type;    // aggregate type				聚合类型
} RelAttr;

typedef struct {
  char *relation_name;   // relation name (may be NULL) 表名
  char *attribute_name;  // attribute name              属性名
  int   is_asc;          // is ascending order          是否升序
} OrderAttr;             

typedef enum {
  EQUAL_TO,     //"="     0
  LESS_EQUAL,   //"<="    1
  NOT_EQUAL,    //"<>"    2
  LESS_THAN,    //"<"     3
  GREAT_EQUAL,  //">="    4
  GREAT_THAN,   //">"     5
  LIKE,         //"like"  6
  NOT_LIKE,     //"not like"  7
  NO_OP
} CompOp;

//属性值类型
typedef enum {
  UNDEFINED,
  CHARS,
  INTS,
  FLOATS,
  DATES,
} AttrType;

//属性值
typedef struct _Value {
  AttrType type;  // type of value
  void *data;     // value
} Value;

typedef struct _Condition {
  int left_is_attr;    // TRUE if left-hand side is an attribute
                       // 1时，操作符左边是属性名，0时，是属性值
  Value left_value;    // left-hand side value if left_is_attr = FALSE
  RelAttr left_attr;   // left-hand side attribute
  CompOp comp;         // comparison operator
  int right_is_attr;   // TRUE if right-hand side is an attribute
                       // 1时，操作符右边是属性名，0时，是属性值
  RelAttr right_attr;  // right-hand side attribute if right_is_attr = TRUE 右边的属性
  Value right_value;   // right-hand side value if right_is_attr = FALSE
} Condition;

// struct of select
typedef struct {
  int is_aggr;                    // flag of select type, 1 if it will do aggregation
  size_t attr_num;                // Length of attrs in Select clause
  RelAttr attributes[MAX_NUM];    // attrs in Select clause
  size_t relation_num;            // Length of relations in From clause
  char *relations[MAX_NUM];       // relations in From clause
  size_t join_cond_num;           // Length of join conditions in Fro clause
  Condition join_conds[MAX_NUM];  // join conditions in Fro clause
  size_t condition_num;           // Length of conditions in Where clause
  Condition conditions[MAX_NUM];  // conditions in Where clause
  size_t order_num;               // Length of attrs in Order By clause
  OrderAttr orders[MAX_NUM];      // attrs in Order By clause
  size_t group_by_key_num;        // Length of relations in group by clause
  RelAttr group_by_keys[MAX_NUM];            // attr in group by clause
  Condition having_conditions[MAX_NUM];  // having clause for aggregation
  size_t having_condition_num;           // Length of conditions in having clause
} Selects;

// struct of insert
typedef struct {
  char *relation_name;    // Relation to insert into
  size_t tuple_num;
  size_t value_num[MAX_NUM];       // Length of values
  Value values[MAX_NUM][MAX_NUM];  // values to insert
} Inserts;

// struct of delete
typedef struct {
  char *relation_name;            // Relation to delete from
  size_t condition_num;           // Length of conditions in Where clause
  Condition conditions[MAX_NUM];  // conditions in Where clause
} Deletes;

// struct of update
typedef struct {
  char *relation_name;            // Relation to update
  size_t attribute_num;           // Number of attributes to update
  char *attribute_names[MAX_NUM]; // Attribute to update
  Value values[MAX_NUM];          // update value
  size_t condition_num;           // Length of conditions in Where clause
  Condition conditions[MAX_NUM];  // conditions in Where clause
} Updates;

typedef struct {
  char *name;     // Attribute name
  AttrType type;  // Type of attribute
  size_t length;  // Length of attribute
} AttrInfo;

// struct of craete_table
typedef struct {
  char *relation_name;           // Relation name
  size_t attribute_count;        // Length of attribute
  AttrInfo attributes[MAX_NUM];  // attributes
} CreateTable;

// struct of drop_table
typedef struct {
  char *relation_name;  // Relation name
} DropTable;

// struct of create_index
typedef struct {
  char *index_name;     // Index name
  char *relation_name;  // Relation name
  size_t attribute_count;
  char *attribute_names[MAX_NUM];  // Attribute name
  int unique; // is unique
} CreateIndex;

// struct of  drop_index
typedef struct {
  const char *index_name;  // Index name
} DropIndex;

typedef struct {
  const char *relation_name;
} DescTable;

typedef struct {
  const char *relation_name;
} ShowIndex;

typedef struct {
  const char *relation_name;
  const char *file_name;
} LoadData;

union Queries {
  Selects selection;
  Inserts insertion;
  Deletes deletion;
  Updates update;
  CreateTable create_table;
  DropTable drop_table;
  CreateIndex create_index;
  DropIndex drop_index;
  DescTable desc_table;
  ShowIndex show_index;
  LoadData load_data;
  char *errors;
};

// 修改yacc中相关数字编码为宏定义
enum SqlCommandFlag {
  SCF_ERROR = 0,
  SCF_SELECT,
  SCF_INSERT,
  SCF_UPDATE,
  SCF_DELETE,
  SCF_CREATE_TABLE,
  SCF_DROP_TABLE,
  SCF_CREATE_INDEX,
  SCF_DROP_INDEX,
  SCF_SYNC,
  SCF_SHOW_TABLES,
  SCF_DESC_TABLE,
  SCF_SHOW_INDEX,
  SCF_BEGIN,
  SCF_COMMIT,
  SCF_CLOG_SYNC,
  SCF_ROLLBACK,
  SCF_LOAD_DATA,
  SCF_HELP,
  SCF_EXIT
};
// struct of flag and sql_struct
typedef struct Query {
  enum SqlCommandFlag flag;
  union Queries sstr;
} Query;

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

void relation_attr_init(RelAttr *relation_attr, const char *relation_name, const char *attribute_name);
void relation_attr_destroy(RelAttr *relation_attr);

void order_attr_init(OrderAttr *order_attr, const char *relation_name, const char *attribute_name, const int is_asc);
void order_attr_destroy(OrderAttr *order_attr);

void value_init_integer(Value *value, int v);
void value_init_float(Value *value, float v);
void value_init_string(Value *value, const char *v);
void value_init_date(Value *value, time_t v);
void value_destroy(Value *value);

void condition_init(Condition *condition, CompOp comp, int left_is_attr, RelAttr *left_attr, Value *left_value,
    int right_is_attr, RelAttr *right_attr, Value *right_value);
void condition_destroy(Condition *condition);

void attr_info_init(AttrInfo *attr_info, const char *name, AttrType type, size_t length);
void attr_info_destroy(AttrInfo *attr_info);

void selects_init(Selects *selects, ...);
void selects_append_attribute(Selects *selects, RelAttr *rel_attr);
void selects_append_relation(Selects *selects, const char *relation_name);
void selects_append_joincond(Selects *selects, Condition condition);
void selects_append_conditions(Selects *selects, Condition conditions[], size_t condition_num);
void selects_append_orders(Selects *selects, OrderAttr *order_attr);
void selects_destroy(Selects *selects);

void inserts_init(Inserts *inserts, const char *relation_name);
void inserts_create_tuple(Inserts *inserts, Value values[], size_t value_num);
void inserts_destroy(Inserts *inserts);

void deletes_init_relation(Deletes *deletes, const char *relation_name);
void deletes_set_conditions(Deletes *deletes, Condition conditions[], size_t condition_num);
void deletes_destroy(Deletes *deletes);

void updates_init(Updates *updates, const char *relation_name, Condition conditions[], size_t condition_num);
void updates_attr_init(Updates *updates, const char *attribute_name, Value *value);
void updates_destroy(Updates *updates);

void create_table_append_attribute(CreateTable *create_table, AttrInfo *attr_info);
void create_table_init_name(CreateTable *create_table, const char *relation_name);
void create_table_destroy(CreateTable *create_table);

void drop_table_init(DropTable *drop_table, const char *relation_name);
void drop_table_destroy(DropTable *drop_table);

void create_index_init(CreateIndex *create_index, const char *index_name, const char *relation_name);
void create_index_destroy(CreateIndex *create_index);
void create_index_add_attr(CreateIndex *create_index, const char *attr_name);
void create_index_set_unique(CreateIndex *create_index, int unique);

void drop_index_init(DropIndex *drop_index, const char *index_name);
void drop_index_destroy(DropIndex *drop_index);

void desc_table_init(DescTable *desc_table, const char *relation_name);
void desc_table_destroy(DescTable *desc_table);

void show_index_init(ShowIndex *show_index, const char *relation_name);
void show_index_destroy(ShowIndex *show_index);

void load_data_init(LoadData *load_data, const char *relation_name, const char *file_name);
void load_data_destroy(LoadData *load_data);

void query_init(Query *query);
Query *query_create();  // create and init
void query_reset(Query *query);
void query_destroy(Query *query);  // reset and delete

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // __OBSERVER_SQL_PARSER_PARSE_DEFS_H__
