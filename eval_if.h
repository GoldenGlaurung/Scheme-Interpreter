#include <stdlib.h>
#include <stdio.h>
#include "linkedlist.h"
#include "value.h"
#include "interpreter.h"
#include "look_up_symbol.h"

#ifndef _EVALIF
#define _EVALIF

Value *evalIf(Value *args, Frame *frame);

#endif