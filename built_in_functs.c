#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "talloc.h"
#include "linkedlist.h"
#include "value.h"
#include "parser.h"
#include "interpreter.h"

void error(char *message){
    printf("%s", message);
    texit(0);
}

//error check, then take the car of the first value of the list of args
//(which should only be one arg anyway).
Value *builtInCar(Value *args){
    if (!(args->type == CONS_TYPE && car(args)->type == CONS_TYPE)){
        error("Evaluation error: attempted to car something that wasn't a list!\n");
    } else if (length(args) != 1){
        error("Evaluation error: \"car\" only takes one arg!\n");
    }
    return car(car(args));
}

//similar to above; error check, then return.
Value *builtInCdr(Value *args){
    if (args->type != CONS_TYPE){
        error("Evaluation error: attempted to cdr something that wasn't a list!\n");
    } else if (length(args) != 1){
        error("Evaluation error: \"cdr\" only takes one arg!\n");
    }
    return cdr(car(args));
}

//this cons is somewhat odd in that the cdr doesn't always point to a list; it could point to
//an INT_TYPE, for example. The printTree function handles this with . notation.
Value *builtInCons(Value *args){
    if (args->type != CONS_TYPE || cdr(args)->type != CONS_TYPE) {
        error("Evaluation error: bad args for \"cons\"!\n");
    } else if (length(args) != 2) {
        error("Evaluation error: \"cons\" takes two args!\n");
    }
    return cons(car(args), car(cdr(args)));
}

Value *builtInPlus(Value *args){
    bool isInt = true;
    double sum = 0.0;
    ///*iterate through the args, check each time for double or int to
    //get the right value and set the boolean isInt to false if a 
    //double is detected.*/
    if (args->type == CONS_TYPE){
        Value *temp;
        while(args->type != NULL_TYPE){
            temp = car(args);
            if (temp->type == INT_TYPE){
                sum += temp->i;
            } else if (temp->type == DOUBLE_TYPE){
                isInt = false;
                sum += temp->d;
            } else {
                error("Evaluation error: non-number argument in function \"+\"\n");
            }
            args = cdr(args);
        }
    }
    Value *ret = makeNull();
    //determine if result should be an int or a double, then do the math
    if (isInt) {
        ret->type = INT_TYPE;
        ret->i = (int)(sum);
    } else {
        ret->type = DOUBLE_TYPE;
        ret->d = sum;
    }
    return ret;
}

//in theory similar to plus, but because we don't start with zero and error
//checking on the first value is needed before assignment, the earlier used
//"builtInPlus"-style of solving doesn't work well here. Instead, the function
//just goes in order: checks first arg, second arg, and then computes. No loops.
Value *builtInMinus(Value *args){
    if (length(args) != 2) {
        error("Evaluation error: \"-\" takes two arguments.\n");
    }

    Value *first = car(args);
    if (first->type != INT_TYPE && first->type != DOUBLE_TYPE) {
        error("Evaluation error: bad arguments for \"-\"\n");
    }
    bool int1 = (first->type == INT_TYPE);

    Value *second = car(cdr(args));
    if (second->type != INT_TYPE && second->type != DOUBLE_TYPE) {
        error("Evaluation error: bad arguments for \"-\"\n");
    }
    bool int2 = (second->type == INT_TYPE);

    Value *ret = makeNull();
    //determine if result should be an int or a double, then do the math
    //it's a bit of a process because the algorithm doesn't iterate through
    //each of the args
    if (int1 && int2) {
        ret->type = INT_TYPE;
        ret->i = first->i - second->i;
    } else if (int1 && !int2) {
        ret->type = DOUBLE_TYPE;
        ret->d = first->i - second->d;
    } else if (!int1 && int2) {
        ret->type = DOUBLE_TYPE;
        ret->d = first->d - second->i;
    } else {
        ret->type = DOUBLE_TYPE;
        ret->d = first->d - second->d;
    }
    return ret;
}

//similar to "builtInPlus"
Value *builtInMultiply(Value *args){
    bool isInt = true;
    double product = 1.0;
    if (args->type == CONS_TYPE){
        Value *temp;
        while(args->type != NULL_TYPE){
            temp = car(args);
            if (temp->type == INT_TYPE){
                product *= temp->i;
            } else if (temp->type == DOUBLE_TYPE){
                isInt = false;
                product *= temp->d;
            } else {
                error("Evaluation error: non-number argument in function \"+\"\n");
            }
            args = cdr(args);
        }
    }
    Value *ret = makeNull();
    //determine if result should be an int or a double, then do the math
    if (isInt) {
        ret->type = INT_TYPE;
        ret->i = (int)(product);
    } else {
        ret->type = DOUBLE_TYPE;
        ret->d = product;
    }
    return ret;
}

//like "builtInMinus", checks each of the two args undividually then does some
//logic and math to determine what to return.
Value *builtInDivide(Value *args){
    //error check # of args
    if (length(args) != 2) {
        error("Evaluation error: \"/\" takes two arguments.\n");
    }

    double first;
    if (car(args)->type == INT_TYPE){
        first = car(args)->i * 1.0;
    } else if (car(args)->type == DOUBLE_TYPE) {
        first = car(args)->d;
    } else {
        error("Evaluation error: bad arguments for \"/\"\n");
    }
    
    double second;
    if (car(cdr(args))->type == INT_TYPE){
        second = car(cdr(args))->i * 1.0;
    } else if (car(cdr(args))->type == DOUBLE_TYPE){
        second = car(cdr(args))->d;
    } else {
        error("Evaluation error: bad arguments for \"/\"\n");
    }

    //check so that we don't divide by zero
    if (second == 0){
        error("Evaluation error: attempted to divide by zero!\n");
    }

    //determine if result should be an int or a double, then do the math
    Value *ret = makeNull();
    double result = first / second;
    if (result == (int)(result)){
        ret->type = INT_TYPE;
        ret->i = (int)result;
    } else {
        ret->type = DOUBLE_TYPE;
        ret->d = result;
    }
    return ret;
}

Value *builtInModulo(Value *args){
    //error check # of args
    if (length(args) != 2) {
        error("Evaluation error: \"modulo\" takes two arguments.\n");
    }

    //check so that both arguments are ints
    if (car(args)->type != INT_TYPE || car(cdr(args))->type != INT_TYPE){
        error("Evaluation error: \"modulo\" takes integer arguments!\n");
    }

    //check so that we don't divide by zero
    if (car(cdr(args)) == 0){
        error("Evaluation error: attempted to divide by zero!\n");
    }

    Value *ret = makeNull();
    ret->type = INT_TYPE;
    ret->i = car(args)->i % car(cdr(args))->i;
    return ret;
}

//borrows the number prep from "builtInDivide", then does one of three comparisons
//(deepending on id arg) to return the appropraite truthy statement.
bool comparitor(Value *args, int id){
    //error check # of args
    if (length(args) != 2) {
        error("Evaluation error: logical comparisons take two arguments.\n");
    }

    double first;
    if (car(args)->type == INT_TYPE){
        first = car(args)->i * 1.0;
    } else if (car(args)->type == DOUBLE_TYPE) {
        first = car(args)->d;
    } else {
        error("Evaluation error: bad arguments for logical comparison.\n");
    }
    
    double second;
    if (car(cdr(args))->type == INT_TYPE){
        second = car(cdr(args))->i * 1.0;
    } else if (car(cdr(args))->type == DOUBLE_TYPE){
        second = car(cdr(args))->d;
    } else {
        error("Evaluation error: bad arguments for logical comparison.\n");
    }
    
    switch(id){
        case 0: //less than
            return (first < second);
        case 1: //greater than
            return (first > second);
        case 2: //equal to
            return (first == second);
        default:
            error("Internal error: comparitor in built-in suite (oops).\n");
            
    }
    return false;
}

Value *builtInLessThan(Value *args){
    Value *ret = makeNull();
    ret->type = BOOL_TYPE;
    ret->i = comparitor(args, 0);
    return ret;
}

Value *builtInMoreThan(Value *args){
    Value *ret = makeNull();
    ret->type = BOOL_TYPE;
    ret->i = comparitor(args, 1);
    return ret;
}

Value *builtInEquals(Value *args){
    Value *ret = makeNull();
    ret->type = BOOL_TYPE;
    ret->i = comparitor(args, 2);
    return ret;
}

Value *builtInNull(Value *args){
    //error check # of args
    if (args->type != CONS_TYPE || cdr(args)->type != NULL_TYPE){
        error("Evaluation error: wrong number of arguments in function \"null?\"\n");
    }
    Value *result = makeNull();
    result->type = BOOL_TYPE;

    if (car(args)->type == NULL_TYPE){
        result->i = 1; //true
    } else {
        result->i = 0; //false
    }
    return result;
}