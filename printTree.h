#ifndef PRINTTREE_H
#define PRINTTREE_H

#include <vector>
#include "node.h"
#include "symbolTable.h"
// #include "codeGenerator.h"

class PrintTree {
  public:
    ofstream out;
    int tempVarsCount;
    int tempLabelsCount;
    vector<string> globalValues;
    string lab1;

    void semanticAnalyze(Node *node, string outputFilename);
    void scanPreorder(Node *node, int level);
    void printTree(Node *root);
    void printPreorder(Node *node, int level);
    void printGlobalsToStorage();
    void printTempVarsToStorage();
    void printTempLabelsToStorage();
    string generateTempVar();
    string generateTempLabel();
    string getMostRecentTempVar();
    string getMostRecentTempLabel();
    void fillLabel(string label);

  private:
    SymbolTable symbolTable;
    // CodeGenerator codeGenerator;
    int stackLevel;
};

#endif