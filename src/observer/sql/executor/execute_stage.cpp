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
// Created by Meiyi & Longda on 2021/4/13.
//

#include <string>
#include <sstream>

#include "execute_stage.h"

#include "common/io/io.h"
#include "common/log/log.h"
#include "common/lang/defer.h"
#include "common/seda/timer_stage.h"
#include "common/lang/string.h"
#include "session/session.h"
#include "event/storage_event.h"
#include "event/sql_event.h"
#include "event/session_event.h"
#include "sql/expr/tuple.h"
#include "sql/operator/table_scan_operator.h"
#include "sql/operator/index_scan_operator.h"
#include "sql/operator/predicate_operator.h"
#include "sql/operator/order_operator.h"
#include "sql/operator/update_operator.h"
#include "sql/operator/delete_operator.h"
#include "sql/operator/project_operator.h"
#include "sql/operator/hash_aggregate_operator.h"
#include "sql/operator/join_operator.h"
#include "sql/stmt/stmt.h"
#include "sql/stmt/select_stmt.h"
#include "sql/stmt/update_stmt.h"
#include "sql/stmt/delete_stmt.h"
#include "sql/stmt/insert_stmt.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/common/table.h"
#include "storage/common/field.h"
#include "storage/index/index.h"
#include "storage/default/default_handler.h"
#include "storage/common/condition_filter.h"
#include "storage/trx/trx.h"
#include "storage/clog/clog.h"

using namespace common;

//RC create_selection_executor(
//   Trx *trx, const Selects &selects, const char *db, const char *table_name, SelectExeNode &select_node);

//! Constructor
ExecuteStage::ExecuteStage(const char *tag) : Stage(tag)
{}

//! Destructor
ExecuteStage::~ExecuteStage()
{}

//! Parse properties, instantiate a stage object
Stage *ExecuteStage::make_stage(const std::string &tag)
{
  ExecuteStage *stage = new (std::nothrow) ExecuteStage(tag.c_str());
  if (stage == nullptr) {
    LOG_ERROR("new ExecuteStage failed");
    return nullptr;
  }
  stage->set_properties();
  return stage;
}

//! Set properties for this object set in stage specific properties
bool ExecuteStage::set_properties()
{
  //  std::string stageNameStr(stageName);
  //  std::map<std::string, std::string> section = theGlobalProperties()->get(
  //    stageNameStr);
  //
  //  std::map<std::string, std::string>::iterator it;
  //
  //  std::string key;

  return true;
}

//! Initialize stage params and validate outputs
bool ExecuteStage::initialize()
{
  LOG_TRACE("Enter");

  std::list<Stage *>::iterator stgp = next_stage_list_.begin();
  default_storage_stage_ = *(stgp++);
  mem_storage_stage_ = *(stgp++);

  LOG_TRACE("Exit");
  return true;
}

//! Cleanup after disconnection
void ExecuteStage::cleanup()
{
  LOG_TRACE("Enter");

  LOG_TRACE("Exit");
}

void ExecuteStage::handle_event(StageEvent *event)
{
  LOG_TRACE("Enter\n");

  handle_request(event);

  LOG_TRACE("Exit\n");
  return;
}

void ExecuteStage::callback_event(StageEvent *event, CallbackContext *context)
{
  LOG_TRACE("Enter\n");

  // here finish read all data from disk or network, but do nothing here.

  LOG_TRACE("Exit\n");
  return;
}

void ExecuteStage::handle_request(common::StageEvent *event)
{
  SQLStageEvent *sql_event = static_cast<SQLStageEvent *>(event);
  SessionEvent *session_event = sql_event->session_event();
  Stmt *stmt = sql_event->stmt();
  Session *session = session_event->session();
  Query *sql = sql_event->query();

  if (stmt != nullptr) {
    switch (stmt->type()) {
    case StmtType::SELECT: {
      do_select(sql_event);
    } break;
    case StmtType::INSERT: {
      do_insert(sql_event);
    } break;
    case StmtType::UPDATE: {
      do_update(sql_event);
    } break;
    case StmtType::DELETE: {
      do_delete(sql_event);
    } break;
    default: {
      LOG_WARN("should not happen. please implenment");
    } break;
    }
  } else {
    switch (sql->flag) {
    case SCF_HELP: {
      do_help(sql_event);
    } break;
    case SCF_CREATE_TABLE: {
      do_create_table(sql_event);
    } break;
    case SCF_CREATE_INDEX: {
      do_create_index(sql_event);
    } break;
    case SCF_SHOW_TABLES: {
      do_show_tables(sql_event);
    } break;
    case SCF_DESC_TABLE: {
      do_desc_table(sql_event);
    } break;
    case SCF_SHOW_INDEX: {
      do_show_index(sql_event);
    } break;

    case SCF_DROP_TABLE:{
      do_drop_table(sql_event);
    } break;
    case SCF_DROP_INDEX:
    case SCF_LOAD_DATA: {
      default_storage_stage_->handle_event(event);
    } break;
    case SCF_SYNC: {
      /*
      RC rc = DefaultHandler::get_default().sync();
      session_event->set_response(strrc(rc));
      */
    } break;
    case SCF_BEGIN: {
      do_begin(sql_event);
      /*
      session_event->set_response("SUCCESS\n");
      */
    } break;
    case SCF_COMMIT: {
      do_commit(sql_event);
      /*
      Trx *trx = session->current_trx();
      RC rc = trx->commit();
      session->set_trx_multi_operation_mode(false);
      session_event->set_response(strrc(rc));
      */
    } break;
    case SCF_CLOG_SYNC: {
      do_clog_sync(sql_event);
    }
    case SCF_ROLLBACK: {
      Trx *trx = session_event->get_client()->session->current_trx();
      RC rc = trx->rollback();
      session->set_trx_multi_operation_mode(false);
      session_event->set_response(strrc(rc));
    } break;
    case SCF_EXIT: {
      // do nothing
      const char *response = "Unsupported\n";
      session_event->set_response(response);
    } break;
    default: {
      LOG_ERROR("Unsupported command=%d\n", sql->flag);
    }
    }
  }
}

void end_trx_if_need(Session *session, Trx *trx, bool all_right)
{
  if (!session->is_trx_multi_operation_mode()) {
    if (all_right) {
      trx->commit();
    } else {
      trx->rollback();
    }
  }
}

void print_tuple_header(std::ostream &os, const ProjectOperator &oper)
{
  const int cell_num = oper.tuple_cell_num();
  const TupleCellSpec *cell_spec = nullptr;
  for (int i = 0; i < cell_num; i++) {
    oper.tuple_cell_spec_at(i, cell_spec);
    if (i != 0) {
      os << " | ";
    }

    if (cell_spec->alias()) {
      os << cell_spec->alias();
    }
  }

  if (cell_num > 0) {
    os << '\n';
  }
}
void tuple_to_string(std::ostream &os, const Tuple &tuple)
{
  TupleCell cell;
  RC rc = RC::SUCCESS;
  bool first_field = true;
  for (int i = 0; i < tuple.cell_num(); i++) {
    rc = tuple.cell_at(i, cell);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to fetch field of cell. index=%d, rc=%s", i, strrc(rc));
      break;
    }

    if (!first_field) {
      os << " | ";
    } else {
      first_field = false;
    }
    cell.to_string(os);
  }
}

IndexScanOperator *try_to_create_index_scan_operator(FilterStmt *filter_stmt)
{
  const std::vector<FilterUnit *> &filter_units = filter_stmt->filter_units();
  if (filter_units.empty() ) {
    return nullptr;
  }

  // 在所有过滤条件中，找到字段与值做比较的条件，然后判断字段是否可以使用索引
  // 如果是多列索引，这里的处理需要更复杂。
  // 这里的查找规则是比较简单的，就是尽量找到使用相等比较的索引
  // 如果没有就找范围比较的，但是直接排除不等比较的索引查询. (你知道为什么?)
  const Table *table = nullptr;
  std::vector<Index *> indexes;
  std::vector<const FilterUnit *>better_filters;
  Expression *left0 = filter_units[0]->left();
  Expression *right0 = filter_units[0]->right();
  if (left0->type() == ExprType::VALUE && right0->type() == ExprType::FIELD) {
    std::swap(left0, right0);
  }
  if(left0->type() == ExprType::FIELD) {
    FieldExpr &left_field_expr = *(FieldExpr *)left0;
    const Field &field = left_field_expr.field();
    // miigon: 这里偷了个懒，只考虑一个表的select，第一个查询条件所涉及表就认为是查询表
    table = field.table();
    indexes = (table->indexes()); // again, 偷懒, copy
  }
  
  const Index *use_index = nullptr;

  for(const Index *index : indexes) {
    const char **idxfields = index->index_meta().fields();
    int num_idxfields = index->index_meta().num_fields();
    int i = 0;
    for(;i<num_idxfields;i++) {
      bool match = false;
      bool range = false;
      const FilterUnit *local_betterfilter = nullptr;
      for (const FilterUnit * filter_unit : filter_units) {
        if (filter_unit->comp() == NOT_EQUAL) {
          continue;
        }

        Expression *left = filter_unit->left();
        Expression *right = filter_unit->right();
        if (left->type() == ExprType::FIELD && right->type() == ExprType::VALUE) {
        } else if (left->type() == ExprType::VALUE && right->type() == ExprType::FIELD) {
          std::swap(left, right);
        }
        FieldExpr &left_field_expr = *(FieldExpr *)left;
        const Field &field = left_field_expr.field();
        if(strcmp(field.field_name(), idxfields[i]) == 0) { // 匹配一列索引列
          match = true;
          range = (filter_unit->comp() != EQUAL_TO);
          local_betterfilter = filter_unit;
          if(!range) break;
        }
      }
      if(!match) {
        break;
      }
      better_filters.push_back(local_betterfilter);
      if(match && range) { // range can only be the last attr
        i++; // still a match;
        break;
      }
    }
    // 要求所有列全匹配，理论上也可以做最左前缀匹配，这里简化没有实现
    if(i < num_idxfields) {
      break;
    }
    // use index
    use_index = index;
  }

  if (use_index == nullptr) {
    return nullptr;
  }

  std::vector<TupleCell> left_cells;
  std::vector<TupleCell> right_cells;
  bool left_inclusive = false;
  bool right_inclusive = false;

  for(int i=0;i<better_filters.size();i++) {
    const FilterUnit *better_filter = better_filters[i];
    Expression *left = better_filter->left();
    Expression *right = better_filter->right();
    CompOp comp = better_filter->comp();
    if (left->type() == ExprType::VALUE && right->type() == ExprType::FIELD) {
      std::swap(left, right);
      switch (comp) {
      case EQUAL_TO:    { comp = EQUAL_TO; }    break;
      case LESS_EQUAL:  { comp = GREAT_THAN; }  break;
      case NOT_EQUAL:   { comp = NOT_EQUAL; }   break;
      case LESS_THAN:   { comp = GREAT_EQUAL; } break;
      case GREAT_EQUAL: { comp = LESS_THAN; }   break;
      case GREAT_THAN:  { comp = LESS_EQUAL; }  break;
      default: {
        LOG_WARN("should not happen");
      }
      }
    }

    FieldExpr &left_field_expr = *(FieldExpr *)left;
    const Field &field = left_field_expr.field();
    const Table *table = field.table();

    ValueExpr &right_value_expr = *(ValueExpr *)right;
    TupleCell value;
    right_value_expr.get_tuple_cell(value);

    switch (comp) {
    case EQUAL_TO: {
      left_cells.push_back(value);
      right_cells.push_back(value);
      left_inclusive = true;
      right_inclusive = true;
    } break;

    case LESS_EQUAL: {
      left_cells.clear();
      left_inclusive = false;
      right_cells.push_back(value);
      right_inclusive = true;
    } break;

    case LESS_THAN: {
      left_cells.clear();
      left_inclusive = false;
      right_cells.push_back(value);
      right_inclusive = false;
    } break;

    case GREAT_EQUAL: {
      left_cells.push_back(value);
      left_inclusive = true;
      right_cells.clear();
      right_inclusive = false;
    } break;

    case GREAT_THAN: {
      left_cells.push_back(value);
      left_inclusive = false;
      right_cells.clear();
      right_inclusive = false;
    } break;

    default: {
      LOG_WARN("should not happen. comp=%d", comp);
    } break;
    }
  }

  IndexScanOperator *oper = new IndexScanOperator(table, use_index,
       left_cells, left_inclusive, right_cells, right_inclusive);

  LOG_INFO("use index for scan: %s in table %s", use_index->index_meta().name(), table->name());
  return oper;
}

RC ExecuteStage::do_select(SQLStageEvent *sql_event)
{
  SelectStmt *select_stmt = (SelectStmt *)(sql_event->stmt());
  SessionEvent *session_event = sql_event->session_event();
  RC rc = RC::SUCCESS;


  std::unordered_map<std::string, Operator*> table_operator_map;
  if (select_stmt->tables().empty()) {
    LOG_WARN("invalid argument. size of tables = 0");
    return RC::INVALID_ARGUMENT;
  }
  for(int i=0; i<select_stmt->tables().size(); i++){
    FilterStmt *filter_stmt = select_stmt->filter_stmts(i);
    Table *table = select_stmt->tables()[i];
    Operator *scan_oper = try_to_create_index_scan_operator(filter_stmt);
    if (nullptr == scan_oper) {
      scan_oper = new TableScanOperator(table);
    }  
    PredicateOperator pred_oper(filter_stmt);
    pred_oper.add_child(scan_oper);
    table_operator_map[table->table_meta().name()] = &pred_oper;
  }

  // DEFER([&] () {delete scan_oper;});

  Operator *join_oper = JoinOperator::create_join_tree(table_operator_map, select_stmt->join_stmt());

  HashAggregateOperator aggregate_oper(
      select_stmt->query_fields(), select_stmt->group_keys(), select_stmt->having_stmt());
  OrderOperator order_oper = OrderOperator(select_stmt->order_fields());
  if (select_stmt->do_aggregate()) {
    aggregate_oper.add_child(join_oper);
    order_oper.add_child(&aggregate_oper);
  } else {
    order_oper.add_child(join_oper);
  }

  ProjectOperator project_oper;
  project_oper.add_child(&order_oper);

  // 初始化project_operator
  for (const Field &field : select_stmt->query_fields()) {
    project_oper.add_projection(field.table(), field.meta());
  }
  rc = project_oper.open();
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open operator");
    session_event->set_response("FAILURE\n");
    return rc;
  }

  // 开始执行
  std::stringstream ss;
  print_tuple_header(ss, project_oper);
  while ((rc = project_oper.next()) == RC::SUCCESS) {
    // get current record
    // write to response
    Tuple * tuple = project_oper.current_tuple();
    if (nullptr == tuple) {
      rc = RC::INTERNAL;
      LOG_WARN("failed to get current record. rc=%s", strrc(rc));
      break;
    }

    tuple_to_string(ss, *tuple);
    ss << std::endl;
  }

  if (rc != RC::RECORD_EOF) {
    LOG_WARN("something wrong while iterate operator. rc=%s", strrc(rc));
    session_event->set_response("FAILED\n");
    project_oper.close();
  } else {
    rc = project_oper.close();
    session_event->set_response(ss.str());
  }
  return rc;
}

RC ExecuteStage::do_help(SQLStageEvent *sql_event)
{
  SessionEvent *session_event = sql_event->session_event();
  const char *response = "show tables;\n"
                         "desc `table name`;\n"
                         "create table `table name` (`column name` `column type`, ...);\n"
                         "create index `index name` on `table` (`column`);\n"
                         "insert into `table` values(`value1`,`value2`);\n"
                         "update `table` set column=value [where `column`=`value`];\n"
                         "delete from `table` [where `column`=`value`];\n"
                         "select [ * | `columns` ] from `table`;\n";
  session_event->set_response(response);
  return RC::SUCCESS;
}

RC ExecuteStage::do_create_table(SQLStageEvent *sql_event)
{
  const CreateTable &create_table = sql_event->query()->sstr.create_table;
  SessionEvent *session_event = sql_event->session_event();
  Db *db = session_event->session()->get_current_db();
  RC rc = db->create_table(create_table.relation_name,
			create_table.attribute_count, create_table.attributes);
  if (rc == RC::SUCCESS) {
    session_event->set_response("SUCCESS\n");
  } else {
    session_event->set_response("FAILURE\n");
  }
  return rc;
}
RC ExecuteStage::do_drop_table(SQLStageEvent *sql_event)
{
  const DropTable &drop_table = sql_event->query()->sstr.drop_table;
  SessionEvent *session_event = sql_event->session_event();
  Db *db = session_event->session()->get_current_db();
  RC rc = db->drop_table(drop_table.relation_name);
  if (rc == RC::SUCCESS) {
    session_event->set_response("SUCCESS\n");
  } else {
    session_event->set_response("FAILURE\n");
  }
  return rc;
}
RC ExecuteStage::do_create_index(SQLStageEvent *sql_event)
{
  SessionEvent *session_event = sql_event->session_event();
  Db *db = session_event->session()->get_current_db();
  const CreateIndex &create_index = sql_event->query()->sstr.create_index;
  // due to the way the sql is parsed, the order of attributes are reversed
  // this fixes that, in a very dirty way..... normally i woundn't do this.
  char *attribute_names[MAX_NUM];
  for(int i=0;i<create_index.attribute_count;i++) {
    attribute_names[i] = create_index.attribute_names[create_index.attribute_count-i-1];
  }
  Table *table = db->find_table(create_index.relation_name);
  if (nullptr == table) {
    session_event->set_response("FAILURE\n");
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  RC rc = table->create_index(
    nullptr,
    create_index.index_name,
    const_cast<const char**>(attribute_names),
    create_index.attribute_count,
    create_index.unique);
  sql_event->session_event()->set_response(rc == RC::SUCCESS ? "SUCCESS\n" : "FAILURE\n");
  return rc;
}

RC ExecuteStage::do_show_tables(SQLStageEvent *sql_event)
{
  SessionEvent *session_event = sql_event->session_event();
  Db *db = session_event->session()->get_current_db();
  std::vector<std::string> all_tables;
  db->all_tables(all_tables);
  if (all_tables.empty()) {
    session_event->set_response("No table\n");
  } else {
    std::stringstream ss;
    for (const auto &table : all_tables) {
      ss << table << std::endl;
    }
    session_event->set_response(ss.str().c_str());
  }
  return RC::SUCCESS;
}


RC ExecuteStage::do_desc_table(SQLStageEvent *sql_event)
{
  Query *query = sql_event->query();
  Db *db = sql_event->session_event()->session()->get_current_db();
  const char *table_name = query->sstr.desc_table.relation_name;
  Table *table = db->find_table(table_name);
  std::stringstream ss;
  if (table != nullptr) {
    table->table_meta().desc(ss);
  } else {
    ss << "No such table: " << table_name << std::endl;
  }
  sql_event->session_event()->set_response(ss.str().c_str());
  return RC::SUCCESS;
}

RC ExecuteStage::do_show_index(SQLStageEvent *sql_event)
{
  Query *query = sql_event->query();
  Db *db = sql_event->session_event()->session()->get_current_db();
  const char *table_name = query->sstr.desc_table.relation_name;
  Table *table = db->find_table(table_name);
  if (table != nullptr) {
    std::stringstream ss;
    table->table_meta().desc_index(ss);
    sql_event->session_event()->set_response(ss.str().c_str());
    return RC::SUCCESS;
  } else {
    sql_event->session_event()->set_response("FAILURE\n");
    return RC::NOTFOUND;
  }
}

RC ExecuteStage::do_insert(SQLStageEvent *sql_event)
{
  Stmt *stmt = sql_event->stmt();
  SessionEvent *session_event = sql_event->session_event();
  Session *session = session_event->session();
  Db *db = session->get_current_db();
  Trx *trx = session->current_trx();
  CLogManager *clog_manager = db->get_clog_manager();

  if (stmt == nullptr) {
    LOG_WARN("cannot find statement");
    return RC::GENERIC_ERROR;
  }

  InsertStmt *insert_stmt = (InsertStmt *)stmt;
  Table *table = insert_stmt->table();
  const Value **tuple_values = insert_stmt->tuple_values();
  int tuple_amount = insert_stmt->tuple_amount();
  const int *value_amounts = insert_stmt->value_amounts();
  RC rc;
  for(int i=0;i<tuple_amount;i++) {
    rc = table->insert_record(trx, value_amounts[i], tuple_values[i]);
    if(rc != RC::SUCCESS) {
      session_event->set_response("FAILURE\n");
      return rc;
    }
  }
  if (rc == RC::SUCCESS) {
    if (!session->is_trx_multi_operation_mode()) {
      CLogRecord *clog_record = nullptr;
      rc = clog_manager->clog_gen_record(CLogType::REDO_MTR_COMMIT, trx->get_current_id(), clog_record);
      if (rc != RC::SUCCESS || clog_record == nullptr) {
        session_event->set_response("FAILURE\n");
        return rc;
      }

      rc = clog_manager->clog_append_record(clog_record);
      if (rc != RC::SUCCESS) {
        session_event->set_response("FAILURE\n");
        return rc;
      } 

      trx->next_current_id();
      session_event->set_response("SUCCESS\n");
    } else {
      session_event->set_response("SUCCESS\n");
    }
  } else {
    session_event->set_response("FAILURE\n");
  }
  return rc;
}

RC ExecuteStage::do_update(SQLStageEvent *sql_event)
{
  Stmt *stmt = sql_event->stmt();
  SessionEvent *session_event = sql_event->session_event();
  Session *session = session_event->session();
  Db *db = session->get_current_db();
  Trx *trx = session->current_trx();
  CLogManager *clog_manager = db->get_clog_manager();

  if (stmt == nullptr) {
    LOG_WARN("cannot find statement");
    return RC::GENERIC_ERROR;
  }

  UpdateStmt *update_stmt = (UpdateStmt *)stmt;
  
  Operator *scan_oper = try_to_create_index_scan_operator(update_stmt->filter_stmt());
  if (nullptr == scan_oper) {
    scan_oper = new TableScanOperator(update_stmt->table());
  }

  PredicateOperator pred_oper(update_stmt->filter_stmt());
  pred_oper.add_child(scan_oper);
  UpdateOperator update_oper(update_stmt, trx);
  update_oper.add_child(&pred_oper);

  RC rc = update_oper.open();
  if (rc != RC::SUCCESS) {
    session_event->set_response("FAILURE\n");
  } else {
    session_event->set_response("SUCCESS\n");
    if (!session->is_trx_multi_operation_mode()) {
      CLogRecord *clog_record = nullptr;
      rc = clog_manager->clog_gen_record(CLogType::REDO_MTR_COMMIT, trx->get_current_id(), clog_record);
      if (rc != RC::SUCCESS || clog_record == nullptr) {
        session_event->set_response("FAILURE\n");
        return rc;
      }

      rc = clog_manager->clog_append_record(clog_record);
      if (rc != RC::SUCCESS) {
        session_event->set_response("FAILURE\n");
        return rc;
      } 

      trx->next_current_id();
      session_event->set_response("SUCCESS\n");
    }
  }
  return rc;
}


RC ExecuteStage::do_delete(SQLStageEvent *sql_event)
{
  Stmt *stmt = sql_event->stmt();
  SessionEvent *session_event = sql_event->session_event();
  Session *session = session_event->session();
  Db *db = session->get_current_db();
  Trx *trx = session->current_trx();
  CLogManager *clog_manager = db->get_clog_manager();

  if (stmt == nullptr) {
    LOG_WARN("cannot find statement");
    return RC::GENERIC_ERROR;
  }

  DeleteStmt *delete_stmt = (DeleteStmt *)stmt;
  TableScanOperator scan_oper(delete_stmt->table());
  PredicateOperator pred_oper(delete_stmt->filter_stmt());
  pred_oper.add_child(&scan_oper);
  DeleteOperator delete_oper(delete_stmt, trx);
  delete_oper.add_child(&pred_oper);

  RC rc = delete_oper.open();
  if (rc != RC::SUCCESS) {
    session_event->set_response("FAILURE\n");
  } else {
    session_event->set_response("SUCCESS\n");
    if (!session->is_trx_multi_operation_mode()) {
      CLogRecord *clog_record = nullptr;
      rc = clog_manager->clog_gen_record(CLogType::REDO_MTR_COMMIT, trx->get_current_id(), clog_record);
      if (rc != RC::SUCCESS || clog_record == nullptr) {
        session_event->set_response("FAILURE\n");
        return rc;
      }

      rc = clog_manager->clog_append_record(clog_record);
      if (rc != RC::SUCCESS) {
        session_event->set_response("FAILURE\n");
        return rc;
      } 

      trx->next_current_id();
      session_event->set_response("SUCCESS\n");
    }
  }
  return rc;
}

RC ExecuteStage::do_begin(SQLStageEvent *sql_event)
{
  RC rc = RC::SUCCESS;
  SessionEvent *session_event = sql_event->session_event();
  Session *session = session_event->session();
  Db *db = session->get_current_db();
  Trx *trx = session->current_trx();
  CLogManager *clog_manager = db->get_clog_manager();

  session->set_trx_multi_operation_mode(true);

  CLogRecord *clog_record = nullptr;
  rc = clog_manager->clog_gen_record(CLogType::REDO_MTR_BEGIN, trx->get_current_id(), clog_record);
  if (rc != RC::SUCCESS || clog_record == nullptr) {
    session_event->set_response("FAILURE\n");
    return rc;
  }

  rc = clog_manager->clog_append_record(clog_record);
  if (rc != RC::SUCCESS) {
    session_event->set_response("FAILURE\n");
  } else {
    session_event->set_response("SUCCESS\n");
  }

  return rc;
}

RC ExecuteStage::do_commit(SQLStageEvent *sql_event)
{
  RC rc = RC::SUCCESS;
  SessionEvent *session_event = sql_event->session_event();
  Session *session = session_event->session();
  Db *db = session->get_current_db();
  Trx *trx = session->current_trx();
  CLogManager *clog_manager = db->get_clog_manager();

  session->set_trx_multi_operation_mode(false);

  CLogRecord *clog_record = nullptr;
  rc = clog_manager->clog_gen_record(CLogType::REDO_MTR_COMMIT, trx->get_current_id(), clog_record);
  if (rc != RC::SUCCESS || clog_record == nullptr) {
    session_event->set_response("FAILURE\n");
    return rc;
  }

  rc = clog_manager->clog_append_record(clog_record);
  if (rc != RC::SUCCESS) {
    session_event->set_response("FAILURE\n");
  } else {
    session_event->set_response("SUCCESS\n");
  }

  trx->next_current_id();

  return rc;
}

RC ExecuteStage::do_clog_sync(SQLStageEvent *sql_event)
{
  RC rc = RC::SUCCESS;
  SessionEvent *session_event = sql_event->session_event();
  Db *db = session_event->session()->get_current_db();
  CLogManager *clog_manager = db->get_clog_manager();

  rc = clog_manager->clog_sync();
  if (rc != RC::SUCCESS) {
    session_event->set_response("FAILURE\n");
  } else {
    session_event->set_response("SUCCESS\n");
  }

  return rc;
}
