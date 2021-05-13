#include <stdio.h>
#include <iostream>
#include "symbolTable.h"

using namespace std;

void SymbolTable::push(Symbol *symbol) { // string identifierName, int lineNumber, tokenIDs tokenID, int scopeLevel
cout << "pushing new symbol" << endl;
  Symbol *newSymbol = new Symbol();

  newSymbol->lineNumber = symbol->lineNumber;
  newSymbol->identifierName = symbol->identifierName;
  newSymbol->tokenID = symbol->tokenID;
  newSymbol->scopeLevel = this->getScopeLevel();

  this->localIdentifiers.push_back(newSymbol);
}

void SymbolTable::pop() {
  // remove "top-most" element from stack
  this->localIdentifiers.pop_back();
}

int SymbolTable::find(string identifierName) {
  int distanceFromTop = -1;

  // Iterate over all vars in the Symbol Table
  for(vector<Symbol*>::iterator t = this->localIdentifiers.begin(); t!= this->localIdentifiers.end(); ++t) {
    if(identifierName == (*t)->identifierName) {
      cout << "found matching global variable for token with instance: " << (*t)->identifierName << endl;
      distanceFromTop = (*t)->scopeLevel;
      return distanceFromTop;
    }
  }

    // find first occurrence of the argument on the stack (top to bottom)

    // return the distance from the top of stack, where 0 is top of stack, -1 is not found

  return distanceFromTop;
}

// int SymbolTable::verifyGlobal(Token *token) {
//   int found = 0;
//   // find tokenInstance in globals. iterate
//   size_t i = 0;
//   for(vector<Symbol*>::iterator t = this->globalIdentifiers.begin(); t != this->globalIdentifiers.end(); ++t) {
//     if(token->tokenInstance == (*t)->identifierName) {
//       cout << "found matching global variable for token with instance: " << (*t)->identifierName << endl;
//       found = 1;
//       return found;
//     }
//     ++i;
//   }

//   return found;
// }

void SymbolTable::printStack(vector<Symbol*> symbolList) {
  for(size_t i = 0; i<symbolList.size(); ++i) {
    cout << "Identifier: " << symbolList[i]->identifierName << endl;
  }
}

int SymbolTable::getScopeLevel() {
  cout << "scope level (blockCount): " << this->blockCount << endl;
  return this->blockCount;
  // Get scope level of most recent, return
  // if(this->localIdentifiers.size() == 0) {
  //   return 0; // is at top, global level
  // }
  
  // Symbol *lastSymbol = this->localIdentifiers.back();

  // cout << "last symbol of local identifiers: " << lastSymbol->identifierName << " with scope level: " << lastSymbol->scopeLevel << endl;

  // return lastSymbol->scopeLevel;
}

Symbol* SymbolTable::createSymbol(Token *token) {
  Symbol *newSymbol = new Symbol();

  newSymbol->identifierName = token->tokenInstance;
  newSymbol->lineNumber = token->lineNumber;
  newSymbol->tokenID = token->tokenID;
  newSymbol->scopeLevel = this->getScopeLevel();

  return newSymbol;
}

// void SymbolTable::insert(string identifierName) {
//   // Can Add Scope later, maybe to state
//   cout << "\ninserting identifier: " << identifierName << endl;

//   this->identifierNames.push_back(identifierName);

//   cout << "this is a next identifer in the sequence and we need to do something about it. Okay? Okay? We see what's bout ot happen ext okay" << endl;
// }

// bool SymbolTable::verify(string identifierName) {
//   cout << "\nverifying uniqueness of identifier: " << identifierName << endl;

//   bool found = this->searchName(identifierName);

//   cout << "Is Found? " << found << endl; // 1 is true, 0 is false
  
//   return found;
// }

// bool SymbolTable::searchName(string name) {
//   // Search through the identifierNames vector for the name
//   // If found, return true. Else, return false

//   for(size_t i = 0; i<this->identifierNames.size(); ++i) {
//     if(this->identifierNames[i] == name) {
//       cout << "found name in identifiers. returning true" << endl;
//       return true;
//     }
//   }

//   return false;
// }

// void SymbolTable::printTokens() {
//   for(size_t i = 0; i<this->identifierNames.size(); ++i) {
//     cout << "Identifier: " << this->identifierNames[i] << endl;
//   }
// }