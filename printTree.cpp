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
  this->tempVarsCount = 0;
  this->tempLabelsCount = 0;

  this->out.open(outputFilename);
  if(!this->out.is_open() || !this->out) {
    cout << "Warning! Out is not open" << endl;
    throw invalid_argument("Error: Could not open file provided (named '" + outputFilename + "'");
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

  // Print all global variables to storage
  this->out << "STOP" << endl;
  
  this->printGlobalsToStorage();
  this->printTempVarsToStorage();
  this->printTempLabelsToStorage();

  this->out.close();
}

void PrintTree::scanPreorder(Node *root, int level) {
  if (root==NULL) {
    cout << "root is null. returning" << endl;
    return;
  }

  string tempVar = "";
  string tempLabel = "";

  // Need to switch(root->label) so that based on the label, we can do specific things
  if(root->label == "block") {
    this->symbolTable.blockCount++;
  }

  if(root->tokens.size() > 0) {
    cout << ": ";
  }
    // Print All Token Values, left to right
    size_t i = 0;

    if(root->label == "in") {
      // getter reads in an integer from input and stores it in the identifier
      int found = this->symbolTable.find(root->tokens[1]->tokenInstance);
      if(found >= 0) {
        // 1. Create new temporary variable T#
        string tempVar = this->generateTempVar();
        // 2. Read it, 3. Load it
        this->out << "READ " << tempVar << endl;
        this->out << "LOAD " << tempVar << endl;
        // 4. "STACKW found"
        this->out << "STACKW " << to_string(found) << endl;
      }
      else {
        this->out << "READ " << root->tokens[1]->tokenInstance << endl;
        this->out << "LOAD " << root->tokens[1]->tokenInstance << endl;
        this->out << "STORE " << root->tokens[1]->tokenInstance << endl;
        // Note: Will leave accumulator with this amount
      }
    }

    else if(root->label == "vars") {
      for(vector<Token*>::iterator t = root->tokens.begin(); t != root->tokens.end(); ++t) {
        cout << (*t)->tokenInstance << " ";
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
                Symbol *symbol = this->symbolTable.createSymbol(*t);
                cout << "Pushing global symbol: " << symbol->identifierName << endl;
                this->symbolTable.pushGlobal(symbol);
                this->out << "PUSH" << endl;
              }
              else {
                int found = this->symbolTable.find((*t)->tokenInstance);
                if(found >= 0) { //  && found < this->symbolTable.varCounts.at(found)
                  string errorMessage = "Error: The variable '" + (*t)->tokenInstance + "' is already defined at scope level ";
                  errorMessage += to_string(this->symbolTable.blockCount);
                  errorMessage += " on line #";
                  errorMessage += to_string((*t)->lineNumber);
                  throw invalid_argument(errorMessage);
                }

                Symbol *symbol = this->symbolTable.createSymbol(*t);
                this->symbolTable.push(symbol);
                cout << "pushing temp symbol: " << symbol->identifierName << endl;
                string tempVar = this->generateTempVar();
                this->out << "LOAD " << tempVar << endl;
                this->out << "ADD " << root->tokens[i+2]->tokenInstance << endl;
                this->out << "STACKW 0" << endl;
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
        else if((*t)->tokenID == NUM_tk) {
          if(root->tokens[i-1]->tokenInstance == ":=") {
            // Add the number info to the global vars
            this->globalValues.push_back(root->tokens[i]->tokenInstance);
          }
          else {
            string errorMessage = "Error: Number must be preceded by ':=' on line #" + (*t)->lineNumber;
            throw invalid_argument(errorMessage);
          }
        }

        ++i;

      }
      if(!root->nodes.empty()) {
        this->scanPreorder(root->nodes[0], level + 1);
      }
    }
    else if(root->label == "out") {
      this->scanPreorder(root->nodes[0], level + 1);

      string tempVar = this->generateTempVar();
      this->out << "STORE " << tempVar << endl; // load from ACC after the correct value is loaded in
      this->out << "WRITE " << tempVar << endl;
    }
    else if(root->label == "assign") {
      // Evaluate RHS first, which would be the only node
      this->scanPreorder(root->nodes[0], level + 1);
      // Then STORE
      this->out << "STORE " << root->tokens[1]->tokenInstance << endl;
    }
    else if(root->label == "expr") {
      this->scanPreorder(root->nodes[0], level + 1);
      if(!root->tokens.empty() && root->tokens[0]->tokenInstance == "-") {
        tempVar = this->generateTempVar();
        this->out << "STORE " << tempVar << endl;
        this->scanPreorder(root->nodes[1], level + 1);
        this->out << "SUB " << tempVar << endl;; // Note: can fill the rest in later
        this->out << "MULT -1" << endl;
      }
    }
    else if(root->label == "N") {
      if(!root->tokens.empty()) {
        // Then will go right to left
        this->scanPreorder(root->nodes[1], level + 1);
        tempVar = this->generateTempVar();
        this->out << "STORE " << tempVar << endl;
        this->scanPreorder(root->nodes[0], level + 1);
        if(root->tokens[0]->tokenInstance == "/") {
          this->out << "DIV " << tempVar << endl;
        }
        else if(root->tokens[0]->tokenInstance == "*") {
          this->out << "MULT " << tempVar << endl;
        }
      }
      else {
        // will just do normally
        this->scanPreorder(root->nodes[0], level + 1);
      }
    }
    else if(root->label == "A") {
      if(!root->tokens.empty()) {
        this->scanPreorder(root->nodes[1], level + 1);
        tempVar = this->generateTempVar();
        this->out << "STORE " << tempVar << endl;
        this->scanPreorder(root->nodes[0], level + 1);
        if(root->tokens[0]->tokenInstance == "+") {
          this->out << "ADD " << tempVar << endl;
        }
      }
      else {
        this->scanPreorder(root->nodes[0], level + 1);
      }
    }
    else if(root->label == "M") {
      this->scanPreorder(root->nodes[0], level + 1);
    }
    else if(root->label == "R") {
      if(!root->tokens.empty() && (root->tokens[0]->tokenID == IDENT_tk || root->tokens[0]->tokenID == NUM_tk)) {
        // find the variable if IDENT, then error or LOAD
        if(root->tokens[0]->tokenID == IDENT_tk) {
          int found = this->symbolTable.find(root->tokens[0]->tokenInstance);
          if(found == -1) {
            int foundGlobal = this->symbolTable.findGlobal(root->tokens[0]->tokenInstance);
            if(foundGlobal == -1) {
              // Throw error
              string errorMessage = "Error: Unrecognized identifier '" + root->tokens[0]->tokenInstance;
              errorMessage += "' used before declaration (with 'data') on line #";
              errorMessage += to_string(root->tokens[0]->lineNumber);
              throw invalid_argument(errorMessage);
            }
          }
        }
        this->out << "LOAD " << root->tokens[0]->tokenInstance << endl;
      }
    }
    else if(root->label == "if") {
      cout << "we are in 'if'" << endl;
      tempLabel = this->generateTempLabel();
      // 1. expr right
      this->scanPreorder(root->nodes[2], level + 1);
      tempVar = this->generateTempVar();
      this->out << "STORE " << tempVar << endl;
      // 2. expr left
      this->scanPreorder(root->nodes[0], level + 1);
      // 3. RO
      if(root->nodes[1]->tokens[0]->tokenInstance == "=>") {
        this->out << "SUB " << tempVar << endl;
        // greater than equal
        this->out << "BRZNEG " << tempLabel << endl;

        this->scanPreorder(root->nodes[3], level + 1);
      }
      else if(root->nodes[1]->tokens[0]->tokenInstance == "=<") {
        this->out << "SUB " << tempVar << endl;
        // less than equal
        this->out << "BRZPOS " << tempLabel << endl;

        this->scanPreorder(root->nodes[3], level + 1);
      }
      else if(root->nodes[1]->tokens[0]->tokenInstance == "==") {
        this->out << "SUB " << tempVar << endl;
        // equal
        this->out << "BRNEG " << tempLabel << endl;
        this->out << "BRPOS " << tempLabel << endl;

        this->scanPreorder(root->nodes[3], level + 1);
      }
      else if(root->nodes[1]->tokens[0]->tokenInstance == "%") {
        this->out << "MULT " << tempVar << endl;
        // opposite signs
        this->out << "BRPOS " << tempLabel << endl;

        this->scanPreorder(root->nodes[3], level + 1);
      }
      else if(root->nodes[1]->tokens[0]->tokenInstance == "[==]") {
        this->out << "SUB " << tempVar << endl;
        // opposite signs
        this->out << "BRZERO " << tempLabel << endl;

        this->scanPreorder(root->nodes[3], level + 1);
      }
      this->out << tempLabel << ": ";
    }
    else if(root->label == "loop") {
      tempLabel = this->generateTempLabel();
      string loopLabel = this->generateTempLabel();
      this->out << loopLabel << ": ";
      // 1. expr right
      this->scanPreorder(root->nodes[2], level + 1);
      tempVar = this->generateTempVar();
      this->out << "STORE " << tempVar << endl;
      // 2. expr left
      this->scanPreorder(root->nodes[0], level + 1);
      // 3. RO
      
      if(root->nodes[1]->tokens[0]->tokenInstance == "=>") {
        this->out << "SUB " << tempVar << endl;
        // greater than equal
        this->out << "BRZNEG " << tempLabel << endl;
        // Do everything inside loop
        this->scanPreorder(root->nodes[3], level + 1);
      }
      else if(root->nodes[1]->tokens[0]->tokenInstance == "=<") {
        this->out << "SUB " << tempVar << endl;
        // less than equal
        this->out << "BRZPOS " << tempLabel << endl;
        // Do everything inside loop
        this->scanPreorder(root->nodes[3], level + 1);
      }
      else if(root->nodes[1]->tokens[0]->tokenInstance == "==") {
        this->out << "SUB " << tempVar << endl;
        // equal
        this->out << "BRNEG " << tempLabel << endl;
        this->out << "BRPOS " << tempLabel << endl;
        // Do everything inside loop
        this->scanPreorder(root->nodes[3], level + 1);
      }
      else if(root->nodes[1]->tokens[0]->tokenInstance == "%") {
        this->out << "MULT " << tempVar << endl;
        // opposite signs
        this->out << "BRPOS " << tempLabel << endl;
        // Do everything inside loop
        this->scanPreorder(root->nodes[3], level + 1);
      }
      else if(root->nodes[1]->tokens[0]->tokenInstance == "[==]") {
        this->out << "SUB " << tempVar << endl;
        // opposite signs
        this->out << "BRZERO " << tempLabel << endl;
        // Do everything inside loop
        this->scanPreorder(root->nodes[3], level + 1);
      }
      this->out << "BR " << loopLabel << endl;
      // 4. label stuff...
      this->out << tempLabel << ": NOOP" << endl;
    }
    else if(root->label == "label") { // void Identifier
      if(!root->tokens.empty() && root->tokens[1]->tokenID == IDENT_tk) {
        tempLabel = this->generateTempLabel();
        this->out << tempLabel << ": ";
        this->fillLabel(tempLabel);
      }
    }
    else if(root->label == "goto") { // proc Identifier
      if(!root->tokens.empty() && root->tokens[1]->tokenID == IDENT_tk) {
        if(this->lab1 != "") {
          this->out << "BR " << this->lab1 << endl;
        }
      }
    }
    else {
      size_t i = 0;
      for(vector<Node*>::iterator t = root->nodes.begin(); t != root->nodes.end(); ++t) {
        scanPreorder(root->nodes[i], level + 1);
        ++i;
      }
    }

  if(root->label == "block") {
    this->symbolTable.varCounts.push_back(this->symbolTable.varCount);
    this->symbolTable.varCount = 0;
  }

  if(root->tokens.size() > 0) {
    cout << "\n";
  }

  if(root->label == "block") {
    // Pop All elements at the block level
    this->symbolTable.removeAtBlockLevel(this->symbolTable.blockCount);

    this->symbolTable.blockCount--;
  }
}

string PrintTree::generateTempVar() {
  string tempName = "T" + to_string(this->tempVarsCount);
  this->tempVarsCount++;
  return tempName;
}
string PrintTree::getMostRecentTempVar() {
  string tempName = "T" + to_string(this->tempVarsCount-1);
  return tempName;
}

string PrintTree::generateTempLabel() {
  string tempName = "L" + to_string(this->tempLabelsCount);
  this->tempLabelsCount++;
  return tempName;
}
string PrintTree::getMostRecentTempLabel() {
  string tempName = "L" + to_string(this->tempLabelsCount-1);
  return tempName;
}

void PrintTree::printGlobalsToStorage() {
  int i = 0;
  for(vector<Symbol*>::iterator t = this->symbolTable.globalIdentifiers.begin(); t != this->symbolTable.globalIdentifiers.end(); ++t) {
    this->out << (*t)->identifierName << " " << this->globalValues[i] << endl;
    i++;
  }
}

void PrintTree::printTempVarsToStorage() {
  for(int i=0; i<this->tempVarsCount; i++) {
    this->out << "T" << to_string(i) << " 0" << endl;
  }
}
void PrintTree::printTempLabelsToStorage() {
  for(int i=0; i<this->tempLabelsCount; i++) {
    this->out << "L" << to_string(i) << " 0" << endl;
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

void PrintTree::fillLabel(string label) {
  if(this->lab1 == "") {
    this->lab1 = label;
  }
  // else if(this->lab2 == "") {
  //   this->lab2 = label;
  // }
  // else if(this->lab3 == "") {
  //   this->lab3 = label;
  // }
}