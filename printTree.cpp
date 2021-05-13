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
  this->symbolTable.isBeforeMain = true;
  this->symbolTable.varCount = 0;
  this->symbolTable.blockCount = 0;
  this->stackLevel = 0; // Global = 0
  cout << "3. Analyzing Parse Tree for Semantics...\n\n" << endl;

  // print pre-order, use symbolTable functions as needed
  int startingLevel = 0;
  this->scanPreorder(root, startingLevel);

  cout << "Scan Complete. Tokens at the end: \n" << endl;
  cout << "Globals: \n" << endl;
  this->symbolTable.printStack(this->symbolTable.globalIdentifiers);
  cout << "\nLocals: \n" << endl;
  this->symbolTable.printStack(this->symbolTable.localIdentifiers);
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
    // update var counts
    this->symbolTable.varCounts.push_back(0);
    this->symbolTable.blockCount++;
  }

  if(root->tokens.size() > 0) {
    cout << ": ";
    // Print All Token Values
    size_t i = 0;
    for(vector<Token*>::iterator t = root->tokens.begin(); t != root->tokens.end(); ++t) {
      cout << (*t)->tokenInstance << " ";
      // if(this->symbolTable.isBeforeMain) {
      //   cout << "is before main" << endl;
      //   if((*t)->tokenInstance == "main") {
      //     this->symbolTable.isBeforeMain = false;
      //     cout << "is at main" << endl;
      //   }
      //   else if ((*t)->tokenID == IDENT_tk) {
      //     cout << "is identifier" << endl;

      //     Symbol *globalSymbol = this->symbolTable.createSymbol(*t);
          
      //     this->symbolTable.globalIdentifiers.push_back(globalSymbol);
      //   }
      //   else {
      //     cout << "is not main, is not identifier, but still in parse tree...? Error?" << endl;
      //     cout << "instance: " << (*t)->tokenInstance << endl;
      //   }
      // }
      // else {
        // if((*t)->tokenID == IDENT_tk) {
          if(root->label == "vars") {
            if((*t)->tokenID == IDENT_tk) {
              cout << "is 'identifier' declaration" << endl;
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
                this->symbolTable.push(symbol);
              }
              this->symbolTable.varCount++;
            }
            // else {
            //   cout << "Error: Variables can only be declared in <vars>. Use 'data' to do this." << endl;
            // }
          }
          else {
            if((*t)->tokenID == IDENT_tk) {
              cout << "is identifier token. is being used" << endl;
              int found = this->symbolTable.find((*t)->tokenInstance);
              if(found == -1) {
                cout << "could check globals, but we aren't using those" << endl;
                cout << "Error: Variable not found in scope" << endl;
                // int globalFound = this->symbolTable.verifyGlobal(*t);
                // if(globalFound == 0) {
                //   cout << "Error: variable not found in local or global scope. Must define before using" << endl;
                //   cout << (*t)->tokenInstance << " at line #" << (*t)->lineNumber << endl;
                // }
                // else {
                //   cout << "The variable was found in globals. We are OK" << endl;
                // }
              }
            }
          }
        // }
        
        if ((*t)->tokenInstance == "begin") {
          cout << "begin block found" << endl;
          // this->symbolTable.varCount = 0;
          // this->symbolTable.blockCount++;
        }
        else if ((*t)->tokenInstance == "end") {
          cout << "end block found" << endl;
          // this->symbolTable.blockCount--;
          // // pop varCount number of times
          // for(int i = this->symbolTable.varCount; i>0; i--) {
          //   this->symbolTable.pop();
          // }
          // this->symbolTable.varCount = 0;
        }
      // }
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

      // If is Identifier
      // if((*t)->tokenID==IDENT_tk) {
      //   // If Is vars, insert, else, verify
      //   if(isVars) {
      //     cout << "is vars, checking then inserting" << endl;
      //     // If exists, throw error
      //     int levelFound = this->symbolTable.verify((*t)->tokenInstance);
      //     if(levelFound >= 0 && levelFound < this->varCount) {
      //       string errorMessage = "Error: Identifier '" + (*t)->tokenInstance + "' is already in use on line #";
      //       errorMessage +=  (*t)->lineNumber;
      //       throw invalid_argument(errorMessage);
      //     }
      //     // else, insert into table
      //     else {
      //       this->symbolTable.insert((*t)->tokenInstance);
      //     }
      //   }
      //   else {
      //     cout << "is not vars, verifying" << endl;
      //     int levelFound = this->symbolTable.verify((*t)->tokenInstance);
      //     if(levelFound) {
      //       // Throw Error:
      //       cout << "ERROR: Identifier '" << (*t)->tokenInstance << "' is already being used" << endl;
      //     }
      //   }
      // }
      // ++i;
    }
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
    cout << "assuming end of block" << endl;
    // this->symbolTable.blockCount--;
    // pop varCount number of times
    int numVars;
    int varCountsSize = this->symbolTable.varCounts.size();
    if(varCountsSize > 0) {
      cout << "size > 0: " << varCountsSize << endl;
      numVars = this->symbolTable.varCounts[varCountsSize - 1];
    }
    else {
      numVars = 0;
    }
    cout << "numVars: " << numVars << endl;
    for(int i = numVars; i>0; i--) {
      this->symbolTable.pop();
    }
    this->symbolTable.varCounts.pop_back();
    cout << "made out of block" << endl;
  }
}

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
