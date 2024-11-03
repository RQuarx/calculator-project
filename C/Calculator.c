#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define PI 3.14159265358979323846
#define E 2.7182818284590452354
#define TAU (PI * 2)
#define INIT_CAPACITY 3
#define MAX_SIZE 251


/*
  * Struct for the tokens
*/
typedef struct {
  char **tokens;
  size_t size;
  size_t capacity;
} vector;

void InitToken(vector *list) {
  list->size = 0;
  list->capacity = INIT_CAPACITY;
  list->tokens = (char**)malloc(sizeof(char*) * list->capacity);
}

void AddToken(vector *list, const char *token) {
  if (list->size >= list->capacity) {
    list->capacity *= 2;
    list->tokens = (char**)realloc(list->tokens, sizeof(char*) * list->capacity);
  }

  list->tokens[list->size] = strdup(token);
  list->size++;
}

void FreeToken(vector *list) {
  for (int i = 0; i < list->size; ++i) {
    free(list->tokens[i]);
  }
  free(list->tokens);
}

/*
  * Struct for a stack (char*)
*/
typedef struct {
  char **data;
  int top;
  size_t capacity;
} Stack;

Stack *InitStack(void) {
  Stack *stack = (Stack*)malloc(sizeof(Stack));
  stack->capacity = INIT_CAPACITY;
  stack->data = (char**)malloc(sizeof(char*) * stack->capacity);
  stack->top = -1;
  return stack;
}

int isEmpty(Stack *stack) {
  return (stack->top == -1);
}

void push(Stack *stack, const char *inputString) {
  if (stack->top == stack->capacity - 1) {
    stack->capacity *= 2;
    stack->data = (char**)realloc(stack->data, sizeof(char*) * stack->capacity);
  }
  stack->data[++stack->top] = strdup(inputString);
}

char *pop(Stack *stack) {
  return stack->data[stack->top--];
}

char *top(Stack *stack) {
  return stack->data[stack->top];
}

/*
  * Struct for a stack (double*)
*/
typedef struct {
  double *data;
  int top;
  size_t capacity;
} StackD;

StackD *InitStackD(void) {
  StackD *stack = (StackD*)malloc(sizeof(StackD));
  stack->capacity = INIT_CAPACITY;
  stack->data = (double*)malloc(sizeof(double) * stack->capacity);
  stack->top = -1;
  return stack;
}

void pushD(StackD *stack, double value) {
  if (stack->top == stack->capacity - 1) {
    stack->capacity *= 2;
    stack->data = (double*)realloc(stack->data, sizeof(double) * stack->capacity);
  }
  stack->data[++stack->top] = value;
}

double popD(StackD *stack) {
  return stack->data[stack->top--];
}

double topD(StackD *stack) {
  return stack->data[stack->top];
}


// ? Functions that is used to make other functions function functionally

// * Checks if inputChar is an operator
bool isoperator(const char inputChar) {
  return (inputChar == '+' || inputChar == '-' || inputChar == '*' || inputChar == '/' || inputChar == '^' || inputChar == '.' || inputChar == '$');
}

// * Checks if inputChar is part of a character in a constant name
bool isconstant(const char inputChar) {
  return (inputChar == 'p' || inputChar == 'i' || inputChar == 'e' || inputChar == 't' || inputChar == 'a' || inputChar == 'u');
}

// * Checks if inputChar is part of a character in the word root
bool isroot(const char inputChar) {
  return (inputChar == 'r' || inputChar == 'o' || inputChar == 't');
}

// * Checks if the string starting from index is a constant or a sqrt
void CheckConstant(vector *tokens, int *index, const char *inputString) {
  int addLen;
  bool isSuccess = 0;
  char buffer[15];

  if (!strncmp(&inputString[*index], "e", 1)) {
    snprintf(buffer, sizeof(buffer), "%.7f", E);
    AddToken(tokens, buffer);
    isSuccess = 1;
  } else if (!strncmp(&inputString[*index], "pi", 2)) {
    snprintf(buffer, sizeof(buffer), "%.7f", PI);
    AddToken(tokens, buffer);
    isSuccess = 1;
    addLen = 1;
  } else if (!strncmp(&inputString[*index], "tau", 3)) {
    snprintf(buffer, sizeof(buffer), "%.7f", TAU);
    AddToken(tokens, buffer);
    isSuccess = 1;
    addLen = 2;
  } else if (!strncmp(&inputString[*index], "root", 4)) {
    AddToken(tokens, "$");
    (*index) += 3;
    return;
  }

  if (isSuccess) {
    if (*index > 0 && isdigit(inputString[*index - 1])) {
      AddToken(tokens, "*");
    }

    (*index) += addLen;
  }
}

int Precedence(const char *operation) {
  if (!strcmp(operation, "+") || !strcmp(operation, "-")) return 1;
  if (!strcmp(operation, "*") || !strcmp(operation, "/")) return 2;
  if (!strcmp(operation, "^") || !strcmp(operation, "$")) return 3;
  return 0; // ? Default for unsupported operators

}

double PerformOperation(double a, double b, char operation) {
  switch (operation) {
    case '*':
      return a * b;
    case '/':
      if (!b) {
        fprintf(stderr, "Division by zero!");
        exit(EXIT_FAILURE);
      }
      return a / b;
    case '+':
      return a + b;
    case '-':
      return a - b;
    case '^':
      return pow(a, b);
    case '$':
      return pow(b, (1 / a));
  }

  return -1;
}


// ? Main functions

// * Tokenise the inputString
void Tokenise(vector *inputTokens, const char *inputString) {
  InitToken(inputTokens);
  char currentToken[MAX_SIZE];
  int currentTokenLen = 0;

  for (int i = 0; i <= strlen(inputString); ++i) {
    char currentChar = (i < strlen(inputString) ? inputString[i] : '\0');

    // * Checks for '-' and if the '-' is a negative sign
    if (currentChar == '-' && (!i || isoperator(inputString[i - 1]) || inputString[i - 1] == '(')) {
      continue;
      currentToken[currentTokenLen++] = currentChar;
    }
    // * Checks if current char is a digit or a '.'
    if (isdigit(currentChar) || currentChar == '.') {
      currentToken[currentTokenLen++] = currentChar;
      continue;
    }

    // * Checks if current char is pi, e, tau, or a square root
    CheckConstant(inputTokens, (int*)&i, inputString);

    // * If current char is an operator, then it means that the previous digits are complete
    if (currentTokenLen > 0) {
      currentToken[currentTokenLen] = '\0';
      AddToken(inputTokens, currentToken);
      currentTokenLen = 0;
    }

    // * Avoid pushing an empty token for the end of the string
    if (currentChar != '\0' && !isalpha(currentChar)) {
      char operatorToken[2] = {currentChar, '\0'};
      AddToken(inputTokens, operatorToken);
    }
  }
}

/*
  * Applies the shunting yard algorithm to converts infix to postfix notation
  * source: https://en.wikipedia.org/wiki/Shunting_yard_algorithm#The_algorithm_in_detail
*/
void ToPostfix(vector *tokens) {
  vector postfix;
  InitToken(&postfix);
  Stack *stack = InitStack();

  for (int i = 0; i < tokens->size; ++i) {
    const char *token = tokens->tokens[i];

    if (isdigit(token[0])) {
      AddToken(&postfix, token);
    } else if (isoperator(token[0])) {
      while (!isEmpty(stack) && Precedence(top(stack)) >= Precedence(token)) {
        AddToken(&postfix, top(stack));
        free(pop(stack));
      }

      push(stack, token);
    } else if (!strcmp(token, "(")) {
      push(stack, token);
    } else if (!strcmp(token, ")")) {
      while (!isEmpty(stack) && strcmp(top(stack), "(")) {
        AddToken(&postfix, top(stack));
        free(pop(stack));
      }

      if (!isEmpty(stack)) { free(pop(stack)); }
    }
  }

  while (!isEmpty(stack)) {
    AddToken(&postfix, top(stack));
    free(pop(stack));
  }

  free(stack);
  FreeToken(tokens);
  *tokens = postfix;
}


double EvaluatePostfix(vector *tokens) {
  StackD *stack = InitStackD();

  for (int i = 0; i < tokens->size; ++i) {
    char *token = tokens->tokens[i];

    if (isdigit(token[0])) {
      pushD(stack, atof(token));
      continue;
    }

    double b = popD(stack);
    double a = popD(stack);
    pushD(stack, PerformOperation(a, b, token[0]));
  }

  if (stack->top != 0) {
    fprintf(stderr, "Too many operators!\n");
    exit(EXIT_FAILURE);
  }

  double result = topD(stack);
  FreeToken(tokens);
  free(stack);
  return result;
}


// * Cleans inputString from any whitespaces and unused characters
char *CleanString(char *inputString) {
  int index = 0;

  for (int i = 0; i < strlen(inputString); ++i) {
    const char c = inputString[i];
    
    if (isdigit(c) || isoperator(c) || isconstant(c) || isroot(c) || c == '(' || c == ')') {
      inputString[index++] = c;
    }
  }

  inputString[index] = '\0';

  char *returnString = (char*)realloc(inputString, (sizeof(char)) * (index + 1));
  return returnString ? returnString : inputString;
}


int main() {
  char *input = (char*)malloc((sizeof(char)) * MAX_SIZE);
  printf("Input: ");
  scanf("%250[^\n]", input);

  // * Cleans the string
  input = CleanString(input);

  // * Tokenise the string
  vector tokens;
  Tokenise(&tokens, input);

  free(input);

  // * Turns the infix operation into postfix
  ToPostfix(&tokens);

  printf("Result: %.5f\n", EvaluatePostfix(&tokens));
  return 0;
}