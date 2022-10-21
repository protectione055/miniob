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
// Created by WangYunlai on 2022/07/05.
//

#include "sql/expr/tuple_cell.h"
#include "storage/common/field.h"
#include "common/log/log.h"
#include "util/comparator.h"
#include "util/util.h"
#include <time.h>
#include "sql/stmt/typecast.h"

void TupleCell::to_string(std::ostream &os) const
{
  switch (attr_type_) {
  case INTS: {
    os << *(int *)data_;
  } break;
  case FLOATS: {
    float v = *(float *)data_;
    os << double2string(v);
  } break;
  case CHARS: {
    for (int i = 0; i < length_; i++) {
      if (data_[i] == '\0') {
        break;
      }
      os << data_[i];
    }
  } break;
  case DATES: {
    os << date2string(*(time_t *)data_);
  } break;
  default: {
    LOG_WARN("unsupported attr type: %d", attr_type_);
  } break;
  }
}

int TupleCell::compare(const TupleCell &other) const
{
  RC rc = RC::SUCCESS;
  Value vleft{this->attr_type_, this->data_};
  Value vright{other.attr_type_, other.data_};
  if (vleft.type != vright.type) {
    rc = try_typecast_matchtype(&vleft, &vright);
    if(rc != RC::SUCCESS) {
      // don't really NEED to be an error, just it's easier to see when debugging.
      LOG_ERROR("failed casting between %d and %d.", vleft.type, vright.type);
      return -1;
    }
  }

  switch (vleft.type) {
    case INTS: return compare_int(vleft.data, vright.data);
    case FLOATS: return compare_float(vleft.data, vright.data);
    case CHARS: return compare_string(vleft.data, strlen((char*)vleft.data), vright.data, strlen((char*)vright.data));
    case DATES: return compare_time(vleft.data, vright.data);
    default: {
      LOG_WARN("unsupported type: %d", this->attr_type_);
    }
  }

  LOG_WARN("not supported");
  return -1; // TODO return rc?
}

//字符串模糊匹配
bool TupleCell::like(const TupleCell &other) const
{
  if (this->attr_type_ == CHARS && other.attr_type_ == CHARS) {
    int last[2] = {-1, -1}; //记录上一次匹配到%的位置
    int sPos = 0, pPos = 0; //记录当前匹配到的位置
    const char *str = (const char *)this->data_;
    const char *pattern = (const char *)other.data_;

	  while (sPos < strlen(str))
	  {
		  if (pPos < strlen(pattern) && (pattern[pPos] == str[sPos]) ||
            pattern[pPos] == '_' || pattern[pPos] == '%')
		  {
		  	if (pattern[pPos] == '%')
		  	{
		  		last[0] = pPos;
		  		last[1] = sPos;
		  	}else{
		  		sPos++;
		  	}
		  	pPos++;
		  }else if (last[0] != -1){
		  	pPos = last[0] + 1;
		  	sPos = last[1] + 1;
		  	last[1]++;
		  }
		  else
		  	return false;
	  }

	  while (pPos < strlen(pattern) && pattern[pPos] == '%')
		  pPos++;

	  return (pPos == strlen(pattern));
  }
  LOG_WARN("not supported");
  return false;
}
