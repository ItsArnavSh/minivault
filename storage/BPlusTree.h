#ifndef BPLUS_TREE_H
#define BPLUS_TREE_H

#include <string>
#include <vector>

const int MAX_CHILDREN = 5;

// Forward declaration
class BPlusTreeIterator;

struct Nodee {
    bool isLeaf;
    std::vector<std::pair<std::string, std::string>> kvPairs; // only for leaf nodes
    std::vector<std::string> keys; // only for internal nodes
    std::vector<Nodee*> children;

    Nodee(bool isLeaf);
};

class BPlusTreeIterator {
public:
    BPlusTreeIterator(Nodee* root);
    bool hasNext() const;
    std::pair<std::string, std::string> next();

private:
    std::vector<Nodee*> leaves;
    size_t leafIndex;
    size_t pairIndex;

    void buildLeafList(Nodee* node);
};

class BPlusTree {
public:
    BPlusTree();
    BPlusTreeIterator getIterator();
    void insert(const std::string& key, const std::string& value);
    std::string search(const std::string& key);

private:
    Nodee* root;

    void insertInternal(Nodee* node, const std::string& key, const std::string& value);
    void insertIntoLeaf(Nodee* leaf, const std::string& key, const std::string& value);
    void splitLeaf(Nodee* leaf);
    void insertInternalNodee(Nodee* parent, const std::string& key, Nodee* child);
    void splitInternalNodee(Nodee* parent);
    Nodee* findParent(Nodee* node, Nodee* child);
    std::string searchInternal(Nodee* node, const std::string& key);
};

#endif // BPLUS_TREE_H
