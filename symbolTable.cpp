#include <stdio.h>
#include <iostream>
#include "symbolTable.h"

using namespace std;

void SymbolTable::push(Symbol *symbol) { // string identifierName, int lineNumber, tokenIDs tokenID, int scopeLevel
  this->localIdentifiers.push_back(symbol);
}

void SymbolTable::pop() {
  // remove "top-most" element from stack
  if(this->localIdentifiers.empty()) return;
  this->localIdentifiers.pop_back();
}

int SymbolTable::find(string identifierName) {
  int distanceFromTop = -1;

  // Iterate over all vars in the Symbol Table
  int index = 0;

  // for(vector<Symbol*>::reverse_iterator t = this->localIdentifiers.rbegin(); t != this->localIdentifiers.rend(); ++t) {
  for(vector<Symbol*>::reverse_iterator t = this->localIdentifiers.rbegin(); t != this->localIdentifiers.rend(); ++t) {
    // find first occurrence of the argument on the stack (top to bottom)
    if(identifierName == (*t)->identifierName) {
      // return the distance from the top of stack, where 0 is top of stack, -1 is not found
      // distanceFromTop = (*t)->scopeLevel;
      return index;
    }
    index++;
  }

  // return -1 if not found in stack
  return distanceFromTop;
}

int SymbolTable::findGlobal(string identifierName) {
  int distanceFromTop = -1;

  // Iterate over all vars in the Symbol Table
  int index = 0;

  // for(vector<Symbol*>::reverse_iterator t = this->globalIdentifiers.rbegin(); t!=this->globalIdentifiers.rend(); ++t) {
  for(vector<Symbol*>::reverse_iterator t = this->globalIdentifiers.rbegin(); t!=this->globalIdentifiers.rend(); ++t) {
    // find first occurrence of the argument on the stack (top to bottom)
    if(identifierName == (*t)->identifierName) {
      // return the distance from the top of stack, where 0 is top of stack, -1 is not found
      // distanceFromTop = (*t)->scopeLevel;
      return index;
    }
    index++;
  }

  // return -1 if not found in stack
  return distanceFromTop;
}

void SymbolTable::printIdentifiers() {
  cout << "Globals: \n" << endl;
  this->printGlobal();
  cout << "\nLocals (should be empty): \n" << endl;
  this->printLocal();
}

void SymbolTable::printGlobal() {
  for(vector<Symbol*>::iterator t = this->globalIdentifiers.begin(); t != this->globalIdentifiers.end(); ++t) {
    cout << "Identifier: " << (*t)->identifierName << " - scope level: " << (*t)->scopeLevel << endl;
  }
}

void SymbolTable::printLocal() {
  for(vector<Symbol*>::iterator t = this->localIdentifiers.begin(); t != this->localIdentifiers.end(); ++t) {
    cout << "Identifier: " << (*t)->identifierName << " - scope level: " << (*t)->scopeLevel << endl;
  }
}

void SymbolTable::removeAtBlockLevel(int blockLevel) {
  int matchingBlocks = 0;

  // Iterate over local symbols, remove if matches block level
  for(vector<Symbol*>::iterator t = this->localIdentifiers.begin(); t != this->localIdentifiers.end(); ++t) {
    if((*t)->scopeLevel == blockLevel) {
      matchingBlocks++;
    }
  }

  for(int i = matchingBlocks; i>0; i--) {
    this->pop();
  }
}

Symbol* SymbolTable::createSymbol(Token *token) {
  Symbol *newSymbol = new Symbol();

  newSymbol->identifierName = token->tokenInstance;
  newSymbol->lineNumber = token->lineNumber;
  newSymbol->tokenID = token->tokenID;
  newSymbol->scopeLevel = this->blockCount;

  return newSymbol;
}

void SymbolTable::printVarCounts() {
  size_t i = 0;
  for(vector<int>::iterator t = this->varCounts.begin(); t != this->varCounts.end(); ++t) {
    cout << "var count #" << i << ": " << *(t) << endl;
    ++i;
  }
}

void SymbolTable::pushGlobal(Symbol *symbol) { // string identifierName, int lineNumber, tokenIDs tokenID, int scopeLevel
  this->globalIdentifiers.push_back(symbol);
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