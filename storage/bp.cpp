#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

const int MAX_CHILDREN = 5;

struct Nodee {
    bool isLeaf;
    std::vector<std::pair<std::string, std::string>> kvPairs; // only for leaf nodes
    std::vector<std::string> keys; // only for internal nodes
    std::vector<Nodee*> children;

    Nodee(bool isLeaf) : isLeaf(isLeaf) {}
};
class BPlusTreeIterator {
public:
    BPlusTreeIterator(Nodee* root) {
        buildLeafList(root);
        leafIndex = 0;
        pairIndex = 0;
    }

    bool hasNext() const {
        return leafIndex < leaves.size() &&
               pairIndex < leaves[leafIndex]->kvPairs.size();
    }

    std::pair<std::string, std::string> next() {
        if (!hasNext()) {
            throw std::out_of_range("No more elements");
        }

        auto& kv = leaves[leafIndex]->kvPairs[pairIndex++];

        // Move to next leaf if current is exhausted
        while (leafIndex < leaves.size() && pairIndex >= leaves[leafIndex]->kvPairs.size()) {
            leafIndex++;
            pairIndex = 0;
        }

        return kv;
    }

private:
    std::vector<Nodee*> leaves;
    size_t leafIndex;
    size_t pairIndex;

    void buildLeafList(Nodee* node) {
        if (node->isLeaf) {
            leaves.push_back(node);
        } else {
            for (auto child : node->children) {
                buildLeafList(child);
            }
        }
    }
};

class BPlusTree {
public:
    BPlusTree() {
        root = new Nodee(true);
    }
    BPlusTreeIterator getIterator() {
        return BPlusTreeIterator(root);
    }

    void insert(const std::string& key, const std::string& value) {
        insertInternal(root, key, value);
    }

    std::string search(const std::string& key) {
        return searchInternal(root, key);
    }

private:
    Nodee* root;

    void insertInternal(Nodee* node, const std::string& key, const std::string& value) {
        if (node->isLeaf) {
            insertIntoLeaf(node, key, value);
        } else {
            int i = 0;
            while (i < node->keys.size() && key > node->keys[i]) {
                i++;
            }
            insertInternal(node->children[i], key, value);
        }
    }

    void insertIntoLeaf(Nodee* leaf, const std::string& key, const std::string& value) {
        auto it = std::lower_bound(
            leaf->kvPairs.begin(), leaf->kvPairs.end(), key,
            [](const std::pair<std::string, std::string>& p, const std::string& k) {
                return p.first < k;
            }
        );

        leaf->kvPairs.insert(it, {key, value});

        if (leaf->kvPairs.size() > MAX_CHILDREN - 1) {
            splitLeaf(leaf);
        }
    }

    void splitLeaf(Nodee* leaf) {
        int mid = MAX_CHILDREN / 2;
        Nodee* newLeaf = new Nodee(true);

        newLeaf->kvPairs.assign(leaf->kvPairs.begin() + mid, leaf->kvPairs.end());
        leaf->kvPairs.erase(leaf->kvPairs.begin() + mid, leaf->kvPairs.end());

        std::string splitKey = newLeaf->kvPairs[0].first;

        if (leaf == root) {
            Nodee* newRoot = new Nodee(false);
            newRoot->keys.push_back(splitKey);
            newRoot->children.push_back(leaf);
            newRoot->children.push_back(newLeaf);
            root = newRoot;
        } else {
            insertInternalNodee(findParent(root, leaf), splitKey, newLeaf);
        }
    }

    void insertInternalNodee(Nodee* parent, const std::string& key, Nodee* child) {
        auto it = std::lower_bound(parent->keys.begin(), parent->keys.end(), key);
        int index = it - parent->keys.begin();
        parent->keys.insert(it, key);
        parent->children.insert(parent->children.begin() + index + 1, child);

        if (parent->keys.size() > MAX_CHILDREN - 1) {
            splitInternalNodee(parent);
        }
    }

    void splitInternalNodee(Nodee* parent) {
        int mid = MAX_CHILDREN / 2;
        Nodee* newInternal = new Nodee(false);

        std::string midKey = parent->keys[mid];

        newInternal->keys.assign(parent->keys.begin() + mid + 1, parent->keys.end());
        newInternal->children.assign(parent->children.begin() + mid + 1, parent->children.end());

        parent->keys.erase(parent->keys.begin() + mid, parent->keys.end());
        parent->children.erase(parent->children.begin() + mid + 1, parent->children.end());

        if (parent == root) {
            Nodee* newRoot = new Nodee(false);
            newRoot->keys.push_back(midKey);
            newRoot->children.push_back(parent);
            newRoot->children.push_back(newInternal);
            root = newRoot;
        } else {
            insertInternalNodee(findParent(root, parent), midKey, newInternal);
        }
    }

    Nodee* findParent(Nodee* node, Nodee* child) {
        if (node->isLeaf) return nullptr;

        for (int i = 0; i < node->children.size(); ++i) {
            if (node->children[i] == child) return node;
            Nodee* res = findParent(node->children[i], child);
            if (res) return res;
        }
        return nullptr;
    }

    std::string searchInternal(Nodee* node, const std::string& key) {
        if (node->isLeaf) {
            for (const auto& p : node->kvPairs) {
                if (p.first == key) return p.second;
            }
            return "Key not found";
        } else {
            int i = 0;
            while (i < node->keys.size() && key > node->keys[i]) {
                i++;
            }
            return searchInternal(node->children[i], key);
        }
    }
};
