#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "value.h"
#include "linkedlist.h"
#include "parser.h"

#ifndef _LOOKUPSYMBOL
#define _LOOKUPSYMBOL

Value *lookUpSymbol(Value *symbol, Frame *frame);

Value *checkFrame(Value *symbol, Frame *frame);

#endif