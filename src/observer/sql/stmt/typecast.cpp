#include <stdio.h>
#include <stdlib.h>
#include "sql/stmt/typecast.h"

RC cast_string_to_date(Value *dest, void *data)
{
  struct tm t;
  time_t t_of_day;
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

RC try_typecast(Value *dest, Value source, AttrType target) {
  if(source.type == CHARS && target == DATES) {
	cast_string_to_date(dest, source.data);
  } else {
	return RC::MISMATCH;
  }
}