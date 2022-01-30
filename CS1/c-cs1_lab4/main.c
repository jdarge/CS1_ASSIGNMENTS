#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SIZE 100
#define EMPTY -1

struct stack {
    char items[SIZE];
    int top;
};

void initialize(struct stack *stackPtr);//
void print_stack(struct stack *stackPtr);//
char *infixToPostfix(char infix[]);//                                     <-----
char pop(struct stack *stackPtr);//
int push(struct stack *stackPtr, char value);
int peek(struct stack *stackPtr);//
int full(struct stack *stackPtr);//
int empty(struct stack *stackPtr);//
int priority(char ch);//
int isOperator(char ch);//
int isParentheses(char ch1);//
int checkBalance(char exp[]);//                                           <-----

int main(void) {
    int valid;
    char exp[SIZE];

    printf("\nEnter Expression: ");
    scanf("%[^\n]s", exp);

    printf("Your input expression: %s\n", exp);

    valid = checkBalance(exp);

    if (valid){
        char *postfix = infixToPostfix(exp);
        printf("The postfix is: %s\n", postfix);
        free(postfix);
    }
}

void initialize(struct stack *stackPtr) {
    stackPtr->top = -1;
}

void print_stack(struct stack *stackPtr) {
    for (int i = 0; i <= stackPtr->top; i++){
        printf("%c-->", stackPtr->items[i]);
    } printf("\n");
}

char *infixToPostfix(char infix[]) {
    struct stack operators;
    initialize(&operators);

    int count = 0;

    char *postfix =  (char*)malloc((strlen(infix)+1)*sizeof(char));

    for(unsigned int i = 0; i < strlen(infix); i++){
        if(isdigit(infix[i])){
            while(isdigit(infix[i]))
                postfix[count++] = infix[i++];
            i--;
            postfix[count++] = ' ';
        }

        else if(isOperator(infix[i])){
            if(empty(&operators))
                push(&operators, infix[i]);
            else if(priority(infix[i]) > priority(peek(&operators)))
                push(&operators, infix[i]);
            else{
                while(!empty(&operators) && priority(infix[i]) <= priority(peek(&operators)) && !isParentheses(peek(&operators)) ){
                    postfix[count++] = pop(&operators);
                    postfix[count++] = ' ';
                }
                push(&operators, infix[i]);
            }
        }

        else if(infix[i] == '(' || infix[i] == ')'){
            if(infix[i] == '(')
                push(&operators, infix[i]);
            else{
                while(peek(&operators) != '('){
                    postfix[count++] = pop(&operators);
                    postfix[count++] = ' ';
                }
                pop(&operators);
            }
        }
    }

    while(!empty(&operators)){
        postfix[count++] = ' ';
        postfix[count++] = pop(&operators);
    }
    postfix[count] = '\0';

    return postfix;
}

char pop(struct stack *stackPtr) {
    char retval;

    if (empty(stackPtr))
        return 'I';

    retval = stackPtr->items[stackPtr->top];
    (stackPtr->top)--;
    return retval;
}

int push(struct stack *stackPtr, char value) {
    if (full(stackPtr))
        return 0;

    stackPtr->items[stackPtr->top + 1] = value;
    (stackPtr->top)++;
    return 1;
}

int peek(struct stack *stackPtr) {
    if (empty(stackPtr))
        return EMPTY;

    return stackPtr->items[stackPtr->top];
}

int full(struct stack *stackPtr) {
    return (stackPtr->top == SIZE - 1);
}

int empty(struct stack *stackPtr) {
    return (stackPtr->top == -1);
}

int priority(char ch) {
    if (ch == '(' || ch == '[' || ch == '{') return 0;
    if (ch == '+' || ch == '-') return 1;
    if (ch == '*' || ch == '/' || ch == '%') return 2;
    if (ch == '^') return 3;

    return -1;//                                                             !!!
}

int isOperator(char ch) {
    if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^' || ch == '%')
        return 1;

    return 0;
}

int isParentheses(char ch1) {
    if (ch1 == '{' || ch1 == '}' || ch1 == '(' || ch1 == ')' || ch1 == '[' || ch1 == ']') return 1;

    return 0;
}

int checkBalance(char exp[]) {
    int valid = 1;
    struct stack mine;

    printf("\nChecking balance...\n");

    initialize(&mine);
    for(int i=0; exp[i] != '\0'; i++){
        if(exp[i] == '(' || exp[i] == '{' || exp[i] == '['){
            push(&mine, exp[i]);
        }

        else if(exp[i] == ')'){
            char a = pop(&mine);
            if(a == 'I' || a != '('){
                printf("INVALID for )!!!\n");
                return 0;
            }
        }//end else-if

        else if(exp[i] == '}'){
            char a = pop(&mine);
            if(a == 'I' || a != '{'){
                printf("INVALID for }!!!\n");
                return 0;
            }
        }//end else-if

        else if(exp[i] == ']'){
            char a = pop(&mine);
            if(a == 'I' || a != '['){
                printf("INVALID for ]!!!\n");
                return 0;
            }
        }//end else-if
    }//end for-loop

    if(pop(&mine) != 'I'){
        printf("INVALID\n");
        return 0;
    }

    printf("VALID\n");
    return valid;
}

