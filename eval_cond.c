#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "value.h"
#include "parser.h"
#include "linkedlist.h"
#include "interpreter.h"

Value *eval_cond(Value *args, Frame *frame){
    Value *temp = makeNull();
    while (args->type == CONS_TYPE && car(args)->type == CONS_TYPE){
        temp = car(car(args));
        if (temp->type == SYMBOL_TYPE && !strcmp(temp->s, "else")){
            return eval(car(cdr(car(args))), frame);
        } else {
            temp = eval(temp, frame); 
            if (!(temp->type == BOOL_TYPE && temp->i == 0)){
                return eval(car(cdr(car(args))), frame);
            }
        }
        args = cdr(args);
    }
    temp->type = VOID_TYPE;
    return temp;
}

