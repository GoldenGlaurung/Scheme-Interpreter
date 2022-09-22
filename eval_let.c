#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "parser.h"
#include "value.h"
#include "linkedlist.h"
#include "talloc.h"
#include "look_up_symbol.h"
#include "interpreter.h"

Value *addToFrame(Value *symbol, Value *expression, Frame *frame);

//some simple error-checking
void check(Value *pair){
    if (pair->type != CONS_TYPE || length(pair) != 2){
        printf("Evaluation error: bad \"let\"\n");
        texit(0);
    }
    if (car(pair)->type != SYMBOL_TYPE){
        printf("Evaluation error: left side of a \"let\" pair doesn't have a variable.\n");
        texit(0);
    }
    return;
}

//takes in a list of args and a frame with the intent of modifying the frame with additional symbols.
//things get complicated when symbols need evaluation first...
Value *evalLet(Value *args, Frame *frame){
    Frame *newFrame = talloc(sizeof(Frame));
    newFrame->parent = frame;
    newFrame->bindings = makeNull();

    Value *binding = car(args);
    //throws errors on cases like (let 1 2) but not for (let () 1)--the latter of which is apparently valid
    if (binding->type != CONS_TYPE && binding->type != NULL_TYPE) {
        printf("Evaluation error: bad \"let\"\n");
        texit(0);
    }
    Value *pair;
    
    //first while loop: for the bindings
    while (binding->type == CONS_TYPE) {
        pair = car(binding);
        check(pair);
        Value *checker = newFrame->bindings;
        while (checker->type != NULL_TYPE){
            Value *prevSymbol = car(car(checker));
            if (!strcmp(prevSymbol->s, car(pair)->s)){
                printf("Evaluation error: repeated variable in \"let\"\n");
                texit(0);
            }
            checker = cdr(checker);
        }
        //newFrame->bindings = addToFrame(car(pair), car(cdr(pair)), newFrame);
        newFrame->bindings = cons(cons(car(pair), eval(car(cdr(pair)), newFrame->parent)), newFrame->bindings);
        binding = cdr(binding);
    }

    //second while loop: for the arguments. Only returns the last one, but still
    //needs to evaluate the others (for some reason)
    Value *result = makeNull();
    Value *params = cdr(args);
    if (params->type == NULL_TYPE){
        printf("Evaluation error: no parameters to evaluate in \"let\".\n");
        texit(0);
    }

    while (params->type != NULL_TYPE) {
        result = eval(car(params), newFrame);
        params = cdr(params);
    }
    return result;
}

//let, but with a new frame each time--allowing successive bindings to reference their ancestors
Value *evalLetStar(Value *args, Frame *frame){
    Frame *newFrame = talloc(sizeof(Frame));
    newFrame->parent = frame;
    newFrame->bindings = makeNull();

    Value *binding = car(args);
    //throws errors on cases like (let 1 2) but not for (let () 1)--the latter of which is apparently valid
    if (binding->type != CONS_TYPE && binding->type != NULL_TYPE) {
        printf("Evaluation error: bad \"let\"\n");
        texit(0);
    }

    Value *pair;
    while (binding->type != NULL_TYPE){
        pair = car(binding);
        check(pair);
        //newFrame->bindings = addToFrame(car(pair), car(cdr(pair)), newFrame);
        newFrame->bindings = cons(cons(car(pair), eval(car(cdr(pair)), newFrame->parent)), newFrame->bindings);

        Frame *newNewFrame = talloc(sizeof(Frame));
        newNewFrame->bindings = makeNull();
        newNewFrame->parent = newFrame;

        newFrame = newNewFrame;
        binding = cdr(binding);
    }

    Value *result = makeNull();
    Value *params = cdr(args);
    if (params->type == NULL_TYPE){
        printf("Evaluation error: no parameters to evaluate in \"let\".\n");
        texit(0);
    }

    while (params->type != NULL_TYPE) {
        result = eval(car(params), newFrame);
        params = cdr(params);
    }
    return result;
}

/* Thanks, Dave
// 1. Create a new frame env-2 with parent env-1.
// 2. Create each of the bindings, and set them to UNSPECIFIED_TYPE (add this to value.h).
// 3. Evaluate each of e-1,..., e-n in environment env-2.
//     If any of those evaluations use anything with UNSPECIFIED_TYPE, that should result in an error.
// 4. After all of these evaluations are complete, replace bindings for each v-i with the
//     evaluated result of e-i (from step 2) in environment env-2.
// 5. Evaluate body-1,..., body-n sequentially in env-2, and return the result of evaluating body-n.
*/
Value *evalLetrec(Value *args, Frame *frame){
    //Step 1:
    Frame *newFrame = talloc(sizeof(Frame));
    newFrame->parent = frame;
    newFrame->bindings = makeNull();

    Value *binding = car(args);
    //throws errors on cases like (let 1 2) but not for (let () 1)--the latter of which is apparently valid
    if (binding->type != CONS_TYPE && binding->type != NULL_TYPE) {
        printf("Evaluation error: bad \"letrec\"\n");
        texit(0);
    }
    Value *pair;
    
    //Step 2 and Step 3 (I think they get evaluated in the addToFrame function):
    while (binding->type == CONS_TYPE) {
        pair = car(binding);
        check(pair);
        Value *unspec = talloc(sizeof(Value));
        unspec->type = UNSPECIFIED_TYPE;
        newFrame->bindings = addToFrame(car(pair), unspec, newFrame);
        binding = cdr(binding);
    }
    
    Value *newBinding = makeNull();
    binding = car(args);

    //Step 4:
    while (binding->type == CONS_TYPE) {
        pair = car(binding);
        check(pair);
        Value *evaluated = eval(car(cdr(pair)), newFrame);
        if (evaluated->type == UNSPECIFIED_TYPE){
            printf("Evaluation error: undefined symbol in letrec.\n");
            texit(0);
        }
        newBinding = cons(cons(car(pair), evaluated), newBinding);
        binding = cdr(binding);
    }
    newFrame->bindings = newBinding;

    //Step 5:
    Value *result = makeNull();
    Value *params = cdr(args);
    if (params->type == NULL_TYPE){
        printf("Evaluation error: no parameters to evaluate in \"letrec\".\n");
        texit(0);
    }

    while (params->type != NULL_TYPE) {
        result = eval(car(params), newFrame);
        params = cdr(params);
    }
    return result;
}


//does some additional checking, then makes a cons_value that houses the
//symbol and the expression
Value *addToFrame(Value *symbol, Value *expression, Frame *frame){
    Value *checker = frame->bindings;

    #pragma region  //Error checking
    //make sure that symbol is actually symbol_type"
    if (symbol->type != SYMBOL_TYPE){
        // printTree(symbol);
        printf("Evaluation error: non-symbol assignment!\n");
        texit(0);
    }

    //check for repeated symbols
    while(checker->type != NULL_TYPE){
        if (!strcmp(symbol->s, car(car(checker))->s)){
            printf("Evaluation error: repeated variable in \"let\"\n");
            texit(0);
        }
        checker = cdr(checker);
    }
    #pragma endregion


    Value *lookup;
    //get symbols, if necessary
    switch(expression->type){
        case SYMBOL_TYPE:
            return addToFrame(symbol, eval(expression, frame->parent), frame);
        case CONS_TYPE:
            if (car(expression)->type == SYMBOL_TYPE){
                return addToFrame(symbol, eval(expression, frame), frame);
            } else {
                break;
            }
        default:
            //printf("%d\n", expression->type);
            break;
    }

    //wrap things up with a bow...
    Value *mid = makeNull();
    mid = cons(symbol, expression);
    return cons(mid, frame->bindings);
}