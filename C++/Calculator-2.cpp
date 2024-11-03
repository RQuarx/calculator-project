#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <cmath>
#include <stdio.h>


// ? Functions that is used to make other functions function functionally
bool isoperator(const char inputChar) {
  return (inputChar == '+' || inputChar == '-' || inputChar == '*' || inputChar == '/' || inputChar == '^' || inputChar == '.' || inputChar == '$');
}


bool isconstant(const char inputChar) {
  return (inputChar == 'p' || inputChar == 'i' || inputChar == 'e' || inputChar == 't' || inputChar == 'a' || inputChar == 'u');
}


bool isroot(const char inputChar) {
  return (inputChar == 'r' || inputChar == 'o' || inputChar == 'o' || inputChar == 't');
}


int Precedence(std::string operation) {
  if (operation == "+" || operation == "-") return 1;
  if (operation == "*" || operation == "/") return 2;
  if (operation == "^" || operation == "$") return 3;
  return 0; // ? Default for unsupported operators
}


double PerformOperation(double a, double b, char operation) {
  switch (operation) {
    case '*':
      return a * b;
    case '/':
      if (!b) {
        std::cerr << "Division by zero!" << '\n';
        break;
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

/*
  * Checks if there are any constants in the given string
*/
void CheckConstants(std::vector<std::string> &tokens, int &index, const std::string str) {
  bool isSuccess = false;
  int addLen;


  if (str[index] == 'e') {
    tokens.push_back(std::to_string(M_E));
    isSuccess = true;
    addLen = 0;
  } else if (str.substr(index, 2) == "pi") {
    tokens.push_back(std::to_string(M_PI));
    isSuccess = true;
    addLen = 1;
  } else if (str.substr(index, 3) == "tau") {
    tokens.push_back(std::to_string(M_PI * 2.0000000000));
    isSuccess = true;
    addLen = 2;
  } else if (str.substr(index, 4) == "root") {
    tokens.push_back("$"); // ? $ is a character that is used to replace "sqrt"
    index += 3;
    return;
  }

  if (isSuccess && index && isdigit(str[index - 1])) {
    tokens.push_back("*");
    index += addLen;
  }
}


/*
  * Tokenise inputString
  * 
  * @param inputTokens The token vector that is going to be filled with the tokens
  * @param inputString The string that is going to be vectorised
*/
void Tokenise(std::vector<std::string> &inputTokens, const std::string inputString) {
  std::vector<std::string> tokens;
  std::string currentToken;

  for (int i = 0; i <= inputString.length(); ++i) {
    char currentChar = (i < inputString.length()) ? inputString[i] : '\0';

    // ? Checks if the - means substract or negative.
    if (currentChar == '-' && (!i || isoperator(inputString[i - 1]))) {
      currentToken += currentChar;
      continue;
    }

    // ? Checks if current char is a digit or a '.'
    if (isdigit(currentChar) || currentChar == '.') {
      currentToken += currentChar;
      continue;
    }

    //? Checks if current char is pi, e, tau, or a square root
    CheckConstants(tokens, i, inputString);

    // ? If current char is an operator, then it means that the previous digits are complete
    if (!currentToken.empty()) {
      tokens.push_back(currentToken); 
      currentToken.clear(); 
    }

    // ? Avoid pushing an empty token for the end of the string
    if (currentChar != '\0' && !isalpha(currentChar)) {
      tokens.push_back(std::string(1, currentChar)); 
    }
  }

  inputTokens = tokens;
}


/*
  * Applies the shunting yard algorithm to converts infix to postfix notation
  * source: https://en.wikipedia.org/wiki/Shunting_yard_algorithm#The_algorithm_in_detail
*/
void ToPostfix(std::vector<std::string> &tokens) {
  std::vector<std::string> postfixOutput;
  std::stack<std::string> stack;

  for (const std::string token : tokens) {
    if (isdigit(token[0])) {
      postfixOutput.push_back(token);
    } else if (isoperator(token[0])) {
      while (!stack.empty() && Precedence(stack.top()) >= Precedence(token)) {
        postfixOutput.push_back(stack.top());
        stack.pop();
      }
      stack.push(token);
    } else if (token == "(") {
      stack.push(token);
    } else if (token == ")") {
      while (!stack.empty() && stack.top() != "(") {
        postfixOutput.push_back(stack.top());
        stack.pop();
      }
      if (!stack.empty()) stack.pop();
    }
  }

  while (!stack.empty()) {
    postfixOutput.push_back(stack.top());
    stack.pop();
  }

  tokens = postfixOutput;
}


/*
  * Perform operations / evaluate the answer from the postfix tokens
*/
double EvaluatePostfix(std::vector<std::string> tokens) {
  std::stack<double> stack;

  for (const std::string token : tokens) {
    if (isdigit(token[0])) {
      stack.push(std::stod(token));
      continue;
    }

    double b = stack.top(); stack.pop();
    double a = stack.top(); stack.pop();
    stack.push(PerformOperation(a, b, token[0]));
  }

  if (stack.size() != 1) {
    exit(1);
  }

  return stack.top();
}


/*
  * Cleans inputString from whitespaces and unused stuff/
*/
void CleanString(std::string &inputString) {
  int index = 0;

  for (const char c : inputString) {
    if (isdigit(c) || isoperator(c) || isconstant(c) || isroot(c) || c == '(' || c == ')') {
      inputString[index++] = c;
    }
  }

  inputString.resize(index);
}


int main() {
  std::string input;
  getline(std::cin, input);

  // * Cleans the string

  CleanString(input);

  // * Tokenise the string

  std::vector<std::string> tokens;
  Tokenise(tokens, input);

  // * Turning the infix tokens into postfix

  ToPostfix(tokens);

  for (auto token : tokens) {
    std::cout << token << ", ";
  }

  printf("Result: %.5f\n", EvaluatePostfix(tokens));

  return 0;
}