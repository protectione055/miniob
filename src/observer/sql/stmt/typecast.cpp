#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "sql/stmt/typecast.h"
#include "storage/common/table.h"
#include "common/log/log.h"

RC cast_string_to_date(Value *dest, void *data)
{
  struct tm t;
  int year, mon, day;
  if(sscanf((char*)data, "%d-%d-%d", &year, &mon, &day) < 3) {
    return RC::MISMATCH;
  }
  year-=1900;
  mon--; // january is 0
  t.tm_mday = day;
  t.tm_mon = mon;
  t.tm_year = year;
  t.tm_hour = 0;
  t.tm_min = 0;
  t.tm_sec = 0;
  t.tm_isdst = 0;
  time_t res = timegm(&t);
  if (res == -1 || (day != t.tm_mday) || (mon != t.tm_mon) || (year != t.tm_year)) {
    return RC::MISMATCH;
  }
  dest->type = DATES;
  dest->data = malloc(sizeof(time_t));
  (*(time_t*)dest->data) = res;
  return RC::SUCCESS;
}

RC cast_string_to_int(Value *dest, void *data)
{
  int res;
  if(sscanf((char*)data, "%d", &res) == 0) {
    res = 0;
  }
  dest->type = INTS;
  dest->data = malloc(sizeof(int));
  (*(int*)dest->data) = res;
  return RC::SUCCESS;
}

RC cast_string_to_float(Value *dest, void *data)
{
  float res;
  if(sscanf((char*)data, "%f", &res) == 0) {
    res = 0;
  }
  dest->type = FLOATS;
  dest->data = malloc(sizeof(float));
  (*(float*)dest->data) = res;
  return RC::SUCCESS;
}

RC cast_int_to_string(Value *dest, void *data)
{
  char *res = (char*)malloc(32);
  sprintf(res, "%d", *(int*)data);
  dest->type = CHARS;
  dest->data = res;
  return RC::SUCCESS;
}

RC cast_float_to_string(Value *dest, void *data)
{
  char *res = (char*)malloc(32);
  sprintf(res, "%g", *(float*)data);
  dest->type = CHARS;
  dest->data = res;
  return RC::SUCCESS;
}

RC cast_int_to_float(Value *dest, void *data)
{
  dest->type = FLOATS;
  dest->data = malloc(sizeof(float));
  (*(float*)dest->data) = *(int*)data;
  return RC::SUCCESS;
}

RC cast_float_to_int(Value *dest, void *data)
{
  dest->type = INTS;
  dest->data = malloc(sizeof(int));
  (*(int*)dest->data) = lround(*(float*)data);
  return RC::SUCCESS;
}


RC try_typecast(Value *dest, Value source, AttrType target) {
  if(source.type == CHARS && target == DATES) {
    return cast_string_to_date(dest, source.data);
  } else if(source.type == CHARS && target == INTS) {
    return cast_string_to_int(dest, source.data);
  } else if(source.type == CHARS && target == FLOATS) {
    return cast_string_to_float(dest, source.data);
  } else if(source.type == INTS && target == CHARS) {
    return cast_int_to_string(dest, source.data);
  } else if(source.type == FLOATS && target == CHARS) {
    return cast_float_to_string(dest, source.data);
  } else if(source.type == INTS && target == FLOATS) {
    return cast_int_to_float(dest, source.data);
  } else if(source.type == FLOATS && target == INTS) {
    return cast_float_to_int(dest, source.data);
  } else {
    return RC::MISMATCH;
  }
}

RC try_typecast_bidirection(Value *dest, Value source0, Value source1, int *which_casted) {
  RC rc = RC::SUCCESS;
  if(source0.type == INTS && source1.type == FLOATS) {
    rc = cast_int_to_float(dest, source0.data);
    *which_casted = 0;
  } else if(source0.type == FLOATS && source1.type == INTS) {
    rc = cast_int_to_float(dest, source1.data);
    *which_casted = 1;
  } else if(source0.type == CHARS && source1.type == FLOATS) {
    rc = cast_string_to_float(dest, source0.data);
    *which_casted = 0;
  } else if(source0.type == FLOATS && source1.type == CHARS) {
    rc = cast_string_to_float(dest, source1.data);
    *which_casted = 1;
  } else {
    rc = try_typecast(dest, source0, source1.type);
    if(rc == RC::SUCCESS) {
      *which_casted = 0;
      goto done;
    }
    rc = try_typecast(dest, source1, source0.type);
    if(rc == RC::SUCCESS) {
      *which_casted = 1;
      goto done;
    }
    rc = RC::MISMATCH;
  }
  done:
  return rc;
}

// this function really shouldn't be here, but again, the codebase is a hot mess already...
int get_length_from_value(const Value &v) {
  if(v.type == FLOATS) return sizeof(float);
  else if(v.type == INTS) return sizeof(int);
  else if(v.type == CHARS) return strlen((char*)v.data);
  else if(v.type == DATES) return sizeof(time_t);
  // really shouldn't reach here
  LOG_ERROR("get_length_from_value: unrecognized type %d", v.type);
  return -1;
}