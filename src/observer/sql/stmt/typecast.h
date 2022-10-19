#ifndef _TYPECAST_H_
#define _TYPECAST_H_

#include "rc.h"
#include "sql/stmt/stmt.h"

RC cast_string_to_date(Value *dest, void *data);
RC try_typecast(Value *dest, Value source, AttrType target);

#endif