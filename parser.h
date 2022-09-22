#include "value.h"

#ifndef _PARSER
#define _PARSER


// Return a pointer to a parse tree representing the structure of a Scheme 
// program, given a list of tokens in the program.
Value *parse(Value *tokens);


// Print a parse tree to the screen in a readable fashion. It should look 
// just like Scheme code (use parentheses to mark subtrees).
void printTree(Value *tree);


#endif
