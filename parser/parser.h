#ifndef  PARSER_H
#define PARSER_H
#include "token.h"
#include "error.h"
#include "../utils/log.h"
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>
class Node {
public:
    TokenType type;
    std::string value;
    std::vector<Node*> children;

    // Constructor for non-terminal nodes
    Node(TokenType type) : type(type) {}

    // Constructor for terminal nodes (tokens)
    Node(TokenType type, Token* token) : type(type), value(token->value) {}

    void addChild(Node* child);
    void printTree(int indent);
};

class Parser{
    public:
    std::vector<Token> tokens;
    uint current = 0;
    Parser(std::vector<Token> tokens): tokens(tokens){};
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& errorMessage);
    Token peek();
    Node* parseStatement();
    Node* parseCreate();
    //Node* parseColumns();
    Node* parseInsert();
    Node* parseSelect();
    Node* parseExpression();
    Node* parseCompareSections();
    Node* parseEquality();
    Node* parseComparison();
    Node* parseTerm();
    Node* parseFactor();
    Node* parseUnary();
    Node* parsePrimary();
};
#endif
