#include <iostream>
#include <stdio.h>
#include "node.h"
#include "tree.h"
#include "printTree.h"
#include "symbolTable.h"

using namespace std;

void PrintTree::semanticAnalyze(Node *root) {
  SymbolTable symbolTable;
  this->symbolTable = symbolTable;
  this->symbolTable.varCount = 0;
  this->symbolTable.blockCount = 0;
  this->symbolTable.newBlock = true;
  this->stackLevel = 0; // Global = 0
  cout << "3. Analyzing Parse Tree for Semantics...\n\n" << endl;

  // print pre-order, use symbolTable functions as needed
  int startingLevel = 0;
  this->scanPreorder(root, startingLevel);

  cout << "Scan Complete. Tokens at the end: \n" << endl;
  this->symbolTable.printIdentifiers();
}

void PrintTree::scanPreorder(Node *root, int level) {
  if (root==NULL) {
    cout << "root is null. returning" << endl;
    return;
  }

  // if variables before <main>, process as global variables (this->symbolTable->isBeforeMain)
  // else when working in a block (<begin>...<end>)
    // set varCount = 0;
    // if inside <vars>, handle variable declarations:
      // if varCount > 0, call find(var) and error/exit if returns non-negative integer that's < varCount (means multiple definitions)
    // else if inside non-vars:
      // when identifier token is found:
      // Call find(var), if returns -1 then try verify(var), throw error if still not found
    // when leaving the block (<end>):
      // call pop() varCount times
      // reset varCount
  if(root->label == "block") {
    this->symbolTable.blockCount++;
    cout << "is a block element, assuming start of block (level: " << this->symbolTable.blockCount << ")" << endl;
    cout << "previous level's varCount: " << this->symbolTable.varCount << "\n" << endl;
    // this->symbolTable.newBlock = true;
    // if(this->symbolTable.blockCount > 1) {
    //   this->symbolTable.varCounts.push_back(this->symbolTable.varCount);
    // }
    // this->symbolTable.varCount = 0;
    // update var counts
    // this->symbolTable.varCounts.push_back(0);
  }

  if(root->tokens.size() > 0) {
    cout << ": ";
    // Print All Token Values, left to right
    size_t i = 0;

    for(vector<Token*>::iterator t = root->tokens.begin(); t != root->tokens.end(); ++t) {
      cout << (*t)->tokenInstance << " ";

      if(root->label == "vars") {
        if((*t)->tokenID == IDENT_tk) {
          // If previous token is 'data'
          if(i > 0 && root->tokens[i-1]->tokenInstance == "data") {
            // And if next token is ':='
            if(i < root->tokens.size()-1 && root->tokens[i+1]->tokenInstance == ":=") {
              if(this->symbolTable.blockCount == 0) {
                // Check global
                int foundGlobal = this->symbolTable.findGlobal((*t)->tokenInstance);
                cout << "found global: " << foundGlobal << endl;
                if(foundGlobal >= 0) { //  && foundGlobal < this->symbolTable.varCounts.at(foundGlobal)
                  string errorMessage = "Error: The global variable '" + (*t)->tokenInstance + "' is already defined at scope level 0 on line #";
                  errorMessage += to_string((*t)->lineNumber);
                  throw invalid_argument(errorMessage);
                }
              }
              if(this->symbolTable.varCount > 0) {
                int found = this->symbolTable.find((*t)->tokenInstance);
                cout << "found local: " << found << endl;
                if(found >= 0 && found < this->symbolTable.varCounts.at(found)) {
                  string errorMessage = "Error: The variable '" + (*t)->tokenInstance + "' is already defined at scope level ";
                  errorMessage += to_string(this->symbolTable.blockCount);
                  errorMessage += " on line #";
                  errorMessage += to_string((*t)->lineNumber);
                  throw invalid_argument(errorMessage);
                }
              }
              Symbol *symbol = this->symbolTable.createSymbol(*t);
              if(this->symbolTable.blockCount == 0) {
                this->symbolTable.pushGlobal(symbol);
              }
              else {
                this->symbolTable.push(symbol);
              }
              this->symbolTable.varCount++;
            }
            else {
              string errorMessage = "Error: Variable must be followed by assignment ':=' on line #" + (*t)->lineNumber;
              throw invalid_argument(errorMessage);
            }
          }
          else {
            string errorMessage = "Error: 'data' must be used to declare a new variable on line #" + (*t)->lineNumber;
            throw invalid_argument(errorMessage);
          }
        }
      }
      else {
        if((*t)->tokenID == IDENT_tk) {
          int found = this->symbolTable.find((*t)->tokenInstance);
              cout << "found local: " << found << endl;
          
          if(found == -1) {
            // Check in Globals
            int foundGlobal = this->symbolTable.findGlobal((*t)->tokenInstance);
              cout << "found global: " << foundGlobal << endl;

            if(foundGlobal == -1) {
              this->symbolTable.printIdentifiers();
              // still not found, throw error
              string errorMessage = "Error: Unrecognized identifier '" + (*t)->tokenInstance;
              errorMessage += "' used before declaration (with 'data') on line #";
              errorMessage += to_string((*t)->lineNumber);
              throw invalid_argument(errorMessage);
            }
          }
        }
      }

      ++i;

      // If is before <main>
        // If is <main>
          // Set isBeforeMain = false;
        // Else if is identifier
          // Add identifier to symbolTable as a global variable: this->symbolTable.push(...data);
        // Else... throw error(?)
      // Else if is not before <main>
        // If is identifier
          // If is in <vars>
            // If varCount > 0, call find(var) and error/exit if non-negative number that's < varCount (multiple definitions)
            // Then push(var) and this->varCount++;
          // If is not in <vars>
            // Call find(var), if -1, try this->symbolTable.verify(var) (to check if global variable). Error if still not found.
        // If is <begin> block
          // reset varCount = 0;
        // If is <end> block
          // call pop() varCount number of times
          // reset varCount = 0; (or pop from vector if using that option)
    }
    // END of for loop
    // if(root->label == "vars" && this->symbolTable.newBlock && this->symbolTable.blockCount > 0) {
    //   cout << "\n!! pushing varCount to varCounts: " << this->symbolTable.varCount << endl;
    //   // this->symbolTable.varCounts.push_back(this->symbolTable.varCount);
    //   // this->symbolTable.varCount = 0; // reset the var count
    //   // this->symbolTable.newBlock = false;
    //   // this->symbolTable.printVarCounts();
    // }
  }
  if(root->label == "block") {
    // if(this->symbolTable.blockLevel > 1) { ... } // if wanting to exclude globals from varCounts
    cout << "\npushing varCount: " << this->symbolTable.varCount << "\n" << endl;
    this->symbolTable.varCounts.push_back(this->symbolTable.varCount);
    this->symbolTable.varCount = 0;
  }

  cout << "\n";

  // Iterate over vectors, from left to right, calling printPreorder() on the vector
  // (source) https://cal-linux.com/tutorials//vectors.html
  size_t i = 0;
  for(vector<Node*>::iterator t = root->nodes.begin(); t != root->nodes.end(); ++t) {
    scanPreorder(root->nodes[i], level + 1);
    ++i;
  }

  if(root->label == "block") {
    cout << "assuming end of block (level: " << this->symbolTable.blockCount << ")\n" << endl;
    // pop varCount number of times

    cout << "PRINTING VAR COUNTS (before pop): \n" << endl;
    this->symbolTable.printVarCounts();
    cout << "\n\nLocal Variables (before pop):\n" << endl;
    this->symbolTable.printLocal();
    cout << "\n";

    // Get the last element
    // int lastVarCount = this->symbolTable.varCounts.back();
       
    // for(int numVars = lastVarCount; numVars > 0; numVars--) {
    //   this->symbolTable.pop();
    // }

    // Pop All elements at the block level
    this->symbolTable.removeAtBlockLevel(this->symbolTable.blockCount);

    this->symbolTable.varCounts.pop_back();
    cout << "\nPRINTING VAR COUNTS (after pop): \n" << endl;
    this->symbolTable.printVarCounts();
    cout << "\n\nLocal Variables (after pop):\n" << endl;
    this->symbolTable.printLocal();
    cout << "\n";

    this->symbolTable.blockCount--;
  }
}

void PrintTree::printTree(Node *root)
{
  cout << "2. Printing Tree...\n\n";

  int startingLevel = 0;
  this->printPreorder(root, startingLevel);
}

void PrintTree::printPreorder(Node *root, int level) {
  if (root==NULL) {
    cout << "root is null. returning" << endl;
    return;
  }

  cout << std::string((level*2), '-') << root->label
    << ": " << root->nodes.size() << " nodes, "
    << root->tokens.size() << " tokens";

  if(root->tokens.size() > 0) {
    cout << ": ";
    // Print All Token Values
    size_t i = 0;
    for(vector<Token*>::iterator t = root->tokens.begin(); t != root->tokens.end(); ++t) {
      cout << (*t)->tokenInstance << " ";
      ++i;
    }
  }
  cout << "\n";

  // Iterate over vectors, from left to right, calling printPreorder() on the vector
  // (source) https://cal-linux.com/tutorials//vectors.html
  size_t i = 0;
  for(vector<Node*>::iterator t = root->nodes.begin(); t != root->nodes.end(); ++t) {
    printPreorder(root->nodes[i], level + 1);
    ++i;
  }
}
