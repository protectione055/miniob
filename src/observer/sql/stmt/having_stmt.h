#pragma once

#include <algorithm>
#include "rc.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/common/table.h"

class FilterUnit;

class HavingStmt {
public:
  HavingStmt() = default;
  virtual ~HavingStmt();

public:
  const std::vector<FilterUnit *> &filter_units() const
  {
    return filter_units_;
  }

  static RC create_having_stmt(const std::vector<Field> &query_fields, const std::vector<Field> &group_by_keys,
      const Selects *select_sql, HavingStmt *&stmt);

  static RC create_having_filter_unit(const std::vector<Field> &query_fields, const std::vector<Field> &group_by_keys,
      const Condition &having_condition, FilterUnit *&filter_unit, HavingStmt *stmt);

public:
private:
  std::vector<FilterUnit *> filter_units_;  // 默认当前都是AND关系
};

static const Field *find_table_field_for_having(
    const std::vector<Field> &query_fields, const std::vector<Field> &group_by_keys, const RelAttr &attr);
static bool is_field_and_attr_correspond(const Field &field, const RelAttr &attr);