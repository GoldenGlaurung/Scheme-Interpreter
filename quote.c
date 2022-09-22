#include <stdlib.h>
#include <stdio.h>
#include "value.h"
#include "linkedlist.h"
#include "interpreter.h"
#include "parser.h"

Value *quote(Value *val, Frame *frame){
    return val;
}