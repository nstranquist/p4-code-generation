#ifndef PRINTTREE_H
#define PRINTTREE_H

#include "node.h"
#include "symbolTable.h"
// #include "codeGenerator.h"

class PrintTree {
  public:
    void semanticAnalyze(Node *node, string outputFilename);
    void scanPreorder(Node *node, int level);
    void printTree(Node *root);
    void printPreorder(Node *node, int level);

  private:
    SymbolTable symbolTable;
    // CodeGenerator codeGenerator;
    int stackLevel;
};

#endif