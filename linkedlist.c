#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "value.h"
#include "talloc.h"
#include "linkedlist.h"


// Create a pointer to a new NULL_TYPE Value (hint: where in memory will 
// the value have to live?)
Value *makeNull(){
    Value *value = talloc(sizeof(Value));
    value->type = NULL_TYPE;
    return value;
}

// Return whether the given pointer points at a NULL_TYPE Value. Use assertions 
// to make sure that this is a legitimate operation. See the assignment
// instructions for further explanation on assertions.
bool isNull(Value *value){
    assert(value != NULL && "Value is null!\n");
    return (value->type == NULL_TYPE);
}

// Create a pointer to a new CONS_TYPE Value
Value *cons(Value *newCar, Value *newCdr){
    Value *value = talloc(sizeof(Value));
    value->type = CONS_TYPE;
    value->c.car = newCar;
    value->c.cdr = newCdr;
    return value;
}

// Return a pointer to the car value for the cons cell at the head of the given 
// linked list. Use assertions here to make sure that this is a legitimate operation 
// (e.g., there is no car value at the head of an empty list). See the assignment 
// instructions for further explanation.
Value *car(Value *list){
    isNull(list);
    assert(list->type == CONS_TYPE && "Attempted to car a non-CONS Value.\n");
    return list->c.car;
}

// Return a pointer to the cdr value for the cons cell at the head of the given linked
// list. Again use assertions to make sure that this is a legitimate operation.
Value *cdr(Value *list){
    isNull(list);
    assert(list->type == CONS_TYPE && "Attempted to cdr a non-CONS Value.\n");
    return list->c.cdr;
}

//Modified: just prints a single something.
void display(Value *list, bool quote){
        switch (list->type) {
            case INT_TYPE:
                printf("%i ", list->i);
                break;
            case DOUBLE_TYPE:
                printf("%lf ", list->d);
                break;
            case STR_TYPE:
                printf("%s ", list->s);
                break;
            case CONS_TYPE:
                if(quote){
                    printf("( ");
                }
                display(car(list), false);
                display(cdr(list), false);
                if(quote){
                    printf(" )");
                }
                break;
            case BOOL_TYPE:
                if (list->i){
                    printf("#t ");
                } else {
                    printf("#f "); 
                }
                break;
            case SYMBOL_TYPE:
                printf("%s ", list->s);
                break;
            case NULL_TYPE:
                printf("()");
                break;
            case PTR_TYPE:
                printf("PTR_TYPE.\n");
                break;
            case CLOSURE_TYPE:
                printf("#<procedure>");
                break;
            default:
                printf("Some other type? %d\n", list->type);
                break;
    }
    return;
}


// Return a new list that is the reverse of the one that is passed in. All
// content within the list should be duplicated; there should be no shared
// memory whatsoever between the original list and the new one. Use your
// cons(), car(), and cdr() functions from above -- but be sure that you
// don't end up pointing to memory used by the old list! Hint: this means 
// that you'll need to make copies of the Value structs that serve as car 
// values for the cons cells in the original list; more specifically, you'll 
// want to talloc new space for them on the heap. In the case of a string, 
// the strlen() function will come in handy, in addition to strcpy(); note 
// that strlen() will not include the null terminator in its count. To use 
// these functions, you'll need to include <string.h> above.

// FAQ: What if there are nested lists inside that list?

// ANS: There won't be for this assignment. There will be later, but that will
// be after we've set up an easier way of managing memory.
Value *reverseHelper(Value *list, int len);

Value *reverse(Value *list){
    Value *newList;
    if(isNull(list)){
        newList = makeNull();
        return newList;
    }
    int len = length(list);
    newList = reverseHelper(list, len);
    return newList;
}

Value *reverseHelper(Value *list, int len){
    //Base Case
    if (0 == len){
        Value *nullTypeValue = makeNull();
        return nullTypeValue;
    } else if (0 > len){
        assert(1 == 0 && "Len less than zero.");
    }

    //Recursive case
    Value *target = list;
    for (int i = 1; i < len; i++){
        target = target->c.cdr;
    }
    Value *carPtr = talloc(sizeof(Value));
    carPtr = target->c.car;
    Value *newCons = cons(carPtr, reverseHelper(list, --len));
    return newCons;
}
    
// Return the length of the given list, i.e., the number of cons cells. 
// Use assertions to make sure that this is a legitimate operation.
int length(Value *value){
    int len = 0;
    Value *x = value;
    while (!isNull(x)){
        x = cdr(x);
        len += 1;
    }
    return len;
}