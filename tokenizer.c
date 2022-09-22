#include "value.h"
#include "talloc.h"
#include "linkedlist.h"
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

Value *makeToken(char *lexeme, valueType category);

valueType determineType(char *string, valueType guess);

int g_curLen = 0;

//Begin of Helper suite
bool isNum(char a){
    //ascii, baby!
    return (48 <= a && 57 >= a);
}

bool isNumOrDot(char a){
    return (isNum(a) || '.' == a);
}

bool isLetter(char a){
    //ascii, baby!
    return ((a >= 65 && a <= 90) || (a >= 97 && a <= 122));
}

bool isGoodInitial(char a){
    return (isLetter(a) || '!' == a || '$' == a || '%' == a || '&' == a || '*' == a || '/' == a || ':' == a 
                        || '<' == a || '=' == a || '>' == a || '?' == a || '~' == a || '_' == a || '^' == a);
}

bool isGoodSubseq(char a){
    return (isNum(a) || isGoodInitial(a) || '-' == a || '+' == a || '.' == a);
}
//End of Helper Suite

// Read source code that is input via stdin, and return a linked list consisting of the
// tokens in the source code. Each token is represented as a Value struct instance, where
// the Value's type is set to represent the token type, while the Value's actual value
// matches the type of value, if applicable. For instance, an integer token should have
// a Value struct of type INT_TYPE, with an integer value stored in struct variable i.
// See the assignment instructions for more details. 
Value *tokenize() {

    // Prepare list of tokens
    Value *tokensList = makeNull();

    // Prepare the character stream
    char nextChar;
    nextChar = (char)fgetc(stdin);
    char *cur = talloc(300 * sizeof(char));
    memset(cur, '\0', 300 * sizeof(char));

    //flags!
    bool inString = false;
    bool inSymbol = false;
    bool inComment = false;

    // Start tokenizing!
    while (nextChar != EOF) {
        Value *potentialCar = NULL;
        Value *potentialCar2 = NULL;

        //For line breaks
        if (nextChar == '\n'){
            inComment = false;
            if(inSymbol /*&& g_curLen != 0*/){
                potentialCar = makeToken(cur, determineType(cur, INT_TYPE));
                inSymbol = false;
                memset(cur, '\0', 300 * sizeof(char));
                g_curLen = 0;
            }
        } else if (inComment){
            //do nothing

            //Below this line, should not be in comment!

        //For opening and closing parenthesis
        } else if (nextChar == '(' || nextChar == ')'){
            if(!inString){
                if(inSymbol){
                    potentialCar2 = makeToken(cur, determineType(cur, INT_TYPE));
                    inSymbol = false;
                }
                memset(cur, '\0', 300 * sizeof(char));
                g_curLen = 0;
                if (nextChar == '('){
                    potentialCar = makeToken(&nextChar, OPEN_TYPE);
                } else {
                    potentialCar = makeToken(&nextChar, CLOSE_TYPE);
                }
            } else {
                cur[g_curLen] = nextChar;
                g_curLen++;           
            }

        //For strings
        } else if (nextChar == '\"'){
            cur[g_curLen] = '\"';
            g_curLen++;
            if (!inString){
                inString = true;
                g_curLen = 1;
            } else {
                inString = false;
                potentialCar = makeToken(cur, STR_TYPE);
                memset(cur, '\0', 300 * sizeof(char));
                g_curLen = 0;
            }
        
        //For comments
        } else if (nextChar == ';'){
            if (inString){
                cur[g_curLen] = nextChar;
                g_curLen++;
            } else {
                inComment = true;
            }

        //For spaces
        } else if (nextChar == ' ' || nextChar == '\t'){
            if (inSymbol && !inString){
                potentialCar = makeToken(cur, determineType(cur, INT_TYPE));
                inSymbol = false;
                memset(cur, '\0', 300 * sizeof(char));
                g_curLen = 0;
            } else if (inString){
                cur[g_curLen] = nextChar;
                g_curLen++;
            }

        //For all of the non-specific characters (numbers and letters and such)
        } else {
            if (!inString){
                inSymbol = true;
            }
            cur[g_curLen] = nextChar;
            g_curLen++;
        }


        //For the cases where two cars need to be done
        //(i.e. a terminating character needs its own car)
        if (potentialCar2 != NULL){
            Value *cdr = cons(potentialCar2, tokensList);
            tokensList = cdr;
        }

        //The normal cons maker
        if (potentialCar != NULL){
            Value *cdr = cons(potentialCar, tokensList);
            tokensList = cdr;
        }

        // Read next character
        nextChar = (char)fgetc(stdin);
    }

    // Reverse the tokens list, to put it back in order
    Value *reversedList = reverse(tokensList);
    // free(cur);
    return reversedList;
}

//given a particular string, checks which pattern it falls into (if any)
valueType determineType(char *string, valueType guess){
    valueType type = -1;
    bool dec;
    switch (guess){
        case INT_TYPE:
            if (((string[0] == '+' || string[0] == '-') && strlen(string) >= 2) || (isNum(string[0]))){
                for (int i = 1; i < strlen(string); i++){
                    if(!isNum(string[i])){
                        return determineType(string, DOUBLE_TYPE);
                    }
                }
                return INT_TYPE;
            } else {
                return determineType(string, DOUBLE_TYPE);
            }
        case DOUBLE_TYPE:
            dec = false;
            if (((string[0] == '+' || string[0] == '-' || string[0] == '.' ) && strlen(string) >= 2) || isNum(string[0])){
                if (string[0] == '.'){
                    dec = true;
                }
                for (int i = 1; i < strlen(string); i++){
                    if(!isNumOrDot(string[i]) || (string[i] == '.' && dec == true)){
                        return determineType(string, SYMBOL_TYPE);
                    }
                    if (string[0] == '.'){
                        dec = true;
                    }
                }
                return DOUBLE_TYPE;
            } else {
                return determineType(string, SYMBOL_TYPE);
            }
        case SYMBOL_TYPE:
            if (isGoodInitial(string[0])){
                for(int i = 1; i < strlen(string); i++){
                    if (!isGoodSubseq(string[i])){
                        return determineType(string, BOOL_TYPE);
                    }
                }
                return SYMBOL_TYPE;
            } else if (strlen(string) == 1 && (string[0] == '+' || string[0] == '-')) {
                return SYMBOL_TYPE;
            } else {
                return determineType(string, BOOL_TYPE);
            }
        case BOOL_TYPE:
            if (strlen(string) == 2 && string[0] == '#' && (string[1] == 'f' || string[1] == 't')) {
                return BOOL_TYPE;
            } else {
                return determineType(string, NULL_TYPE);
            }
        default:
            printf("Syntax Error! I didn't like: %s\n", string);
            texit(0);
            return -1; //should end before this line, which is present only to make the compiler happy.
    }
}

Value *makeToken(char *lexeme, valueType category){
    Value *car = talloc(sizeof(Value));
    car->type = category;
    char *excess;
    switch(category){
        case INT_TYPE:
            car->i = strtol(lexeme, &excess, 10);
            break;
        case DOUBLE_TYPE:
            car->d = strtod(lexeme, &excess);
            break;
        case STR_TYPE:
            car->s = talloc(1 + (g_curLen * sizeof(char)));
            strcpy(car->s, lexeme);
            break;
        case CONS_TYPE:
            assert(1 == 0 && "This shouldn't happen: attempted to make a CONS_TYPE Token.\n");
            break;
        case NULL_TYPE:
            assert(1 == 0 && "This shouldn't happen: attempted to make a NULL_TYPE Token.\n");
            break;
        case PTR_TYPE:
            assert(1 == 0 && "This shouldn't happen: attempted to make a PTR_TYPE Token.\n");
            break;
        case OPEN_TYPE:       
        case CLOSE_TYPE:
            car->ch = *lexeme;
            break;
        case BOOL_TYPE:
            //true = 1, false = 0
            if (lexeme[1] == 'f'){
                car->i = 0;
            } else if (lexeme[1] == 't') {
                car->i = 1;
            } else {
                assert(1 == 0 && "This shouldn't happen: oddity in boolean assignment.\n");
            }
            break;
        case SYMBOL_TYPE:
            car->s = talloc(1 + (g_curLen * sizeof(char)));
            strcpy(car->s, lexeme);
            break;
        default:
            assert(1 == 0 && "This shouldn't happen: bogus Token Category.\n");
            break;
    }
    return car;
}

// Display the contents of the list of tokens, along with associated type information.
// The tokens are displayed one on each line, in the format specified in the instructions.
void displayTokens(Value *list){
    //printf("Lexeme:Category\n");
    while (list->type != NULL_TYPE) {
        switch(list->c.car->type){
            case INT_TYPE:
                printf("%i:integer\n", list->c.car->i);
                break;
            case DOUBLE_TYPE:
                printf("%f:double\n", list->c.car->d);
                break;
            case STR_TYPE:
                printf("%s:string\n", list->c.car->s);
                break;
            case CONS_TYPE:
                assert(1 == 0 && "This shouldn't happen: cdr of cdr was CONS_TYPE.\n");
                break;
            case NULL_TYPE:
                assert(1 == 0 && "This shouldn't happen: cdr of cdr was NULL_TYPE after check.\n");
                break;
            case PTR_TYPE:
                assert(1 == 0 && "This shouldn't happen: cdr of cdr was PTR_TYPE.\n");
                break;
            case OPEN_TYPE:      
                printf("%c:open\n", list->c.car->ch);
                break; 
            case CLOSE_TYPE:
                printf("%c:close\n", list->c.car->ch);
                break;
            case BOOL_TYPE:
                if (list->c.car->i){
                    printf("#t:boolean\n");
                } else {
                    printf("#f:boolean\n");
                }
                break;
            case SYMBOL_TYPE:
                printf("%s:symbol\n", list->c.car->s);
                break;
            default:
                assert(1 == 0 && "This shouldn't happen: bogus Token Category.\n");
                break; 
        }
        list = list->c.cdr;
    }
    return;
}