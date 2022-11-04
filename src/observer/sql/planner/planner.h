#include "sql/stmt/stmt.h"
#include "sql/stmt/select_stmt.h"
#include "sql/operator/table_scan_operator.h"
#include "sql/operator/index_scan_operator.h"
#include "sql/operator/predicate_operator.h"
#include "sql/operator/update_operator.h"
#include "sql/operator/delete_operator.h"
#include "sql/operator/project_operator.h"
#include "sql/operator/hash_aggregate_operator.h"
#include "sql/operator/order_operator.h"
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

class Planner {
public:
  Planner() = default;
  Planner(Stmt *stmt) : stmt_(stmt)
  {}
  ~Planner() = default;

  void init(Stmt *stmt)
  {
    stmt_ = stmt;
  }

  RC create_executor(Operator *&root);

  RC destroy_executor(Operator *&root);

  Operator *get_executor()
  {
    return root_;
  }

  Stmt *get_stmt()
  {
    return stmt_;
  }

private:
  RC create_select_plan(SelectStmt *select_stmt, Operator *&root);
  Stmt *stmt_ = nullptr;
  Operator *root_ = nullptr;
};