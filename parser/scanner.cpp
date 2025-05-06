#include "scanner.h"
#include "stringreader.h"
#include "token.h"
#include <unordered_map>
#include <vector>
#include <cctype>
std::unordered_map<std::string,TokenType> keywords;
void loadKeywords() {
    keywords["create"] = CREATE;
    keywords["table"] = TABLE;
    keywords["select"] = SELECT;
    keywords["insert"] = INSERT;
    keywords["into"] = INTO;
    keywords["values"] = VALUES;
    keywords["where"] = WHERE;
    keywords["from"] = FROM;
    keywords["order"] = ORDER;
    keywords["by"] = BY;
    keywords["asc"] = ASC;
    keywords["desc"] = DESC;
    keywords["and"] = AND;
    keywords["or"] = OR;
    keywords["not"] = NOT;
    keywords["txt"] = TXT;
    keywords["num"] = NUM;
    keywords["blob"] = BLOB;
}

std::vector<Token> Scanner(std::string filename) {
    loadKeywords();
    StringReader reader(filename);
    std::vector<Token> tokens;

    while (reader.hasMore()) {
        char current = reader.readNextChar();

        switch (current) {
            case ',':
             tokens.push_back(Token(COMMA));
            break;
            case '\'':
            tokens.push_back(Token((short)reader.readNextChar()));
            reader.goForward();
            break;
            case '+':
                tokens.push_back(Token(PLUS));
                break;

            case '-':
                tokens.push_back(Token(MINUS));
                break;

            case '*':
                tokens.push_back(Token(STAR));
                break;

            case '/': {
                    tokens.push_back(Token(SLASH));
                break;
            }

            case '=': {
                char next = reader.readNextChar();
                if (next == '=') {
                    tokens.push_back(Token(EQUALEQUAL));
                } else {
                    reader.goBack(); // Move back if it's just '='
                    tokens.push_back(Token(EQUAL));
                }
                break;
            }

            case '!': {
                char next = reader.readNextChar();
                if (next == '=') {
                    tokens.push_back(Token(BANGEQUAL));
                } else {
                    reader.goBack(); // Move back if it's just '!'
                    tokens.push_back(Token(BANG));
                }
                break;
            }



            case '(':
                tokens.push_back(Token(OPEN_PAREN));
                break;

            case ')':
                tokens.push_back(Token(CLOSE_PAREN));
                break;

            case ';':
                tokens.push_back(Token(SEMICOLON));
                break;

            case '<': {
                char next = reader.readNextChar();
               if (next == '=') {
                    tokens.push_back(Token(LESS_EQUAL));
                } else {
                    reader.goBack(); // Move back if it's just '<'
                    tokens.push_back(Token(LESS_THAN));
                }
                break;
            }
            case '>': {
                char next = reader.readNextChar();
           if (next == '=') {
                    tokens.push_back(Token(GREATER_EQUAL));

                } else {
                    reader.goBack(); // Move back if it's just '>'
                    tokens.push_back(Token(GREATER_THAN));
                }
                break;
            }
                case '"':{
                    char ch = reader.readNextChar();

                        do{
                        ch = reader.readNextChar();
                        tokens.push_back(ch);
                    }while(ch != '"');

                }
                break;
                default:
                    if (std::isdigit(current)) {
                        int number = current - '0';

                        while (reader.hasMore()) {
                            char next = reader.readNextChar();
                            if (!std::isdigit(next)) {
                                reader.goBack();
                                break;
                            }
                            number = number * 10 + (next - '0');
                        }

                        tokens.push_back(Token(number));
                    }

                    else if (std::isalpha(current) || current == '_') {
                        std::string identifier;
                        identifier += current;

                        while (reader.hasMore()) {
                            char next = reader.readNextChar();
                            if ((!std::isalnum(next)&&next!='*'&&next!=':'&&next!='.'&&next!='+'&&next!='?') && next != '_') {
                                reader.goBack();
                            break;
                            }
                            identifier += next;
                        }
                        //std::cout << "Adding string: "<<identifier << std::endl;
                        if (keywords.find(identifier) != keywords.end()) {
                            tokens.push_back(Token(keywords[identifier]));
                        } else {
                            tokens.push_back(Token(identifier));
                        }
                    }

                    // Optionally handle unknown/unexpected characters here
                    break;
    }
    }

    return tokens;
}
