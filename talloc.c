#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "value.h"
#include "linkedlist.h"

Value *g_active_list = NULL;

// Replacement for malloc that stores the pointers allocated. It should store
// the pointers in a linked list, and you have license here to duplicate code
// that you wrote for linkedlist.c. To be clear, don't actually call functions 
// that are defined in linkedlist.h, because then you'll end up with circular
// dependencies, since you'll be using talloc in linkedlist.c.
void *talloc(size_t size){  
    //Make the car value, which holds a pointer
    Value *ptrVal = malloc(sizeof(Value));
    ptrVal->type = PTR_TYPE;
    ptrVal->p = malloc(size);
    //prepare the cons value
    Value *newCons = malloc(sizeof(Value));
    newCons->type = CONS_TYPE;
    newCons->c.car = ptrVal;
    if (g_active_list == NULL){
        Value *end = malloc(sizeof(Value));
        end->type = NULL_TYPE;
        newCons->c.cdr = end;
    } else {
        newCons->c.cdr = g_active_list;
    }
    g_active_list = newCons;
    return ptrVal->p;
}

// Free all pointers allocated by talloc, as well as whatever memory you
// allocated for purposes of maintaining the active list. Hint: consider 
// that talloc may be called again after tfree is called...
void tfree(){
    Value *target = g_active_list;

    while(target->type == CONS_TYPE){
        free(target->c.car->p);
        free(target->c.car);
        Value *newValue = target;
        target = target->c.cdr;
        free(newValue);
    }
    free(target);
    g_active_list = NULL;
    return;
}

// Replacement for the C function 'exit' that consists of two lines: it calls
// tfree before calling exit. It's useful to have later on, since you'll be able
// to call it to clean up memory and exit your program whenever an error occurs.
// Briefly look up exit to get a sense of what the 'status' parameter does.
void texit(int status){
    tfree();
    exit(status);
}