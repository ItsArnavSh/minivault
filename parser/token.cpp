#include "token.h"


short counter = 0;  // Initialize counter

short mainLocation = -1;
Token::Token(TokenType type) {
    this->type = type;
    this->value = "";
}

Token::Token(std::string literal) {
    this->type = IDENTIFIER;
    this->value = literal;
}
Token::Token(int number) {
    this->type = NUMBER_LITERAL;
    this->value = number;
}

#include <iostream>
#include "token.h"

void Token::printToken() {
    std::cout << "Token Type: ";
    switch (type) {
        // Keywords
        case CREATE:         std::cout << "CREATE"; break;
        case TABLE:          std::cout << "TABLE"; break;
        case SELECT:         std::cout << "SELECT"; break;
        case INSERT:         std::cout << "INSERT"; break;
        case INTO:           std::cout << "INTO"; break;
        case VALUES:         std::cout << "VALUES"; break;
        case WHERE:          std::cout << "WHERE"; break;
        case FROM:           std::cout << "FROM"; break;
        case ORDER:          std::cout << "ORDER"; break;
        case BY:             std::cout << "BY"; break;
        case ASC:            std::cout << "ASC"; break;
        case DESC:           std::cout << "DESC"; break;

        // Symbols
        case OPEN_PAREN:     std::cout << "OPEN_PAREN ("; break;
        case CLOSE_PAREN:    std::cout << "CLOSE_PAREN )"; break;
        case COMMA:          std::cout << "COMMA (,)"; break;
        case SEMICOLON:      std::cout << "SEMICOLON (;)"; break;

        // Operators
        case EQUAL:          std::cout << "EQUAL (=)"; break;
        case EQUALEQUAL:     std::cout << "EQUALEQUAL (==)"; break;
        case NOT_EQUAL:      std::cout << "NOT_EQUAL (!=)"; break;
        case BANGEQUAL:      std::cout << "BANGEQUAL (!=)"; break;
        case BANG:           std::cout << "BANG (!)"; break;
        case LESS_THAN:      std::cout << "LESS_THAN (<)"; break;
        case GREATER_THAN:   std::cout << "GREATER_THAN (>)"; break;
        case LESS_EQUAL:     std::cout << "LESS_EQUAL (<=)"; break;
        case GREATER_EQUAL:  std::cout << "GREATER_EQUAL (>=)"; break;

        case PLUS:           std::cout << "PLUS (+)"; break;
        case MINUS:          std::cout << "MINUS (-)"; break;
        case STAR:           std::cout << "STAR (*)"; break;
        case SLASH:          std::cout << "SLASH (/)"; break;
        case PERCENT:        std::cout << "PERCENT (%)"; break;

        case AND:            std::cout << "AND"; break;
        case OR:             std::cout << "OR"; break;
        case NOT:            std::cout << "NOT"; break;

        // Identifiers
        case IDENTIFIER:
            std::cout << "IDENTIFIER, Value: " << value;
            break;

        // Literals
        case NUMBER_LITERAL:
            std::cout << "NUMBER_LITERAL, Value: " << value;
            break;
        case STRING_LITERAL:
            std::cout << "STRING_LITERAL, Value: \"" << value << "\"";
            break;

        default:
            std::cout << "UNKNOWN";
    }
    std::cout << std::endl;
}
