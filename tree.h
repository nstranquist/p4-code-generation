#ifndef TREE_H
#define TREE_H

#include "node.h"

using namespace std;

class ParseTree {
  public:
    ParseTree();

    Node *buildTree(string label);

    void printPreorder(Node *rootPtr, int level);
    void printToken(Token *token);
    void printNode(Node *node);
    Node *insertNode(string label); // Node *root
    Node *createNode(string label);
    void handleNewNode (string label); // string key, string line
    void addTokenToNode(Node *node, Token *token);

    bool areNodesEmpty(Node *node);
    bool areTokensEmpty(Node *node);

  private:
    Node *root; // Do we still need?

    // Node *searchNode(string word); // should return string maybe?

    // void addWordToNode(Node *temp, string word);
    // void splitByDelimiter(string delimiter, string line);
    // string getCharKey(string word);
    // bool checkDuplicateWord(Node *foundNode, string line);
    
};

#endif