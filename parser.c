#include "value.h"
#include "talloc.h"
#include "linkedlist.h"
#include "tokenizer.h"
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

void tooMany();
void tooFew();

Value *addToParseTree(Value *tree, Value *token, int *depth);

// Return a pointer to a parse tree representing the structure of a Scheme 
// program, given a list of tokens in the program.
Value *parse(Value *tokens){
    Value *current = tokens;
    assert(current != NULL && "Error (parse): null pointer");

    Value *tree = makeNull();
    int depth = 0;

    while (current->type != NULL_TYPE) {
        Value *token = car(current);
        tree = addToParseTree(tree, token, &depth);
        current = cdr(current);
    }
    if (depth < 0) {
        tooMany();
    } else if (depth > 0){
        tooFew();
    }
    return reverse(tree);
}

//because it is used so often, a shortcut function to abort early if the stack is emptied
//and there are no more open parenthesis.
void tooFew(){
    printf("Syntax error: too few close parens.\n");
    texit(0);
}

void tooMany(){
    printf("Syntax error: too many close parens.\n");
    texit(0);
}

//The function that correlates to steps 2b, 2c, and 2cI in the pseudocode instructions;
//2b: If the token is not a close paren, ), push it onto the stack.
//2c: If the token is a close paren:
//  2cI:   Pop items from the stack until you pop off an open paren--(--forming a list of these tokens as you proceed.
//  2cII:  Once you hit an open paren, push that list of tokens back onto the stack.
Value *addToParseTree(Value *tree, Value *token, int *depth){
    Value *pCar = makeNull();
    switch(token->type){
        case OPEN_TYPE:
            *depth = *depth + 1;
            pCar = token;
            break;
        case CLOSE_TYPE:
            *depth = *depth - 1;
            if(tree->type == NULL_TYPE){ //ensures that, if closing parenthesis is first, syntax error is reported
                tooMany();
            }
            while (car(tree)->type != OPEN_TYPE) {
                pCar = cons(car(tree), pCar);
                tree = cdr(tree); //should handle removal--moves pointer from head to next, "losing" it to garbage collection
                if (tree->type == NULL_TYPE){
                    tooMany();
                }
            }
            tree = cdr(tree);
            break;
        default:
            pCar = token;
            break;
    }
    return cons(pCar, tree);
}

// Print a parse tree to the screen in a readable fashion. It should look 
// just like Scheme code (use parentheses to mark subtrees).
//
// Uses a modified version of the old "display" function over in linkedlist.
void printTree(Value *tree){
    if (tree->type == CONS_TYPE) {
        printf("(");
        while (tree->type == CONS_TYPE){
            printTree(car(tree));
            tree = cdr(tree);
        }
        if (tree->type != NULL_TYPE){
            printf(" . ");
            display(tree, true);
        }
        printf(") ");
    } else { //leaf case
        display(tree, true);
    }
    return;
}