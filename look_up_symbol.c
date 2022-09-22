#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "value.h"
#include "talloc.h"
#include "linkedlist.h"
#include "parser.h"

/*bindings tree structure: 
    outer cons:
        *car: inner cons:
            **car: symbol
            **cdr: expression
        *cdr: next outer cons
*/

//first checks the current frame, then it's parent, all the way up...
Value *checkFrame(Value *symbol, Frame *frame){
    Value *binds = frame->bindings;
    while (binds->type != NULL_TYPE){
        //printf("\nChecking binding: %s and symbol: %s\n", car(car(binds))->s, symbol->s);
        if (!strcmp(car(car(binds))->s, symbol->s)){
            //printTree(cdr(car(binds)));
            //printf("\n");
            if (cdr(car(binds))->type == UNSPECIFIED_TYPE){
                printf("Evaluation error: attempted to access UNSPECIFIED_TYPE variable (letrec).\n");
                texit(0);
            }
            return cdr(car(binds));
        }
        binds = cdr(binds);
    }
    return NULL;
}

//takes in a symbol and a frame and attempts to match the symbol to one bound in a frame.
//returns a NULL_TYPE Value if no match is found (error checking is on the receiving side)
Value *lookUpSymbol(Value *symbol, Frame *frame){
    //we know that symbol->type is symbol, so no need to check/guess
    if (frame == NULL){
        return NULL;
    }
    Frame *temp = frame;
    Value *val = checkFrame(symbol, temp);
    if (val == NULL){
        if (temp->parent != NULL){
            return lookUpSymbol(symbol, frame->parent);
        }
        return val;
    } else {       
        return val;
    }
}