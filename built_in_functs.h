#include "value.h"

#ifndef _BUILT_IN_FUNCTS
#define _BUILT_IN_FUNCTS

Value *builtInCar(Value *args);

Value *builtInCdr(Value *args);

Value *builtInCons(Value *args);

Value *builtInPlus(Value *args);

Value *builtInMinus(Value *args);

Value *builtInMultiply(Value *args);

Value *builtInDivide(Value *args);

Value *builtInModulo(Value *args);

Value *builtInLessThan(Value *args);

Value *builtInMoreThan(Value *args);

Value *builtInEquals(Value *args);

Value *builtInNull(Value *args);

#endif