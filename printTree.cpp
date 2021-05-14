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
  this->tempVars = 0;
  this->tempLabels = 0;

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

  this->out.close();
}

void PrintTree::scanPreorder(Node *root, int level) {
  if (root==NULL) {
    cout << "root is null. returning" << endl;
    return;
  }

  if(!root->tokens.empty()) {
    if(root->label == "program") {
      this->out << "<program>" << endl;
    }
    else if(root->label == "block") {
      this->out << "<block>" << endl;
    }
    else if(root->label == "vars") {
      this->out << "<vars>" << endl;
    }
    else if(root->label == "expr") {
      this->out << "<expr>" << endl;
    }
    else if(root->label == "N") {
      this->out << "<N>" << endl;
    }
    else if(root->label == "A") {
      this->out << "<A>" << endl;
    }
    else if(root->label == "M") {
      this->out << "<M>" << endl;
    }
    else if(root->label == "R") {
      this->out << "<R>" << endl;
    }
    else if(root->label == "stats") {
      this->out << "<stats>" << endl;
    }
    else if(root->label == "mStat") {
      this->out << "<mStat>" << endl;
    }
    else if(root->label == "stat") {
      this->out << "<stat>" << endl;
    }
    else if(root->label == "in") {
      // getter reads in an integer from input and stores it in the identifier
      this->out << "<in>" << endl;
    }
    else if(root->label == "out") {
      // outter outputs the given calculated value
      this->out << "<out>" << endl;
    }
    else if(root->label == "if") {
      // if statement is like in C
      // this->out << "<if>" << endl;
      // 1. Evaluate arg2 (should be 3rd token)
      // this->scanPreorder(root->nodes[2], level + 1);
      // 2. STORE results2 (will be done at the token level)
      // 3. Evaluate arg1 (should be 1st token)
      // this->scanPreorder(root->nodes[0], level + 1);

      // 4. Based on the value of <RO> (nodes[1]), write out accordingly
      
    }
    else if(root->label == "loop") {
      // Loop statement is like the while loop in C
      this->out << "<loop>" << endl;
    }
    else if(root->label == "assign") {
      // Assignment evaluates the expression on the right and assigns to the ID on the left
      this->out << "<assign>" << endl;
    }
    else if(root->label == "RO") {
      /*
      =< is less equal
      => is greater equal
      [ == ]  is NOT equal
      == is equal
      % returns true if the signs of the arguments are opposite
      */
      this->out << "<RO>" << endl;
      // if(root->nodes[1]->tokens[0]->tokenInstance == "=>") {
      //   // greater than equal
      //   this->out << "" << endl;
      // }
      // else if(root->nodes[1]->tokens[0]->tokenInstance == "=<") {
      //   // less than equal
      //   this->out << "" << endl;
      // }
      // else if(root->nodes[1]->tokens[0]->tokenInstance == "==") {
      //   // is equal
      //   this->out << "" << endl;
      // }
      // else if(root->nodes[1]->tokens[0]->tokenInstance == "[==]") {
      //   // not equal
      //   this->out << "" << endl;
      // }
      // else if(root->nodes[1]->tokens[0]->tokenInstance == "%") {
      //   // returns true if signs of arguments are opposite
      //   this->out << "" << endl;
      // }
    }
    else if(root->label == "label") {
      // void statement places a label that can be jumped to directly in a branch using proc
      this->out << "<label>" << endl;
    }
    else if(root->label == "goto") {
      this->out << "<goto>" << endl;
    }
    else {
      cout << "ERROR?: no matching label found" << endl;
    }
  }

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
                this->symbolTable.pushGlobal(symbol);
                this->out << "PUSH" << endl;
                this->out << "READ " << root->tokens[i]->tokenInstance << endl;
                this->out << "LOAD " << root->tokens[i]->tokenInstance << endl;
                this->out << "STORE " << root->tokens[i]->tokenInstance << endl;
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
                // string tempVar = this->generateTempVar();
                this->out << "READ " << symbol->identifierName << endl;
                this->out << "LOAD " << symbol->identifierName << endl;
                this->out << "STACKW " << 0 << endl;
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

        ++i;

      }
    }
    else if(root->label == "out") {
      this->scanPreorder(root->nodes[0], level + 1);

      string tempVar = this->generateTempVar();
      this->out << "STORE " << tempVar << endl; // load from ACC after the correct value is loaded in
      this->out << "WRITE " << tempVar << endl;
      // 1. find var in global

      // 2. find var in local

      // 3. READ and PRINT var
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
    else if(root->label == "assign") {
      // Evaluate RHS first, which would be the only node
      this->scanPreorder(root->nodes[0], level + 1);
      // Then STORE
      this->out << "STORE " << root->tokens[1]->tokenInstance << endl;
    }
    else if(root->label == "expr") {
      this->scanPreorder(root->nodes[0], level + 1);
      // Either getting "-" or Not.
      if(!root->tokens.empty() && root->tokens[0]->tokenInstance == "-") {
        this->out << "SUB "; // Note: can fill the rest in later

      }
    }
    else {
      size_t i = 0;
      for(vector<Node*>::iterator t = root->nodes.begin(); t != root->nodes.end(); ++t) {
        scanPreorder(root->nodes[i], level + 1);
        ++i;
      }
    }
      // else if((*t)->tokenID == IDENT_tk && root->label != "in") {
      //   int found = this->symbolTable.find((*t)->tokenInstance);
        
      //   if(found == -1) {
      //     // Check in Globals
      //     int foundGlobal = this->symbolTable.findGlobal((*t)->tokenInstance);

      //     if(foundGlobal == -1) {
      //       // still not found, throw error
      //       string errorMessage = "Error: Unrecognized identifier '" + (*t)->tokenInstance;
      //       errorMessage += "' used before declaration (with 'data') on line #";
      //       errorMessage += to_string((*t)->lineNumber);
      //       throw invalid_argument(errorMessage);
      //     }
      //     else {
      //       // Use Global
      //       this->out << "READ " << this->symbolTable.globalIdentifiers[foundGlobal] << endl;
      //       // this->out << "LOAD " << this->symbolTable.globalIdentifiers[foundGlobal] << endl;
      //       // this->out << ""
      //     }
      //   }
      //   else {
      //     this->out << "STACKR " << found << endl;
      //     // string tempVar = this->generateTempVar();
      //     this->out << "STORE " << this->symbolTable.localIdentifiers[found]->identifierName << endl;
      //     if(root->label == "out") {
      //       this->out << "WRITE " << this->symbolTable.localIdentifiers[found]->identifierName << endl;
      //     }
      //   }
      // }
      // else if((*t)->tokenID == NUM_tk) {
      //   this->out << "LOAD " << (*t)->tokenInstance << endl;
      //   if(root->label == "out") {
      //     this->out << "WRITE " << (*t)->tokenInstance << endl;
      //   }
      // }
  if(root->label == "block") {
    this->symbolTable.varCounts.push_back(this->symbolTable.varCount);
    this->symbolTable.varCount = 0;
  }

  // Begin checks for assembly outputs
  // if(root->label == "program") {
  //   // this->out << "<program>" << endl;
  // }
  // else if(root->label == "block") {
  //   // this->out << "<block>" << endl;
  // }
  // else if(root->label == "vars") {
  //   // this->out << "<vars>" << endl;
  // }
  // else if(root->label == "expr") {
  //   this->out << "<expr>" << endl;
  // }
  // else if(root->label == "N") {
  //   this->out << "<N>" << endl;
  // }
  // else if(root->label == "A") {
  //   this->out << "<A>" << endl;
  // }
  // else if(root->label == "M") {
  //   this->out << "<M>" << endl;
  // }
  // else if(root->label == "R") {
  //   this->out << "<R>" << endl;
  // }
  // else if(root->label == "stats") {
  //   this->out << "<stats>" << endl;
  // }
  // else if(root->label == "mStat") {
  //   this->out << "<mStat>" << endl;
  // }
  // else if(root->label == "stat") {
  //   this->out << "<stat>" << endl;
  // }
  // else if(root->label == "in") {
  //   // getter reads in an integer from input and stores it in the identifier
  //   this->out << "<in>" << endl;
  // }
  // else if(root->label == "out") {
  //   // outter outputs the given calculated value
  //   this->out << "<out>" << endl;
  // }
  // else if(root->label == "if") {
  //   // if statement is like in C
  //   // this->out << "<if>" << endl;
  //   // 1. Evaluate arg2 (should be 3rd token)
  //   // this->scanPreorder(root->nodes[2], level + 1);
  //   // 2. STORE results2 (will be done at the token level)
  //   // 3. Evaluate arg1 (should be 1st token)
  //   // this->scanPreorder(root->nodes[0], level + 1);

  //   // 4. Based on the value of <RO> (nodes[1]), write out accordingly
    
  // }
  // else if(root->label == "loop") {
  //   // Loop statement is like the while loop in C
  //   this->out << "<loop>" << endl;
  // }
  // else if(root->label == "assign") {
  //   // Assignment evaluates the expression on the right and assigns to the ID on the left
  //   this->out << "<assign>" << endl;
  // }
  // else if(root->label == "RO") {
  //   /*
  //   =< is less equal
  //   => is greater equal
  //   [ == ]  is NOT equal
  //   == is equal
  //   % returns true if the signs of the arguments are opposite
  //   */
  //   this->out << "<RO>" << endl;
  //   if(root->nodes[1]->tokens[0]->tokenInstance == "=>") {
  //     // greater than equal
  //     this->out << "" << endl;
  //   }
  //   else if(root->nodes[1]->tokens[0]->tokenInstance == "=<") {
  //     // less than equal
  //     this->out << "" << endl;
  //   }
  //   else if(root->nodes[1]->tokens[0]->tokenInstance == "==") {
  //     // is equal
  //     this->out << "" << endl;
  //   }
  //   else if(root->nodes[1]->tokens[0]->tokenInstance == "[==]") {
  //     // not equal
  //     this->out << "" << endl;
  //   }
  //   else if(root->nodes[1]->tokens[0]->tokenInstance == "%") {
  //     // returns true if signs of arguments are opposite
  //     this->out << "" << endl;
  //   }
  // }
  // else if(root->label == "label") {
  //   // void statement places a label that can be jumped to directly in a branch using proc
  //   this->out << "<label>" << endl;
  // }
  // else if(root->label == "goto") {
  //   this->out << "<goto>" << endl;
  // }
  // else {
  //   cout << "ERROR?: no matching label found" << endl;
  // }

  if(root->tokens.size() > 0) cout << "\n";

  // Iterate over vectors, from left to right, calling printPreorder() on the vector
  // (source) https://cal-linux.com/tutorials//vectors.html
  // size_t i = 0;
  // for(vector<Node*>::iterator t = root->nodes.begin(); t != root->nodes.end(); ++t) {
  //   scanPreorder(root->nodes[i], level + 1);
  //   ++i;
  // }

  if(root->label == "block") {
    // Pop All elements at the block level
    this->symbolTable.removeAtBlockLevel(this->symbolTable.blockCount);

    this->symbolTable.blockCount--;
  }
}

string PrintTree::generateTempVar() {
  string tempName = "V" + to_string(this->tempVars);
  this->tempVars++;
  return tempName;
}

string PrintTree::generateTempLabel() {
  string tempName = "T" + to_string(this->tempLabels);
  this->tempLabels++;
  return tempName;
}

void PrintTree::printGlobalsToStorage() {
  for(vector<Symbol*>::iterator t = this->symbolTable.globalIdentifiers.begin(); t != this->symbolTable.globalIdentifiers.end(); ++t) {
    this->out << (*t)->identifierName << " 0" << endl;
  }
}

void PrintTree::printTempVarsToStorage() {
  for(int i=0; i<this->tempVars; i++) {
    this->out << "V" << to_string(i) << " 0" << endl;
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
