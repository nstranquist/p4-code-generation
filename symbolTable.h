#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <vector>
#include "node.h"
#include "token.h"
#include "symbol.h"

using namespace std;

class SymbolTable {
  public:
    vector<Symbol*> localIdentifiers;
    vector<Symbol*> globalIdentifiers;
    vector<int> varCounts;
    int blockCount;
    bool isBeforeMain;
    bool newBlock;
    int varCount;
    int iter;
    void push(Symbol *symbol); // string identifierName, int lineNumber, tokenIDs tokenID, int scopeLevel
    void pushGlobal(Symbol *symbol); // string identifierName, int lineNumber, tokenIDs tokenID, int scopeLevel
    void pop();
    int find(string identifierName); // , int lineNumber, tokenIDs tokenID
    int findGlobal(string identifierName); // , int lineNumber, tokenIDs tokenID
    int verifyGlobal(Token *token);
    // void printStack(vector<Symbol*> symbolList);
    void printLocal();
    void printGlobal();
    // int getScopeLevel();
    int getLastVarCount();

    Symbol* createSymbol(Token *token);
    void printVarCounts();
    void printIdentifiers();

  private:
    // vector<int*> varCounts; (?) (when popping "end", pop here as well. when pushing (begin), push here as well. Index should be level
    
    // bool searchName(string name);
};

#endif