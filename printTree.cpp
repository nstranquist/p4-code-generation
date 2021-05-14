#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include "node.h"
#include "tree.h"
#include "printTree.h"
#include "symbolTable.h"
// #include "codeGenerator.h"

using namespace std;

void PrintTree::semanticAnalyze(Node *root, string outputFilename) {
  // CodeGenerator codeGenerator;
  // this->codeGenerator = codeGenerator;
  // cout << "initializing" << endl;
  // this->codeGenerator.initialize(outputFilename);
  // cout << "done init" << endl;

  ofstream out;

  out.open(outputFilename);
  if(!out.is_open() || !out) {
    cout << "Warning! Out is not open" << endl;
  }
  else {
    out << "TEST" << endl;
    out.close();
    // return if needed
  }

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

  cout << "\nScan Complete. Tokens at the end: \n" << endl;
  this->symbolTable.printIdentifiers();

  cout << "Writing 'STOP' to file" << endl;
  cout << "'STOP'" << endl;
}

void PrintTree::scanPreorder(Node *root, int level) {
  if (root==NULL) {
    cout << "root is null. returning" << endl;
    return;
  }

  if(root->label == "block") {
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
              if(this->symbolTable.blockCount == 0) {
                // Check global
                int foundGlobal = this->symbolTable.findGlobal((*t)->tokenInstance);
                if(foundGlobal >= 0) { //  && foundGlobal < this->symbolTable.varCounts.at(foundGlobal)
                  string errorMessage = "Error: The global variable '" + (*t)->tokenInstance + "' is already defined at scope level 0 on line #";
                  errorMessage += to_string((*t)->lineNumber);
                  throw invalid_argument(errorMessage);
                }
              }
              if(this->symbolTable.varCount > 0) {
                int found = this->symbolTable.find((*t)->tokenInstance);
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
          
          if(found == -1) {
            // Check in Globals
            int foundGlobal = this->symbolTable.findGlobal((*t)->tokenInstance);

            if(foundGlobal == -1) {
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
  }
  if(root->label == "block") {
    this->symbolTable.varCounts.push_back(this->symbolTable.varCount);
    this->symbolTable.varCount = 0;
  }

  if(root->tokens.size() > 0) cout << "\n";

  // Iterate over vectors, from left to right, calling printPreorder() on the vector
  // (source) https://cal-linux.com/tutorials//vectors.html
  size_t i = 0;
  for(vector<Node*>::iterator t = root->nodes.begin(); t != root->nodes.end(); ++t) {
    scanPreorder(root->nodes[i], level + 1);
    ++i;
  }

  if(root->label == "block") {
    // Pop All elements at the block level
    this->symbolTable.removeAtBlockLevel(this->symbolTable.blockCount);

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
