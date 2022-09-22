#include <stdio.h>
#include "tokenizer.h"
#include "value.h"
#include "linkedlist.h"
#include "parser.h"
#include "talloc.h"
#include "interpreter.h"

int main() {

    Value *tokensList = tokenize();
    Value *tree = parse(tokensList);
    interpret(tree);

    tfree();
    return 0;
}
