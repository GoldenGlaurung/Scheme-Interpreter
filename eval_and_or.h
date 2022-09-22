#include "value.h"

#ifndef _EVALANDOR
#define _EVALANDOR

Value *eval_and(Value *args, Frame *frame);

Value *eval_or(Value *args, Frame *frame);

#endif