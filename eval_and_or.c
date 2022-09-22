#include <stdlib.h>
#include <stdio.h>
#include "value.h"
#include "linkedlist.h"
#include "interpreter.h"

//left to right, return false on first non-truthy statement
Value *eval_and(Value *args, Frame *frame){
    Value *result = makeNull();
    result->type = BOOL_TYPE;
    while (args->type == CONS_TYPE) {
        Value *evaluated = eval(car(args), frame);
        if (evaluated->type == BOOL_TYPE && evaluated->i == 0){
            result->i = 0;
            return result;
        }
        args = cdr(args);
    }
    result->i = 1;
    return result;
}   

//also left to right, return true on first truthy statement
Value *eval_or(Value *args, Frame *frame){
    Value *result = makeNull();
    result->type = BOOL_TYPE;
    while (args->type == CONS_TYPE) {
        Value *evaluated = eval(car(args), frame);
        if (!(evaluated->type == BOOL_TYPE && evaluated->i == 0)){
            result->i = 1;
            return result;
        }
        args = cdr(args);
    }
    result->i = 0;
    return result;
}