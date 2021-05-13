// Each Node will contain the label and token, and anything else that's relevant
// TODO: Make a Class, using the Node class from P0

#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>
#include "token.h"

using namespace std;

struct Node
{
  string label;
  // NOTE: could #include <vector> to dynamically create the children and tokens
  vector<Token*> tokens;
  // Token *token1;
  // Token *token2;
  vector<Node*> nodes;
  // Node *left;
  // Node *right;
  // Add more nodes? Max potential Nodes and Tokens?
};

#endif