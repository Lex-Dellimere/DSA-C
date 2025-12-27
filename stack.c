#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef union {
    int *arrayOfInt;
    char *arrayOfChar;
} Type;

struct Stack {
    int top;
    unsigned int capacity;
    int currentAmount;
    int stackCeiling;
    int type;
    Type array;
};

struct Stack *createStack(int stackCeiling, int type) {

    struct Stack *stack = (struct Stack *)malloc(sizeof(struct Stack));

    if (!stack) {
        return NULL;
    }
    stack->capacity = 1;
    stack->top = -1;
    stack->stackCeiling = stackCeiling;
    stack->type = type;
    stack->currentAmount = 0;

    if (!type) {
        stack->array.arrayOfChar =
            (char *)malloc(stack->capacity * sizeof(char));
        if (!stack->array.arrayOfChar) {
            free(stack);
            return NULL;
        }
    } else {
        stack->array.arrayOfInt = (int *)malloc(stack->capacity * sizeof(int));
        if (!stack->array.arrayOfInt) {
            free(stack);
            return NULL;
        }
    }
    return stack;
}

int extendStack(struct Stack *stack, unsigned int cap) {
    if (cap > stack->stackCeiling)
        return -1;

    void *newArray;

    if (!stack->type) {
        newArray = realloc(stack->array.arrayOfChar, cap * sizeof(char));
    } else {
        newArray = realloc(stack->array.arrayOfInt, cap * sizeof(int));
    }

    if (!newArray) {
        return -1;
    }

    if (!stack->type) {
        stack->array.arrayOfChar = (char *)newArray;
    } else {
        stack->array.arrayOfInt = (int *)newArray;
    }

    stack->capacity = cap;
    return 0;
}

void freeStack(struct Stack *stack) {
    if (stack) {
        if (!stack->type)
            free(stack->array.arrayOfChar);
        else
            free(stack->array.arrayOfInt);

        free(stack);
    }
}

int isFull(struct Stack *stack) {
    return stack->top == (int)stack->capacity - 1;
}

int isEmpty(struct Stack *stack) { return stack->top == -1; }

void push(struct Stack *stack, int value) {
    if (isFull(stack)) {

        if (stack->capacity >= stack->stackCeiling) {
            printf("Stack ceiling reached at %d, cannot push %d\n",
                   stack->stackCeiling, value);
            return;
        }

        stack->currentAmount++;
        unsigned int newCap = stack->capacity * 2;

        if (newCap > stack->stackCeiling)
            newCap = stack->stackCeiling;

        if (extendStack(stack, newCap)) {
            printf("Failed to extend stack, cannot push\n");
            return;
        }
        printf("Stack extended to capacity: %d\n", stack->capacity);
    }

    if (!stack->type) {
        stack->array.arrayOfChar[++stack->top] = (char)value;
        printf("Pushed %c to stack\n", (char)value);
    } else {
        stack->array.arrayOfInt[++stack->top] = value;
        printf("Pushed %d to stack\n", value);
    }

    return;
}

int pop(struct Stack *stack) {
    if (isEmpty(stack)) {
        printf("Stack underflow\n");
        return INT_MIN;
    }

    int value;
    if (!stack->type) {
        value = (int)stack->array.arrayOfChar[stack->top];
        printf("Popping: %c from the stack\n", value);
    } else {
        value = stack->array.arrayOfInt[stack->top];
        printf("Popping: %d from the stack\n", value);
    }
    stack->currentAmount--;
    stack->top--;
    return value;
}

int peek(struct Stack *stack) {
    if (isEmpty(stack)) {
        printf("Stack is empty\n");
        return INT_MIN;
    }

    if (!stack->type)
        return (int)stack->array.arrayOfChar[stack->top];
    else
        return stack->array.arrayOfInt[stack->top];
}

int checkParen(struct Stack *st, char *string) {
    int size = strlen(string);
    if (!size)
        return -1;

    int count = 0;

    for (int i = 0; i < size; i++) {
        if (string[i] == '(' || string[i] == '{' || string[i] == '[') {
            if (count > (size - count)) {
                int position = size - count - st->currentAmount - 1;
                printf("Error at position %d\n", position + 1);
                printf("%s\n", string);
                for (int j = 0; j < position; j++)
                    printf(" ");
                printf("^ Extra opening bracket starts here\n");
                return 0;
            }

            push(st, string[i]);
            count++;
            printf("count %d\n", count);

        } else {
            if (isEmpty(st)) {
                printf("Error at position %d\n", i);
                printf("%s\n", string);
                for (int j = 0; j < i; j++)
                    printf(" ");
                printf("^ Extra closing bracket starts here\n");
                return 0;
            }

            int top = pop(st);

            if ((top == '(' && string[i] != ')') ||
                (top == '{' && string[i] != '}') ||
                (top == '[' && string[i] != ']')) {
                printf("%c and %c do not match at position %d in %s\n", top,
                       string[i], i, string);
                return 0;
            }
        }
    }
    return 1;
}

int main() {
    char *string = "{[{[[(())]]}]})";
    int len = strlen(string);

    struct Stack *st = createStack(20, 0);
    if (!st) {
        printf("Failed to create stack.\n");
        return 1;
    }

    (checkParen(st, string) == 1) ? printf("Parens match\n") : -1;

    freeStack(st);

    return 0;
}
