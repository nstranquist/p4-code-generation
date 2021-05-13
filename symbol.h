#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include "token.h"

using namespace std;

struct Symbol {
  // Identifier Data
  string identifierName;
  int lineNumber;
  tokenIDs tokenID;
  // Symbol Data
  int scopeLevel; // 0 is global, going down
};

#endif