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
    int varCount;
    void push(Symbol *symbol); // string identifierName, int lineNumber, tokenIDs tokenID, int scopeLevel
    void pop();
    int find(string identifierName); // , int lineNumber, tokenIDs tokenID
    // void insert(string identifierName); 
    int verifyGlobal(Token *token);
    void printStack(vector<Symbol*> symbolList);
    int getScopeLevel();

    Symbol* createSymbol(Token *token);

  private:
    // vector<int*> varCounts; (?) (when popping "end", pop here as well. when pushing (begin), push here as well. Index should be level
    
    // bool searchName(string name);
};

#endif