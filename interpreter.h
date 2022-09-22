#ifndef _INTERPRETER
#define _INTERPRETER

void interpret(Value *tree);

Value *eval(Value *expr, Frame *frame);

Value *evalEach(Value *expr, Frame *frame, bool print);

#endif

