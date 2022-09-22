#include "eval_if.h"
#include "lambda.h"
#include "eval_let.h"
#include "look_up_symbol.h"
#include "value.h"
#include "talloc.h"
#include "linkedlist.h"
#include "tokenizer.h"
#include "parser.h"
#include "quote.h"
#include "define.h"
#include "built_in_functs.h"
#include "eval_and_or.h"
#include "eval_cond.h"
#include "set!.h"
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

//A shortcut that takes in an error message string and appends it to the default error
//and then quits.
//todo: figure out what the parameters to exit really do.
void evalError(char *message){
    printf("Evaluation error: %s\n", message);
    texit(0);
}

Value *evalEach(Value *tree, Frame *frame, bool print);

Value *apply(Value *function, Value *args);

/* Given the parse tree for an S-expression (tree) and a frame in which to evaluate that 
expression (frame), eval evaluates the expression and returns the resulting value. */
Value *eval(Value *tree, Frame *frame) {
    Value *val;

    // printf("\n");
    // printTree(tree);
    // //printf("%d\n", tree->type);
    // printf("\n");

    //the big one. Value type is everything here.
    switch (tree->type)  {
        case CLOSURE_TYPE:
        case BOOL_TYPE:
        case INT_TYPE:
        case DOUBLE_TYPE:
        case STR_TYPE:
            return tree;
        case SYMBOL_TYPE:
            val = lookUpSymbol(tree, frame);
            if (val == NULL){
                //printf("unbound var: %s!\n", tree->s);
                evalError("possible unbound variable!");  
            } else {
                return val; 
            }
            break;
        case CONS_TYPE: {
            Value *first = car(tree);
            Value *args = cdr(tree);
            if (first->type == SYMBOL_TYPE){
                //IF
                if (!strcmp(first->s, "if")) {
                    //error check
                    if (length(args) != 3){
                        evalError("Wrong number of parameters for \"if\"");
                    }
                    return eval(evalIf(args, frame), frame);

                //LET
                } else if (!strcmp(first->s, "let")) {
                    //error check
                    if (length(args) == 1 || args->type == NULL_TYPE){
                        evalError("Wrong number of parameters for \"let\"");
                    }
                    return evalLet(args, frame);

                //LET*
                } else if (!strcmp(first->s, "let*")) {
                    //error check
                    if (length(args) == 1 || args->type == NULL_TYPE){
                        evalError("Wrong number of parameters for \"let*\"");
                    }
                    return evalLetStar(args, frame);
                
                //LETREC
                } else if (!strcmp(first->s, "letrec")) {
                    //error check
                    if (length(args) == 1 || args->type == NULL_TYPE){
                        evalError("Wrong number of parameters for \"letrec\"");
                    }
                    return evalLetrec(args, frame);

                //QUOTE
                } else if (!strcmp(first->s, "quote")){
                    //error check
                    if (length(args) != 1 || args->type == NULL_TYPE){
                        evalError("Wrong number of parameters for \"quote\"");
                    }
                    return quote(car(args), frame);

                //Lambda
                } else if (!strcmp(first->s, "lambda")){
                    if (length(args) != 2){
                        evalError("Wrong number of parameters for \"lambda\"");
                    }
                    return lambda(args, frame);

                //Define
                } else if (!strcmp(first->s, "define")){
                    if (length(args) != 2){
                        evalError("Wrong number of parameters for \"define\"");
                    }
                    return define(args, frame);

                //and/or
                } else if (!strcmp(first->s, "and")){
                    return eval_and(args, frame);
                } else if (!strcmp(first->s, "or")){
                     return eval_or(args, frame);
                
                //Begin
                } else if (!strcmp(first->s, "begin")){
                    //check for zero args, return VOID_TYPE so nothing prints
                    if (length(args) == 0){
                        val = makeNull();
                        val->type = VOID_TYPE;
                        val->p = NULL;
                        return val;
                    }
                    val = evalEach(args, frame, false);
                    if (val->type == CONS_TYPE){
                        return car(val);
                    } else {
                        return val;
                    }
                    
                //Cond   
                } else if (!strcmp(first->s, "cond")){  
                    return eval_cond(args, frame);

                //Set!
                } else if (!strcmp(first->s, "set!")){
                    if (length(args) != 2){
                        evalError("Wrong number of parameters for \"set!\"");
                    }
                    return set(args, frame);
                }

            }

            //Apply
            // If it's not a special form, evaluate 'first', evaluate the args,
            // then apply 'first' on the args.
            Value *evaluatedOperator = eval(first, frame);
            if (evaluatedOperator->type == CLOSURE_TYPE){
                Value *evaluatedArgs = evalEach(args, frame, false);
                return apply(evaluatedOperator, evaluatedArgs);
            //handle primitive types
            } else if (evaluatedOperator->type == PRIMITIVE_TYPE){
                Value *(**primFunct)(Value *) = &evaluatedOperator->pf;
                return (*primFunct)(reverse(evalEach(args, frame, false)));
            }
            break;
        }
        case UNSPECIFIED_TYPE:
            evalError("attempted to evaluate UNSPECIFIED_TYPE (letrec).");
            break;
        default:
            // printf("tree->type: %d\n tree:", tree->type);
            // printTree(tree);
            evalError("hit default on Eval!");
            break;
    }    
    return NULL;
}


/*
Called after evaluating some Value to be a closure for function funct,
submitting the arguments for the function call, args, along with the closure.
Returns the result of executing the function on those arguments.

Construct a new frame whose parent frame is the environment stored in the closure.
Because scheme is statically scoped, this will be the frame that was active when the function was defined.

Add bindings to the new frame mapping each formal parameter (found in the closure) to the corresponding argument (found in args).

Evaluate the function body (found in the closure) with the new frame as its environment,
and return the result of the call to eval. Just like always, you'll need to check the parent
frame in the case of a variable in the body that was not bound to an argument.
*/
Value *apply(Value *funct, Value *args){
    Frame *newFrame = talloc(sizeof(Frame));
    Value *pNames = reverse(funct->cl.paramNames);
    newFrame->parent = funct->cl.frame;
    newFrame->bindings = makeNull();

    if (length(args) != length(pNames)){
        evalError("Invalid number of arguments!");
    }

    while (pNames->type != NULL_TYPE && args->type != NULL_TYPE){
        //For reference: Value *addToFrame(Value *symbol, Value *expression, Frame *frame)
        // printf("\nargument: ");
        // printTree(car(args));
        // printf("\nparamName: ");
        // printTree(car(pNames));
        // printf("\n");
        ///*THANK YOU KEVIN FOR YOUR LOVELY DISCUSSION ON SLACK!!!!*/
        Value *value = talloc(sizeof(Value));
        *value = *car(args);
        newFrame->bindings = addToFrame(car(pNames), value, newFrame);
        pNames = cdr(pNames);
        args = cdr(args);
    }
    return eval(funct->cl.functionCode, newFrame);
}


/*
What it says on the tin, really. Turns a list of Values, here items, into
a list of evaluated Values, and then returns that list. Boolean print is used
to determine if printing is necissary.
*/
Value *evalEach(Value *items, Frame *frame, bool print){
    Value *newList = makeNull();
    Value *local = makeNull();
    while(items->type != NULL_TYPE){
        local = eval(car(items), frame);
        if (local->type == VOID_TYPE){
            if (local->p != NULL){
                frame->bindings = local->p;
            }
        } else {
            if (print) {
                printTree(local);
                printf("\n");
            }
            newList = cons(local, newList);
        }
        items = cdr(items);
    }
    return newList;
}

void bindPrimitiveFunction(char *name, Value *(*funct)(struct Value *), Frame *frame) {
    // Bind 'name' to 'function' in 'frame'
    Value *expr = talloc(sizeof(Value));
    expr->type = PRIMITIVE_TYPE;
    expr->pf = funct;
    //for reference: Value *addToFrame(Value *symbol, Value *expression, Frame *frame);
    Value *symbol = talloc(sizeof(Value));
    symbol->type = SYMBOL_TYPE;
    symbol->s = name;
    frame->bindings = addToFrame(symbol, expr, frame);
    return;
}


//sets the global frame then calls eval on each initial branch of the input tree
void interpret(Value *tree){
    Frame *gFrame = talloc(sizeof(Frame));
    gFrame->bindings = makeNull();
    gFrame->parent = NULL;

    bindPrimitiveFunction("car", &builtInCar, gFrame);
    bindPrimitiveFunction("cdr", &builtInCdr, gFrame);
    bindPrimitiveFunction("cons", &builtInCons, gFrame);
    bindPrimitiveFunction("+", &builtInPlus, gFrame);
    bindPrimitiveFunction("-", &builtInMinus, gFrame);
    bindPrimitiveFunction("*", &builtInMultiply, gFrame);
    bindPrimitiveFunction("/", &builtInDivide, gFrame);
    bindPrimitiveFunction("modulo", &builtInModulo, gFrame);
    bindPrimitiveFunction("<", &builtInLessThan, gFrame);
    bindPrimitiveFunction(">", &builtInMoreThan, gFrame);
    bindPrimitiveFunction("=", &builtInEquals, gFrame);
    bindPrimitiveFunction("null?", &builtInNull, gFrame);

    evalEach(tree, gFrame, true);
    return;
}