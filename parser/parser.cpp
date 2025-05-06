#include "parser.h"
#include "token.h"
#include <cstdlib>
#include <unordered_set>
std::unordered_set<TokenType> allowedMajors;

void Node::addChild(Node* child) {
    children.push_back(child);
}

// Parser methods
bool Parser::match(TokenType type) {
    return current < tokens.size() && tokens[current].type == type;
}

Token Parser::consume(TokenType type, const std::string& errorMessage) {

    if (match(type)) return tokens[current++];
    LOG_ERROR(errorMessage);
    //<<"minisql ERROR: " << errorMessage<<std::endl;
    exit(0);
    return Token();
}

Token Parser::peek() {
    return tokens[current];
}
Node* Parser::parseStatement(){
    LOG_DEBUG("Parsing Statement");
    switch(peek().type){
        case CREATE:
        consume(CREATE, "create|insert|select expected at the start");
        return parseCreate();
        break;
        case INSERT:
        consume(INSERT, "create|insert|select expected at the start");
        return parseInsert();
        break;
        case SELECT:
        consume(SELECT, "");
        return parseSelect();
        break;
        default:
        consume(CREATE,"create|insert|select expected at the start");
        exit(1);
    }
    return nullptr;
}
Node* Parser::parseCreate(){
    LOG_DEBUG("Parsing Create");
    consume(TABLE, "Expected Table");
    Token tableName = consume(IDENTIFIER, "Expected table name after TABLE");


    consume(OPEN_PAREN, "Expected '(' after table name");
    Node* root = new Node(CREATE); // Root node for CREATE statement
    root->addChild(new Node(IDENTIFIER, &tableName)); // Table name node

    int columnCount = 0;
    // << "Starting column parsing..." << std::endl;

    while (true) {
        columnCount++;
        // << "Parsing column #" << columnCount << std::endl;

        Token columnName = consume(IDENTIFIER, "Expected column name");

        Node* columnNode = new Node(COLUMN_DEF);
        // << "  Adding column node to root (first time)" << std::endl;

        columnNode->addChild(new Node(IDENTIFIER, &columnName));
        // << "  Added identifier child to column node" << std::endl;

        Token typeToken = consume(peek().type, "Expected type after column name");

        switch (typeToken.type) {
            case NUM:
                columnNode->addChild(new Node(NUM, &typeToken));
                // << "  Added NUM type to column node" << std::endl;
                break;
            case TXT:
                columnNode->addChild(new Node(TXT, &typeToken));
                // << "  Added TXT type to column node" << std::endl;
                break;
            case BLOB:
                columnNode->addChild(new Node(BLOB, &typeToken));
                // << "  Added BLOB type to column node" << std::endl;
                break;
            default:
                LOG_ERROR("Unknown column type");
                exit(1);
        }

        // << "  Adding column node to root " << std::endl;
        root->addChild(columnNode);

        // Check for comma or end
        if (peek().type == COMMA) {
            // << "  Found comma, continuing to next column" << std::endl;
            consume(COMMA, "Expected ','");
        } else if (peek().type == CLOSE_PAREN) {
            // << "  Found closing parenthesis, ending column definitions" << std::endl;
            consume(CLOSE_PAREN, "Expected ')'");
            break;
        } else {
            LOG_ERROR("Expected ',' or ')' after column definition");
            exit(1);
        }
    }

    // << "Finished parsing CREATE statement" << std::endl;
    // << "Root node has " << root->children.size() << " children" << std::endl;

    return root;
}
Node* Parser::parseInsert(){
    Node* root = new Node(INSERT);
    Token tableName = consume(IDENTIFIER, "Expected Table Name");
    Node* tableNode = new Node(IDENTIFIER);
    root->addChild(new Node(IDENTIFIER,&tableName));
    root->addChild(tableNode);
    consume(OPEN_PAREN, "Expected '(' after table name");
    while(true){
        if(match(IDENTIFIER)|| match(NUMBER_LITERAL)){
            Token typeToken = consume(peek().type, "Expected type after column name");
            Node* node = new Node(peek().type,&typeToken);
            tableNode->addChild(node);
        }
        else{
            peek().printToken();
            consume(STRING_LITERAL,"Literal Expected in contents");
        }
        if(peek().type==COMMA){
            consume(peek().type,"Comma Expected");
        }
        else if(peek().type==CLOSE_PAREN){
            consume(peek().type,"Close Paren Expected");
            break;
        }
    }
    return root;
}
Node* Parser::parseSelect() {
    LOG_DEBUG("Parsing Select");
    Node* root = new Node(SELECT);

    consume(FROM, "Expected 'FROM' after SELECT column list");
    Token tableName = consume(IDENTIFIER, "Expected table name after FROM");
    root->addChild(new Node(FROM, &tableName));

    // Optional ASC/DESC
    if (peek().type==WHERE) {
        Token orderToken = consume(peek().type, "Expected WHERE");
        consume(OPEN_PAREN, "Expected '(' after WHERE");
        Token column = consume(IDENTIFIER, "Expected string in WHERE");
        consume(LESS_THAN,"Expected < after column Name");//where(id<hell)
        Token pattern = consume(IDENTIFIER, "Expected pattern");
        consume(CLOSE_PAREN, "Expected ')' after pattern");

        Node* orderNode = new Node(orderToken.type);
        orderNode->addChild(new Node(IDENTIFIER, &column));
        orderNode->addChild(new Node(IDENTIFIER, &pattern));
        root->addChild(orderNode);
    }

    // Optional ASC/DESC
    if (peek().type == ASC || peek().type == DESC) {
        Token orderToken = consume(peek().type, "Expected ASC or DESC");
        consume(OPEN_PAREN, "Expected '(' after ASC/DESC");
        Token column = consume(IDENTIFIER, "Expected column name in ASC/DESC");
        consume(CLOSE_PAREN, "Expected ')' after column name in ASC/DESC");

        Node* orderNode = new Node(orderToken.type);
        orderNode->addChild(new Node(IDENTIFIER, &column));
        root->addChild(orderNode);
    }

    return root;
}

Node* Parser::parseExpression() {
    return parseCompareSections();
}

Node* Parser::parseCompareSections() {
    Node* left = parseEquality();
    while (match(AND) || match(OR)) {
        Token operatorToken = tokens[current++];
        Node* right = parseEquality();
        Node* logicalNode = new Node(operatorToken.type); // AND or OR
        logicalNode->addChild(left);
        logicalNode->addChild(right);
        left = logicalNode;
    }
    return left;
}

Node* Parser::parseEquality() {
    Node* left = parseComparison();
    while (match(EQUALEQUAL) || match(NOT_EQUAL)) {
        Token operatorToken = tokens[current++];
        Node* right = parseComparison();
        Node* equalityNode = new Node(operatorToken.type); // == or !=
        equalityNode->addChild(left);
        equalityNode->addChild(right);
        left = equalityNode;
    }
    return left;
}

Node* Parser::parseComparison() {
    Node* left = parseTerm();
    while (match(GREATER_THAN) || match(GREATER_EQUAL) ||
           match(LESS_THAN) || match(LESS_EQUAL)) {
        Token operatorToken = tokens[current++];
        Node* right = parseTerm();
        Node* comparisonNode = new Node(operatorToken.type);
        comparisonNode->addChild(left);
        comparisonNode->addChild(right);
        left = comparisonNode;
    }
    return left;
}

Node* Parser::parseTerm() {
    Node* left = parseFactor();
    while (match(PLUS) || match(MINUS)) {
        Token operatorToken = tokens[current++];
        Node* right = parseFactor();
        Node* termNode = new Node(operatorToken.type);
        termNode->addChild(left);
        termNode->addChild(right);
        left = termNode;
    }
    return left;
}

Node* Parser::parseFactor() {
    Node* left = parseUnary();
    while (match(STAR) || match(SLASH) || match(PERCENT)) {
        Token operatorToken = tokens[current++];
        Node* right = parseUnary();
        Node* factorNode = new Node(operatorToken.type);
        factorNode->addChild(left);
        factorNode->addChild(right);
        left = factorNode;
    }
    return left;
}

Node* Parser::parseUnary() {
    if (match(NOT) || match(MINUS)) {
        Token operatorToken = tokens[current++];
        Node* right = parseUnary();
        Node* unaryNode = new Node(operatorToken.type);
        unaryNode->addChild(right);
        return unaryNode;
    }
    return parsePrimary();
}

Node* Parser::parsePrimary() {

    if (match(NUMBER_LITERAL)) {
        Token number = consume(NUMBER_LITERAL, "Expected number literal.");
        return new Node(NUMBER_LITERAL, &number);
    }

    if (match(STRING_LITERAL)) {
        Token str = consume(STRING_LITERAL, "Expected string literal.");
        return new Node(STRING_LITERAL, &str);
    }

    if (match(IDENTIFIER)) {
        Token id = consume(IDENTIFIER, "Expected identifier.");
        return new Node(IDENTIFIER, &id);
    }

    if (match(OPEN_PAREN)) {
        consume(OPEN_PAREN, "Expected '(' to begin subexpression.");
        Node* expr = parseExpression();
        consume(CLOSE_PAREN, "Expected ')' after subexpression.");
        return expr;
    }

    return nullptr;
}
