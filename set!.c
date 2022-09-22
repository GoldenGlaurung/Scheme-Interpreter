#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"
#include "value.h"
#include "linkedlist.h"
#include "talloc.h"
#include "eval_let.h"

/*bindings tree structure: 
    outer cons:
        *car: inner cons:
            **car: symbol
            **cdr: expression
        *cdr: next outer cons
*/
//a helper function, because this one is recursive
Value *set2(Value *var, Value *expr, Frame *frame){
    Value *binds = frame->bindings;
    Value *newBinds = makeNull();
    if (var->type != SYMBOL_TYPE){
        printf("Evaluation error: attempted to set! a non-symbol!\n");
        texit(0);
    }

    while (binds->type != NULL_TYPE){
        //printf("Comparing %s and %s\n", car(car(binds))->s, var->s);
        if (!strcmp(car(car(binds))->s, var->s)){
            //can't use addToFrame here because of some checking conditions
            *binds->c.car->c.cdr = *expr;
            return NULL;
        }
        binds = cdr(binds);
    }
    
    if (frame->parent != NULL){
        return set2(var, expr, frame->parent);
    } else {
        printf("Evaluation error: attempted to set! an undefined variable!\n");
        texit(0);
        return NULL;
    }        
}

//okay, so here's the deal: we still want to evaluate the variable under its current frame, right?
//the problem with just returning "set(args, frame->parent)" is that the variable is evaluated under its
//parent. And that's not good, right?
Value *set(Value *args, Frame *frame){
    // printTree(frame->bindings);
    // printf("->");
    Value *voidVal = talloc(sizeof(Value));
    voidVal->type = VOID_TYPE;
    voidVal->p = set2(car(args), eval(car(cdr(args)), frame), frame); 
    return voidVal;
}