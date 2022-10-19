#ifndef _TYPECAST_H_
#define _TYPECAST_H_

#include "rc.h"
#include "sql/stmt/stmt.h"

RC cast_string_to_date(Value *dest, void *data);
RC cast_string_to_int(Value *dest, void *data);
RC cast_string_to_float(Value *dest, void *data);
RC cast_int_to_string(Value *dest, void *data);
RC cast_float_to_string(Value *dest, void *data);
RC cast_int_to_float(Value *dest, void *data);
RC cast_float_to_int(Value *dest, void *data);

RC try_typecast(Value *dest, Value source, AttrType target);
RC try_typecast_bidirection(Value *dest, Value source0, Value source1, int *which_casted);

#endif