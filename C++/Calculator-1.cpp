/*
  * Calculator with PEMDAS and decimal operation.
*/
#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <string>
#include <iomanip>
#include <cctype>


bool StrIsDigit(std::string input) {
  if (input.empty()) {
    return false;
  }

  int dotCount = 0;

  for (char c : input) {
    if (c == '.') {
      dotCount++;

      if (dotCount > 1) {
        return false;
      }
    } else if (!isdigit(c) && c != '-') {
      return false;
    }
  }

  return true;
}


bool IsOperator(char inputChar = ' ') {
  return inputChar == '+' || inputChar == '-' || inputChar == '*' || inputChar == '/' || inputChar == '^' || inputChar == '.';
}


bool IsValid(char input) {
  return (
    isdigit(input) || IsOperator(input) ||
    input == 'p' || input == 'u' ||
    input == 'i' || input == 'e' ||
    input == 't' || input == 'a' ||
    input == '(' || input == ')'
  );
}


// * Removes whitespaces from a string
void CleanString(std::string &inputString) {
  inputString.erase(remove_if(inputString.begin(), inputString.end(), [](unsigned char x) { return !IsValid(x); }), inputString.end());
}


// * Tokenise the input string
void Tokenise(std::vector<std::string> &tokens, const std::string input) {
  std::string currentToken;

  for (int i = 0; i <= input.length(); ++i) {
    char currentChar = (i < input.length()) ? input[i] : '\0';

    // ? Checks if the - means substract or negative.
    if (currentChar == '-' && (!i || IsOperator(input[i - 1]))) {
      currentToken += currentChar;
      continue;
    }

    // ? Checks if current char is a digit or a '.'
    if (isdigit(currentChar) || currentChar == '.') {
      currentToken += currentChar;
      continue;
    }

    //? Checks if current char is pi, e, or tau
    if (currentChar == 'e') {
      tokens.push_back("2.7182818284");
      if (i && isdigit(input[i - 1])) tokens.push_back("*");
      continue;
    }

    if (input.substr(i, 2) == "pi") {
      tokens.push_back("3.1415926535");
      if (i && isdigit(input[i - 1])) tokens.push_back("*");
      i++;
      continue;
    }

    if (input.substr(i, 3) == "tau") {
      tokens.push_back("6.2831853071");
      if (i && isdigit(input[i - 1])) tokens.push_back("*");
      i += 2;
      continue;
    }

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
}


int Precedence(std::string operation) {
  if (operation == "+" || operation == "-") return 1;
  if (operation == "*" || operation == "/") return 2;
  if (operation == "^") return 3; // ? Assuming ^ has the highest precedence
  return 0; // ? Default for unsupported operators
}


/*
  * Applies the shunting yard algorithm to converts infix to postfix notation
  * https://en.wikipedia.org/wiki/Shunting_yard_algorithm#The_algorithm_in_detail
*/
void InfixToPostfix(std::vector<std::string> &input) {
  std::vector<std::string> output, stack;

  for (std::string token : input) {
    // ? If token is a digit, push it to output.
    if (StrIsDigit(token)) {
      output.push_back(token);
      continue;
    }

    // ? If token is operator, push it to stack
    if (IsOperator(token[0])) {      
      while (!stack.empty() && Precedence(stack.back()) >= Precedence(token)) {
        output.push_back(stack.back());
        stack.pop_back();
      }

      stack.push_back(token);
    }

    if (token == "(") {
      stack.push_back(token);
      continue;
    }

    if (token == ")") {
      while (!stack.empty() && stack.back() != "(") {
        output.push_back(stack.back());
        stack.pop_back();
      }
      // ? Pop the left parenthesis from the stack
      if (!stack.empty()) {
        stack.pop_back();
      }
    }
  }

  while (!stack.empty()) {
    output.push_back(stack.back());
    stack.pop_back();
  }

  input = output;
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
  }

  return -1;
}


double PostfixEvaluation(std::vector<std::string> tokens) {
  std::vector<double> stack;

  for (std::string token : tokens) {
    if (StrIsDigit(token)) {
      stack.push_back(stod(token));
      continue;
    }

    double num2 = stack.back(); stack.pop_back(); // ? Second operand
    double num1 = stack.back(); stack.pop_back(); // ? First operand
    double result = PerformOperation(num1, num2, token[0]);
    stack.push_back(result);
  }

  if (stack.size() != 1) {
    std::cerr << "Too many operands!" << '\n';
    return -1;
  }

  return stack.back();
}


void PrintVector(const std::vector<std::string> input) {
  int totalLen = 0;

  for (std::string s : input) {
    totalLen += s.length();
  }

  totalLen += (input.size() * 2);

  for (int i = input.size(); i > 2; --i) {
    if (i & 1) {
      totalLen += 2;
    }
  }

  std::cout << "\e[1;36m╭";

  for (int i = 0; i < totalLen; ++i) {
    std::cout << "─";
  }

  std::cout << "╮\n│";

  for (std::string s : input) {
    std::cout << "\e[1;37m " << s << " \e[1;36m│";
  }

  std::cout << "\n╰";

  for (int i = 0; i < totalLen; ++i) {
    std::cout << "─";
  }

  std::cout << "╯\n";
}


int main() {
  std::vector<std::string> tokens;
  std::string input;

  getline(std::cin, input);

  CleanString(input); // ? Cleans the string from any letters and whitespaces

  Tokenise(tokens, input); // ? Tokenize the input

  std::cout << "\n\e[1;36mPostfix:\n";
  PrintVector(tokens);

  InfixToPostfix(tokens); // ? Change infix notation to postfix

  std::cout << "\nInfix:\n";
  PrintVector(tokens); // ? Print the tokens

  std::cout << "\n\e[1;37mResult: " << std::setprecision(11) << PostfixEvaluation(tokens) << '\n'; // ? Print result
  return 0;
}