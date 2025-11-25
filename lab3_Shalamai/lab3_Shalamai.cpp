#include <iostream>
#include <string>
#include <cctype>
#include <stdexcept>


enum class TokenType {
    Number,
    Plus,
    Minus,
    Star,
    Slash,
    LParen,
    RParen,
    End
};

struct Token {
    TokenType type;
    int value;
};

class Lexer {
public:
    explicit Lexer(const std::string& input)
        : text(input), pos(0) {}

    Token getNextToken() {
        skipWhitespace();

        if (pos >= text.size()) {
            return { TokenType::End, 0 };
        }

        char c = text[pos];

        if (std::isdigit(static_cast<unsigned char>(c))) {
            int value = 0;
            while (pos < text.size()
                && std::isdigit(static_cast<unsigned char>(text[pos]))) {
                value = value * 10 + (text[pos] - '0');
                ++pos;
            }
            return { TokenType::Number, value };
        }

        ++pos;

        switch (c) {
        case '+': return { TokenType::Plus, 0 };
        case '-': return { TokenType::Minus, 0 };
        case '*': return { TokenType::Star, 0 };
        case '/': return { TokenType::Slash, 0 };
        case '(': return { TokenType::LParen, 0 };
        case ')': return { TokenType::RParen, 0 };
        default:
            throw std::runtime_error(std::string("Невідомий символ: '") + c + "'");
        }
    }

private:
    std::string text;
    std::size_t pos;

    void skipWhitespace() {
        while (pos < text.size()
            && std::isspace(static_cast<unsigned char>(text[pos]))) {
            ++pos;
        }
    }
};



class Parser {
public:
    explicit Parser(const std::string& input)
        : lexer(input) {
        current = lexer.getNextToken();
    }

    int parse() {
        int result = parseExpr();
        if (current.type != TokenType::End) {
            throw std::runtime_error("Unknown symbol");
        }
        return result;
    }

private:
    Lexer lexer;
    Token current;

    void eat(TokenType expected) {
        if (current.type == expected) {
            current = lexer.getNextToken();
        }
        else {
            throw std::runtime_error("Syntax error: unexpected token");
        }
    }

    int parseExpr() {
        int value = parseTerm();
        while (current.type == TokenType::Plus || current.type == TokenType::Minus) {
            TokenType op = current.type;
            eat(current.type);
            int right = parseTerm();
            if (op == TokenType::Plus) {
                value += right;
            }
            else {
                value -= right;
            }
        }
        return value;
    }

    int parseTerm() {
        int value = parseFactor();
        while (current.type == TokenType::Star || current.type == TokenType::Slash) {
            TokenType op = current.type;
            eat(current.type);
            int right = parseFactor();
            if (op == TokenType::Star) {
                value *= right;
            }
            else {
                if (right == 0) {
                    throw std::runtime_error("Division by zero");
                }
                value /= right;
            }
        }
        return value;
    }

    int parseFactor() {
        switch (current.type) {
        case TokenType::Number: {
            int v = current.value;
            eat(TokenType::Number);
            return v;
        }
        case TokenType::LParen: {
            eat(TokenType::LParen);
            int v = parseExpr();
            eat(TokenType::RParen);
            return v;
        }
        case TokenType::Minus: {
            eat(TokenType::Minus);
            int v = parseFactor();
            return -v;
        }
        default:
            throw std::runtime_error("Syntax error: operand expected");
        }
    }
};


int main() {
    std::cout << "Lab: syntax analyzer for arithmetic expressions\n"";
    std::cout << "Supported: +  -  *  /  parentheses () and unary minus.\n";
    std::cout << "Empty line = exit.\n\n";

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            break;
        }
        if (line.empty()) {
            break;
        }

        try {
            Parser parser(line);
            int result = parser.parse();
            std::cout << "Результат = " << result << "\n\n";
        }
        catch (const std::exception& ex) {
            std::cout << "Помилка: " << ex.what() << "\n\n";
        }
    }

    return 0;
}
