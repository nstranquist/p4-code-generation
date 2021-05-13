#ifndef PRINTTREE_H
#define PRINTTREE_H

#include "node.h"
#include "symbolTable.h"

class PrintTree {
  public:
    void semanticAnalyze(Node *node);
    void scanPreorder(Node *node, int level);
    void printTree(Node *root);
    void printPreorder(Node *node, int level);

  private:
    SymbolTable symbolTable;
    int stackLevel;
};

#endif