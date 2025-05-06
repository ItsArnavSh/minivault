#ifndef CORE_H
#define CORE_H
    #include <string>
    #include <map>
#include <unordered_map>
    #include"../storage/bp.cpp"
    #include"../parser/scanner.h"
    #include"../parser/parser.h"
    #include"../utils/log.h"
    #include"../serde/serde.h"
    #include "../utils/printer.h"
    std::string processCommand(std::string command);
    std::string engine(Node* root);

    typedef struct column{
        TokenType type;
        std::string name;
    }column;
    typedef struct table{
        BPlusTree* store;
        std::vector<column> columns;
    }table;
    extern std::map<std::string, table> database;
    extern std::vector<int> compressIndexes;
#endif
