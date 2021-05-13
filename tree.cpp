#include "tree.h"

#include <fstream>
#include <iostream>
#include <string>
#include <queue>

using namespace std;

ParseTree::ParseTree() {
  root = nullptr;
}

Node* ParseTree::createNode(string label) {
  Node *node = new Node();

  node->label = label;

  return node;
}

Node* ParseTree::buildTree(string label) {
  string delimiterComma = ",";
  string delimiterSpace = " ";
  string delimiterTab = "\t";
  string line;

  cout << "Building Tree by creating first Node." << endl;

  Node *rootNode;
  rootNode = this->createNode(label);

  // string key = this->getCharKey(line);

  // TODO: replace with this->addTokenToNode();
  // this->handleNewNode(label);
  // this->addTokenToNode(node, );

  // return this->root;
  return rootNode;
}

void ParseTree::addTokenToNode(Node *node, Token *token) {
  node->tokens.push_back(token);
}

// root -> left -> right
void ParseTree::printPreorder(Node *rootPtr, int level) {
  if (rootPtr==NULL) return;

  cout << std::string((level*2), '-') << rootPtr->label << endl; // << ": " << rootPtr->printToken()
  // Iterate over vectors, from left to right, calling printPreorder() on the vector
  // (source) https://cal-linux.com/tutorials//vectors.html
  int i;
  for(vector<Node*>::iterator t = rootPtr->nodes.begin(); t != rootPtr->nodes.end(); ++t) {
    printPreorder(rootPtr->nodes[i], level + 1);
    i++;
  }
  // printPreorder(rootPtr->left, level+1);
  // printPreorder(rootPtr->right, level+1);
}

void ParseTree::printToken(Token *token) {
  if(token == NULL) {
    cout << "token is null" << endl;
    return;
  }
  cout << "Printing Token in tree.cpp" << endl;
  cout << "Token 1: Line #: " << token->lineNumber
    << "\tInstance: " << token->tokenInstance
    << "\tID: " << token->tokenID
    << endl;
}

void ParseTree::printNode(Node *node) {
  if(node == NULL) {
    cout << "node is null" << endl;
    return;
  }
  cout << "Printing Node in tree.cpp" << endl;
  cout << "Node: Label: " << node->label << endl;
  cout<< "Tokens: " << endl;
  int i = 1;
  // Print Tokens
  for(vector<Token*>::iterator token_iterator = node->tokens.begin(); token_iterator != node->tokens.end(); ++token_iterator) {
    cout << "Token #" << i << endl;
    cout << "\tline " << (*token_iterator)->lineNumber
      << " \tInstance: " << (*token_iterator)->tokenInstance
      << " \tID: " << (*token_iterator)->tokenID << endl;
    i++;
  }
  // Print Sub-Nodes
  i = 1;
  for(vector<Node*>::iterator node_iterator = node->nodes.begin(); node_iterator != node->nodes.end(); ++node_iterator) {
    cout << "Node #" << i
       << ": " << (*node_iterator)->label << endl;
    i++;
  }
}

Node* ParseTree::insertNode(string label) { // Node *root
  // if root is null, need to initialize it
  Node *newNode;
  newNode = this->createNode(label);
  return newNode;
}

bool ParseTree::areNodesEmpty(Node *node) {
  size_t numNodes = node->nodes.size();
  cout << "number of nodes: " << numNodes << endl;
  if(numNodes == 0) {
    return true;
  }
  return false;
}

bool ParseTree::areTokensEmpty(Node *node) {
  size_t numTokens = node->tokens.size();
  cout << "number of tokens: " << numTokens << endl;
  if(numTokens == 0) {
    return true;
  }
  return false;
}
