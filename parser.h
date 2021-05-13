#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <istream>
#include "node.h"
#include "token.h"
#include "scanner.h"
#include "tree.h"

using namespace std;

class Parser
{
public:
  Node* parser(istream *input); // TODO: change to the tree root Node

private:
  istream *input; // for getting the next token from scanner
  Token *token; // the current token lookahead
  Scanner *scanner; // to maintain an instance for retaining line info information
  ParseTree *tree; // TODO: rename

  // Define nonterminal functions (as templates for now)
  Node* program();
  Node* block();
  Node* vars();
  Node* expr();

  Node* N();
  Node* A();
  Node* M();
  Node* R();
  Node* stats();
  Node* mStat();
  Node* stat();

  Node* in();
  Node* out();
  Node* _if();
  Node* loop();
  Node* assign();
  Node* RO();
  Node* label();
  Node* _goto();

  void printToken(Token *token);
  Token* getTokenFromScanner();
  bool isEofToken(Token *token);
  void throwError(string message);
};

#endif