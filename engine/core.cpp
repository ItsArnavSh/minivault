#include "core.h"
#include "../aps/aps.h"
#include <iostream>
std::map<std::string, table> database;
std::vector<int> compressIndexes;
std::string processCommand(std::string command){
    std::vector<Token> tokens = Scanner(command);
    LOG_INFO("Tokens Generated");
    Parser p = Parser(tokens);
    Node* ast = p.parseStatement();
    LOG_INFO("AST Generated");
    return engine(ast);

}
std::vector<column> extractColumnsFrom(Node* node) {
    std::vector<column> cols;
    for (size_t i = 1; i  < node->children.size(); i ++) {
        std::string name = node->children[i]->children[0]->value;           // even: value
        TokenType type = node->children[i]->children[1]->type;


        if (type == BLOB) {
            compressIndexes.push_back(i-1);
        }

        cols.push_back({type, name});
    }


    std::cout << std::endl;

    return cols;
}

std::string engine(Node* root){
    //So now if Create Element is run, we will simply create a new table over here
    switch(root->type){
        case CREATE:{
        std::string name = root->children[0]->value;
        if(database.find(name)!=database.end()){
            return "Table with same name already exists";
        }
        std::vector<column> cols =  extractColumnsFrom(root);
        BPlusTree *tree = new BPlusTree();
        table newTable = {tree,cols};
        database[name] = newTable;
        return "Table "+name+" Created.";
        }
        break;
        case INSERT:{
            //Todo check if data is in correct order
            std::string tableName = root->children[0]->value;
            std::vector<Node*> cols = root->children[1]->children;
            std::vector<std::string> colVals;
            for (int i=0;i<cols.size();i++) {
                if(std::find(compressIndexes.begin(), compressIndexes.end(), i) != compressIndexes.end())
                    colVals.push_back(huffmanEncode(cols[i]->value));
                else
                    colVals.push_back(cols[i]->value);
            }
            std::string encodedStr = encode(colVals);

            if(database.find(tableName)!=database.end()){
            database[tableName].store->insert(colVals[0],  encodedStr);
            return "Data inserted Successfully!";
            }
            return "Column Does Not Exist";
        }
        break;
        case SELECT: {

            std::string table = root->children[0]->value;


            if (database.find(table) == database.end()) {
                break;
            }

            auto it = database[table].store->getIterator();
            std::vector<std::vector<std::string>> data;


            // Collect all the rows
            while (it.hasNext()) {
                auto [k, v] = it.next();
                try {
                    auto decoded = decode(v);
                    data.push_back(decoded);
                } catch (const std::exception& e) {
                    std::cerr << "[ERROR] Exception during decode(): " << e.what() << "\n";
                    break;
                }
            }

            int currentOrder = 1;

            Node* node = root->children[currentOrder];
            node = root->children[currentOrder];
            if (node->type == WHERE) {
                std::string colName = node->children[0]->value;
                std::string pattern = node->children[1]->value;


                size_t i = 0;
                bool columnFound = false;
                for (; i < database[table].columns.size(); ++i) {
                    if (database[table].columns[i].name == colName) {
                        columnFound = true;
                        break;
                    }
                }

                if (!columnFound) {
                    std::cerr << "[ERROR] Column '" << colName << "' not found in table '" << table << "'\n";
                } else {
                    if (pattern.rfind("regex:", 0) == 0) {
                        data = regexSearchInColumn(data, pattern.substr(6), i);
                    }
                    else{
                         data = kmpSearchInColumn(data, pattern, i);
                    }
                }

                currentOrder++;
            }

            if (node->type == ASC || node->type == DESC) {
                std::string value = node->children[0]->value;


                size_t i = 0;
                bool columnFound = false;
                for (; i < database[table].columns.size(); ++i) {
                    if (database[table].columns[i].name == value) {
                        columnFound = true;
                        break;
                    }
                }

                if (!columnFound) {
                    std::cerr << "[ERROR] Column '" << value << "' not found for sorting in table '" << table << "'\n";
                } else {
                    smartSortByColumn(data, i, node->type == ASC);
                }
            }
            //Here just before printing, we treat all the values once
            for(auto &row:data){
                for(auto index:compressIndexes)
                {
                    row[index] = huffmanDecode(row[index]);
                }
            }
            std::vector<std::string> colNames;
            for(auto col:database[table].columns){
                colNames.push_back(col.name);
            }
            try {
                return printer(data,colNames);
            } catch (const std::exception& e) {
                std::cerr << "[ERROR] Exception during printer(): " << e.what() << "\n";
                break;
            }

            break;
        }
        default:
        return "";
    }
    return "";
}
