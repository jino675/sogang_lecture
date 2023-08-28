#include <stdio.h>
#include <stdlib.h>

typedef enum { lparen = 40, rparen = 41, plus = 43, minus = 45, times = 42, divide = 47, mod = 37, eos = 0, operand = 99 } precedence;
struct node {
    precedence operator;
    struct node* link;
};
typedef struct node stack;
stack* top = NULL;
char* infix;
char* postfix;

precedence get_token(int* n);
int sig_old(precedence item);
int sig_new(precedence item);
char pop();
void s_delete();
void push(char operator);
void read_from_file();
void infix_to_postfix();

void main()
{
    read_from_file();
    infix_to_postfix();
    free(infix);
    free(postfix);
    return;
}
precedence get_token(int* n)
{
    switch (infix[*n]) {
    case '(':
        (*n)++;
        return lparen;
    case ')':
        (*n)++;
        return rparen;
    case '+':
        (*n)++;
        return plus;
    case '-':
        (*n)++;
        return minus;
    case '*':
        (*n)++;
        return times;
    case '/':
        (*n)++;
        return divide;
    case '%':
        (*n)++;
        return mod;
    case '\0':
        (*n)++;
        return eos;
    default:
        (*n)++;
        return operand;
    }
}
int sig_old(precedence item)
{
    switch (item) {
    case '(':
        return 0;
    case ')':
        return 20;
    case '+':
        return 12;
    case '-':
        return 12;
    case '*':
        return 13;
    case '/':
        return 13;
    case '%':
        return 13;
    case '\0':
        return 0;
    default:
        return -1;
    }
}
int sig_new(precedence item)
{
    switch (item) {
    case '(':
        return 20;
    case ')':
        return 0;
    case '+':
        return 12;
    case '-':
        return 12;
    case '*':
        return 13;
    case '/':
        return 13;
    case '%':
        return 13;
    case '\0':
        return 0;
    default:
        return -1;
    }
}
char pop()
{
    if (top == NULL) return -1;
    else return top->operator;
}
void s_delete()
{
    stack* temp = top;
    top = top->link;
    free(temp);
}
void push(char oper)
{
    stack* temp;
    temp = malloc(sizeof(stack));
    temp->operator = oper;
    temp->link = top;
    top = temp;
}
void read_from_file()
{
    FILE* fp = fopen("expr.txt", "r+");
    fseek(fp, 0, SEEK_END);
    int n = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    infix = malloc(sizeof(char) * (n + 1));
    postfix = malloc(sizeof(char) * (2 * n));
    char c;
    int count = 0;
    while (1) {
        if (fscanf(fp, "%c", &c) < 1) {
            printf("\n", c);
            infix[count] = '\0';
            break;
        }
        else if (c != ' ') {
            printf("%c", c);
            infix[count] = c;
            ++count;
        }
        else printf("%c", c);
    }
    printf("\n%s\n", infix);
    fclose(fp);
}
void infix_to_postfix()
{
    int count = 0;
    int index = 0;
    char token;
    while (1) {
        token = get_token(&count);
        if (token == operand) {
            postfix[index++] = infix[count - 1];
            postfix[index++] = ' ';
        }
        else if (sig_new(token) >= sig_old(pop())) {
            push(token);
        }
        else if (sig_new(token) < sig_old(pop())) {
            while ((pop() != lparen) && pop() >= 0) {
                postfix[index++] = pop();
                postfix[index++] = ' ';
                s_delete();
            }
            if (pop() == lparen) {
                if (token == rparen) {
                    s_delete();
                }
                else {
                    push(token);
                }
            }
            else if (pop() < 0) push(token);
        }
        if (token == eos) {
            postfix[index] = pop();
            printf("\n%s\n", postfix);
            break;
        }
    }
}