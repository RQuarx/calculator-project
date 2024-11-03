use std::io;
use std::char;
use std::f64;


fn is_operator(c: char) -> bool {
    matches!(c , '^' | '$' | '*' | '/' | '+' | '-' | '.')
}


fn is_constant(c: char) -> bool {
    matches!(c, 'p' | 'i' | 'e' | 't' | 'a' | 'u')
}


fn is_root(c: char) -> bool {
    matches!(c, 'r' | 'o' | 't')
}


fn precedence(operation: &str) -> i32 {
    match operation {
        "+" | "-" => 1,
        "*" | "/" => 2,
        "^" | "$" => 3,
        _ => 0
    }
}


fn perform_operation(a: f64, b: f64, operator: char) -> f64 {
    match operator {
        '^' => a.powf(b),
        '$' => b.powf(1.0 / a),
        '*' => a * b,
        '/' => a / b,
        '+' => a + b,
        '-' => a - b,
        _ => panic!("Unsupported operator"),
    }
}


fn handle_constants(input: &str, index: &mut usize, tokens: &mut Vec<String>) {
    let chars: Vec<char> = input.chars().collect();
    let size: usize = input.len();
    let mut _is_success: bool = false;
    let mut _add_index: usize = 0;

    if chars[*index] == 'e' {
        tokens.push(f64::consts::E.to_string());
        _is_success = true;
        _add_index = 1;
    } else if chars[*index] == 'p' && *index + 1 < size && chars[*index + 1] == 'i' {
        tokens.push(f64::consts::PI.to_string());
        _is_success = true;
        _add_index = 2;
    } else if chars[*index] == 't' && *index + 2 < size && chars[*index + 1] == 'a' && chars[*index + 2] == 'u' {
        tokens.push(f64::consts::TAU.to_string());
        _is_success = true;
        _add_index = 3;
    } else if chars[*index] == 'r' && *index + 3 < size && chars[*index + 1] == 'o' && chars[*index + 2] == 'o' && chars[*index + 3] == 't' {
        tokens.push("$".to_string());
        *index += 4;
        return;
    }

    if _is_success {
        if *index > 0 && chars[*index - 1].is_digit(10) {
            tokens.push("*".to_string());
        }

        *index += _add_index;
    }
}


fn tokenise(input_string: String) -> Vec<String> {
    let string_index: Vec<char> = input_string.chars().collect();
    let mut tokens: Vec<String> = Vec::new();
    let mut current_token: String = String::new();
    let mut i: usize = 0;

    while i < input_string.len() {
        let current_char: char = string_index[i];

        // ? Checks if current_char is - and if the - means negative
        if current_char == '-' && (i == 0 || input_string.chars().nth(i - 1).unwrap().is_digit(10)) {
            current_token.push(current_char);
            i += 1;
            continue;
        }

        // ? Checks if current_char is a digit
        if current_char.is_digit(10) || current_char == '.' {
            current_token.push(current_char);
            i += 1;
            continue;
        }

        // ? Checks for constants
        handle_constants(&input_string, &mut i, &mut tokens);

        // ? Checks if current_char is an operator
        if is_operator(current_char) {
            if !current_token.is_empty() {
                tokens.push(current_token);
                current_token = String::new();
            }

            tokens.push(current_char.to_string());
        }

        i += 1;
    }

    // ? Push unpushed tokens if it exist
    if !current_token.is_empty() {
        tokens.push(current_token);
    }

    tokens
}


/*
  * Applies the shunting yard algorithm to converts infix to postfix notation
  * source: https://en.wikipedia.org/wiki/Shunting_yard_algorithm#The_algorithm_in_detail
*/
fn to_postfix(tokens: Vec<String>) -> Vec<String> {
    let mut postfix_tokens: Vec<String> = Vec::new();
    let mut stack: Vec<String> = Vec::new();

    for token in tokens {
        if token.chars().nth(0).unwrap().is_digit(10) {
            postfix_tokens.push(token);
        } else if is_operator(token.chars().nth(0).unwrap()) {
            while !stack.is_empty() && precedence(stack.last().unwrap()) >= precedence(&token.clone()) {
                postfix_tokens.push(stack.pop().unwrap());
            }
            stack.push(token.clone());
        } else if token == "(" {
            stack.push(token.clone());
        } else if token == ")" {
            while !stack.is_empty() && stack.last().unwrap() != "(" {
                postfix_tokens.push(stack.pop().unwrap());
            }

            if !stack.is_empty() {stack.pop();}
        }
    }

    while !stack.is_empty() {
        postfix_tokens.push(stack.pop().unwrap());
    }

    postfix_tokens
}


fn eval_postfix(tokens: Vec<String>) -> f64 {
    let mut stack: Vec<f64> = Vec::new();

    for token in tokens {
        if token.chars().nth(0).unwrap().is_digit(10) {
            stack.push(token.parse::<f64>().unwrap());
        } else {
            let a: f64 = stack.pop().expect("ERR failed to pop!");
            let b: f64 = stack.pop().expect("ERR failed to pop!");
            stack.push(perform_operation(a, b, token.chars().next().unwrap()));
        }
    }

    stack.pop().expect("Invalid postfix operation!")
}


fn clean_string(input: String) -> String {
    input.chars()
        .filter(|&c| c.is_digit(10) || is_operator(c) || is_constant(c) || is_root(c) || c == '(' || c == ')')
        .collect()
}


fn main() {
    let mut input: String = String::new();
    io::stdin().read_line(&mut input).expect("Failed to read!");

    // ? Cleans the input string
    input = clean_string(input);

    // ? Tokenise and turn the infix into postfix
    println!("Result: {:.5}", eval_postfix(to_postfix(tokenise(input))));
}
