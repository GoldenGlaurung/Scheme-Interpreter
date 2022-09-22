#include <stdlib.h>
#include <stdio.h>
#include "linkedlist.h"
#include "value.h"
#include "interpreter.h"
#include "look_up_symbol.h"

//because truth is... odd in Scheme (and only #f is false)
//program just checks for that--anything else is true.
Value *evalIf(Value *args, Frame *frame){
    //printTree(frame->parent->parent->bindings);
    Value *result = eval(car(args), frame);
    if (result->type == BOOL_TYPE && result->i == 0){
        return car(cdr(cdr(args)));
    } else {
        return car(cdr(args));
    }
}