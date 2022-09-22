#include <stdio.h>
#include "value.h"
#include "eval_let.h"

//passes the inputs into addToFrame from "eval_let.c"
Value *define(Value *args, Frame *frame){
    Value *voidVal = talloc(sizeof(Value));
    voidVal->type = VOID_TYPE;
    voidVal->p = addToFrame(car(args), car(cdr(args)), frame);
    return voidVal;
}