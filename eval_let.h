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

#ifndef _EVALLET
#define _EVALLET

Value *addToFrame(Value *symbol, Value *expression, Frame *frame);

void check(Value *pair);

Value *evalLet(Value *args, Frame *frame);

Value *evalLetStar(Value *args, Frame *frame);

Value *evalLetrec(Value *args, Frame *frame);

#endif