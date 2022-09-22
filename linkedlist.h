#include <stdbool.h>
#include "value.h"

#ifndef _LINKEDLIST
#define _LINKEDLIST

// Create a pointer to a new NULL_TYPE Value (hint: where in memory will 
// the value have to live?)
Value *makeNull();

// Return whether the given pointer points at a NULL_TYPE Value. Use assertions 
// to make sure that this is a legitimate operation. See the assignment
// instructions for further explanation on assertions.
bool isNull(Value *value);

// Create a pointer to a new CONS_TYPE Value
Value *cons(Value *newCar, Value *newCdr);

// Return a pointer to the car value for the cons cell at the head of the given 
// linked list. Use assertions here to make sure that this is a legitimate operation 
// (e.g., there is no car value at the head of an empty list). See the assignment 
// instructions for further explanation.
Value *car(Value *list);

// Return a pointer to the cdr value for the cons cell at the head of the given linked
// list. Again use assertions to make sure that this is a legitimate operation.
Value *cdr(Value *list);

// Display the contents of the linked list to the screen in the
// format of a Scheme list -- e.g., ( 33 "lol" 9.9 ). It's okay
// to just use printf here, though you'll have to add the quotes in
// yourself, for strings.
void display(Value *list, bool quote);

// Return a new list that is the reverse of the one that is passed in. None of
// the values in the original linked list should be copied this time. Instead, 
// create a new linked list of CONS_TYPE nodes whose car values point to the 
// corresponding car values in the original list.
Value *reverse(Value *list);

// Return the length of the given list, i.e., the number of cons cells. 
// Use assertions to make sure that this is a legitimate operation.
int length(Value *list);

#endif
