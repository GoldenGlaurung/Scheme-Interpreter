#include <stdio.h>
#include "string.h"
#include "talloc.h"
#include "value.h"
#include "parser.h"
#include "linkedlist.h"

void checkArgs(Value *args){
    //trim the function
    args = car(args);

    //error check args for invalidness
    Value *arg = args;
    while (arg->type != NULL_TYPE){
        if (car(arg)->type != SYMBOL_TYPE){
            printf("Evaluation error: invalid variable.\n");
            texit(0);
        }
        arg = cdr(arg);
    }

    //error check args for repeated variables
    while (args->type != NULL_TYPE){
        arg = args;
        while (cdr(arg)->type != NULL_TYPE){
            arg = cdr(arg);
            //printf("Comparing: car(arg)->s: %s and car(args)->s: %s\n", car(arg)->s, car(args)->s);
            if (!strcmp(car(arg)->s, car(args)->s)){
                printf("Evaluation error: repeated variable.\n");
                texit(0);
            }
        }
        args = cdr(args);
    }
    return;
}

//checks the args, fixes up the closure, then returns that closure.
Value *lambda(Value *args, Frame *frame){
    checkArgs(args);
    Value *closure = makeNull();
    closure->type = CLOSURE_TYPE;
    closure->cl.paramNames = car(args);
    closure->cl.functionCode = car(cdr(args));
    closure->cl.frame = frame;    
    return closure;
}