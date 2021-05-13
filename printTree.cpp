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
    cout << "is a block element, assuming start of block" << endl;
    this->symbolTable.newBlock = true;
    // update var counts
    // this->symbolTable.varCounts.push_back(0);
    this->symbolTable.blockCount++;
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
              cout << "is a valid 'identifier' declaration" << endl;
              if(this->symbolTable.varCount > 0) {
                int found = this->symbolTable.find((*t)->tokenInstance);
                cout << "found value: " << found << endl;
                if(found >= 0 && found < this->symbolTable.varCounts.at(found)) {
                  cout << "Error: This variable is already defined in this scope" << endl;
                }
              }
              Symbol *symbol = this->symbolTable.createSymbol(*t);
              if(this->symbolTable.blockCount == 0) {
                this->symbolTable.globalIdentifiers.push_back(symbol);
              }
              else {
                this->symbolTable.localIdentifiers.push_back(symbol);
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
          cout << "is identifier token. is being used" << endl;
          int found = this->symbolTable.find((*t)->tokenInstance);
              cout << "found local" << endl;
          
          if(found == -1) {
            // Check in Globals
            int foundGlobal = this->symbolTable.findGlobal((*t)->tokenInstance);
              cout << "found global" << endl;

            if(foundGlobal == -1) {
              cout << "printing" << endl;
              this->symbolTable.printIdentifiers();
              // still not found, throw error
              string errorMessage = "Error: Unrecognized identifier '" + (*t)->tokenInstance;
              errorMessage += "' used before declaration (with 'data') on line #";
              errorMessage += (*t)->lineNumber;
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
    if(root->label == "vars" && this->symbolTable.newBlock) {
      cout << "\n!! pushing varCount to varCounts: " << this->symbolTable.varCount << endl;
      this->symbolTable.varCounts.push_back(this->symbolTable.varCount);
      this->symbolTable.varCount = 0; // reset the var count
      this->symbolTable.newBlock = false;
    }
  }
  // END of tokens size check
  cout << "\n";

  // Iterate over vectors, from left to right, calling printPreorder() on the vector
  // (source) https://cal-linux.com/tutorials//vectors.html
  size_t i = 0;
  for(vector<Node*>::iterator t = root->nodes.begin(); t != root->nodes.end(); ++t) {
    scanPreorder(root->nodes[i], level + 1);
    ++i;
  }

  if(root->label == "block") {
    cout << "assuming end of block" << endl;
    this->symbolTable.blockCount--;
    // pop varCount number of times

    cout << "PRINTING VAR COUNTS (before pop): \n" << endl;
    this->symbolTable.printVarCounts();

    // Get the last element
    int lastVarCount = this->symbolTable.varCounts.back();
       
    for(int numVars = lastVarCount; numVars > 0; numVars--) {
      this->symbolTable.pop();
    }
    this->symbolTable.varCounts.pop_back();
    cout << "PRINTING VAR COUNTS (after pop): \n" << endl;
    this->symbolTable.printVarCounts();

    cout << "\nmade out of block" << endl;
  }
}
 // int numVars;
    // int varCountsSize = this->symbolTable.varCounts.size();
    // if(varCountsSize > 0) {
    //   cout << "size > 0: " << varCountsSize << endl;
    //   numVars = this->symbolTable.varCounts[varCountsSize - 1];
    // }
    // else {
    //   numVars = 0;
    // }
void PrintTree::printTree(Node *root)
{
  cout << "2. Printing Tree...\n\n";

  // Print with Preorder traversal, with indentations, for testing purposes
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
