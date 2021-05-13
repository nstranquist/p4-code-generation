#include <istream>
#include <iostream>
#include <string>
#include <stdio.h>

#include "parser.h"
#include "node.h"
#include "scanner.h"
#include "tree.h"

using namespace std;

Node* Parser::parser(istream *input)
{
  ParseTree *tree = new ParseTree();
  this->tree = tree;

  Node *root_node;

  cout << "1. Parsing...\n\n";
  this->input = input;

  Scanner *scanner = new Scanner();
  this->scanner = scanner; // save to state to retain info about line numbers and stuff

  // Is this already our lookahead token, or do we get one more? --> This is the lookahead.
  this->token = this->scanner->getToken(input);

  this->printToken(token);

  root_node = program();

  if (token->tokenID == EOF_tk)
  {
    cout << "Parse was OK" << endl;
  }
  else
  {
    this->throwError("Error, EOF token not found");
  }

  cout << "Finished Processing\n\n";

  return root_node;
}

// Implement the Non-Terminal Functions (20) or 19

Node* Parser::program()
{
  // <vars>main<block>
  // 1. Call variables
  // 2. main? --> it's a "structural identifier", similar to "begin" and "end"
  // 3. Call block
  Node *root;
  string rootLabel = "program";
  root = this->tree->buildTree(rootLabel);
  this->tree->printNode(root);

  Node *varsNode;
  varsNode = vars();
  root->nodes.push_back(varsNode);
  this->tree->printNode(varsNode);
  if(this->token->tokenInstance == "main") {
    this->token = this->getTokenFromScanner();
    Node *blockNode;
    blockNode = block();
    root->nodes.push_back(blockNode);
    return root;
  }
  else
    this->throwError("Error: 'main' expected in program");
}
Node* Parser::block()
{
  Node *subRoot;
  subRoot = this->tree->insertNode("block");
  // begin <vars><stats> end
  if(this->token->tokenInstance == "begin") {
    subRoot->tokens.push_back(this->token);
    this->token = this->getTokenFromScanner();
    Node *varsNode, *statsNode;
    varsNode = vars();
    subRoot->nodes.push_back(varsNode);
    statsNode = stats();
    subRoot->nodes.push_back(statsNode);
    // this->token = this->getTokenFromScanner();
    if(this->token->tokenInstance == "end") {
      subRoot->tokens.push_back(this->token);
      this->token = this->getTokenFromScanner();
      return subRoot;
    }
    else
      this->throwError("Error: 'end' expected after block");
  }
  else
    this->throwError("Error: 'begin' expected at the start of a block");

  return subRoot;
}
Node* Parser::vars()
{
  Node *subRoot;
  subRoot = this->tree->insertNode("vars");

  // empty | data Identifier := Integer ; <vars>
  // 1. If empty, return;
  if(this->token->tokenInstance == "") {
    return subRoot;
  }
  // 2. If data identifier, process token, (see semi-colon?), call vars() again
  if(this->token->tokenInstance == "data") {
    subRoot->tokens.push_back(this->token);
    this->token = this->getTokenFromScanner();
    if(this->token->tokenID == IDENT_tk) {
      subRoot->tokens.push_back(this->token);
      this->token = this->getTokenFromScanner();
      if(this->token->tokenInstance == ":=") {
        subRoot->tokens.push_back(this->token);
        this->token = this->getTokenFromScanner();
        // Check that next is an integer
        if(this->token->tokenID == NUM_tk) {
          subRoot->tokens.push_back(this->token);
          this->token = this->getTokenFromScanner();
          if(this->token->tokenInstance == ";") {
            subRoot->tokens.push_back(this->token);
            this->token = this->getTokenFromScanner();
            Node *varsNode;
            varsNode = vars();
            subRoot->nodes.push_back(varsNode);
            return subRoot;
          }
          else
            this->throwError("Error: ';' missing after Integer in variable declaration");
        }
        else
          this->throwError("Error: Integer expected after ':=' in variable declarations");
      }
      else
        this->throwError("Error: ':=' expected after Identifier in variable declarations");
    }
    else
      this->throwError("Error: Identifier expected after 'data' in variable declarations");
  }
  else {
    return subRoot;
    // cout << "Error: 'data' expected at the beginning of a variable declaration" << endl;
  }
}
Node* Parser::expr()
{
  Node *subRoot;
  subRoot = this->tree->insertNode("expr");
  // <N> - <expr> | <N>
  // 1. Calculate First Set
  // firstSetOfN = {"*", "(", IDENT_tk, NUM_tk} // is * or R() -> (<expr) | Identifier | Integer
  // string firstSetOfN = {"*", "("};
  // so if firstSetOfN.includes(token->tokenInstace), call just N()

  // 2. Either <N> - <expr> or <N>
  Node *nNode;
  nNode = N();
  subRoot->nodes.push_back(nNode);
  // this->token = this->getTokenFromScanner();

  if (this->token->tokenInstance == "-") // && this->token->tokenID == OpDelim_tk
  {
    subRoot->tokens.push_back(this->token);
    this->token = this->getTokenFromScanner();
    Node *exprToken;
    exprToken = expr();
    return subRoot;
  }
  else {
    return subRoot;
  }
}

Node* Parser::N()
{
  Node *subRoot;
  subRoot = this->tree->insertNode("N");
  // <A> / <N> | <A> * <N> | <A>
  cout << "N()" << endl;

  Node *aNode;
  aNode = A();
  subRoot->nodes.push_back(aNode);

  if (this->token->tokenInstance == "/" || this->token->tokenInstance == "*")
  {
    // 1. handle / or * sign
    subRoot->tokens.push_back(this->token);
    this->token = this->getTokenFromScanner();
    // 2. Call N
    Node *nNode;
    nNode = N();
    subRoot->nodes.push_back(nNode);
    return subRoot;
  }
  else
    return subRoot;
}
Node* Parser::A()
{
  Node *subRoot;
  subRoot = this->tree->createNode("A");
  // <M> + <A> | <M>
  // 1. User First Sets and Lookahead to see if M + A or M
  Node *mNode;
  mNode = M();
  subRoot->nodes.push_back(mNode);
  // this->token = this->getTokenFromScanner();
  if (this->token->tokenInstance == "+") {
    subRoot->tokens.push_back(this->token);
    this->token = this->getTokenFromScanner();
    Node *aNode;
    aNode = A();
    subRoot->nodes.push_back(aNode);
    return subRoot;
  }
  else
    return subRoot;
}
Node* Parser::M()
{
  Node *subRoot;
  subRoot = this->tree->createNode("M");
  // * <M> | <R>
  // 1. if mult terminal, consume it and call M
  this->printToken(this->token);
  if (this->token->tokenInstance == "*") {
    subRoot->tokens.push_back(this->token);
    this->token = this->getTokenFromScanner();
    Node *mNode;
    mNode = M();
    subRoot->nodes.push_back(mNode);
    return subRoot;
  }
  else {
    Node *rNode;
    rNode = R();
    subRoot->nodes.push_back(rNode);
    return subRoot;
  }
}
Node* Parser::R()
{
  Node *subRoot;
  subRoot = this->tree->createNode("R");
  // (<expr) | Identifier | Integer

  // 1. If parens, call expr,
  if (this->token->tokenInstance == "(") {
    subRoot->tokens.push_back(this->token);
    this->token = this->getTokenFromScanner();
    Node *exprNode;
    exprNode = expr();
    subRoot->nodes.push_back(exprNode);
    if(this->token->tokenInstance == ")") {
      // Add to subRoot as a token
      subRoot->tokens.push_back(this->token);
      this->token = this->getTokenFromScanner();
      return subRoot;
    }
    else
      this->throwError("Error: Missing closing ')' from R");
  }
  else if(this->token->tokenID == IDENT_tk) {
    subRoot->tokens.push_back(this->token);
    this->token = this->getTokenFromScanner();
    return subRoot;
  }
  else if(this->token->tokenID == NUM_tk) {
    subRoot->tokens.push_back(this->token);
    this->token = this->getTokenFromScanner();
    return subRoot;
  }
  else {
    cout << "About to throw error. Unrecognized token: " << endl;
    this->printToken(this->token);
    this->throwError("Error: Unrecognized token in R()");
    return subRoot;
  }
}
Node* Parser::stats()
{
  Node *subRoot;
  subRoot = this->tree->createNode("stats");
  // <stat> <mStat>
  Node *statNode, *mStatNode;
  statNode = stat();
  subRoot->nodes.push_back(statNode);
  mStatNode = mStat();
  subRoot->nodes.push_back(mStatNode);

  return subRoot;
}
Node* Parser::mStat()
{
  Node *subRoot;
  subRoot = this->tree->createNode("mStat");
  // empty | <stat> <mStat>
  // 1. Use lookahead to detect EOF token
  if (this->token->tokenInstance == "") {
    return subRoot;
  }
  Node *statNode;
  statNode = stat();
  subRoot->nodes.push_back(statNode);
  if(this->token->tokenInstance == "end") {
    return subRoot;
  }
  Node *mStatNode;
  mStatNode = mStat();
  subRoot->nodes.push_back(mStatNode);

  return subRoot;
}
Node* Parser::stat()
{
  Node *subRoot;
  subRoot = this->tree->createNode("stat");
  // <in>; | <out>; | <block> | <if> ; | <loop> ; | <assign> ; | <goto> ; | <label> ;
  // 1. Check which category the token is in
  if(this->token->tokenInstance == "begin") {
    Node *blockNode;
    blockNode = block();
    subRoot->nodes.push_back(blockNode);
    // this->token = this->getTokenFromScanner();
    return subRoot;
  }
  else if(this->token->tokenInstance == "end") {
    return subRoot;
  }
  else {
    Node *node;
    if(this->token->tokenInstance == "getter") {
      node = in();
    }
    else if (this->token->tokenInstance == "outter") {
      node = out();
    }
    else if(this->token->tokenInstance == "if") {
      node = _if();
    }
    else if(this->token->tokenInstance == "loop") {
      node = loop();
    }
    else if (this->token->tokenInstance == "assign") {
      node = assign();
    }
    else if(this->token->tokenInstance == "proc") {
      node = _goto();
    }
    else if(this->token->tokenInstance == "void") {
      node = label();
    }
    else {
      this->throwError("Error: Token could not be identified (yet)");
      this->printToken(this->token);
      return subRoot;
    }
    subRoot->nodes.push_back(node);

    // Since these aren't blocks, they would all require semi-colons at the end
    // this->token = this->getTokenFromScanner();
    if(this->token->tokenInstance == ";") {
      this->token = this->getTokenFromScanner();
      return subRoot;
    }
    else {
      this->throwError("Error: ';' expected");
      return subRoot;
    }
  }
}

Node* Parser::in()
{
  Node *subRoot;
  subRoot = this->tree->createNode("in");
  // getter Identifier (??)
  // cout << "'in' getter Identifier" << endl;
  if(this->token->tokenInstance == "getter") {
    subRoot->tokens.push_back(this->token);
    this->token = this->getTokenFromScanner();
    if(this->token->tokenID == IDENT_tk) {
      subRoot->tokens.push_back(this->token);
      this->token = this->getTokenFromScanner();
      return subRoot;
    }
    else
      this->throwError("Error: Identifier expected after 'in' keyword");
  }
  else
    this->throwError("Error: 'in' missing from in statement(?)");
}
Node* Parser::out()
{
  Node *subRoot;
  subRoot = this->tree->createNode("out");
  // outter <expr>
  // cout << "'outter' expr()" << endl;
  if(this->token->tokenInstance == "outter") {
    subRoot->tokens.push_back(this->token);
    this->token = this->getTokenFromScanner();
    Node *exprNode;
    exprNode = expr();
    subRoot->nodes.push_back(exprNode);
    return subRoot;
  }
  else
    this->throwError("Error: 'outter' missing from outter statement(?)");
}
Node* Parser::_if()
{
  Node *subRoot;
  subRoot = this->tree->createNode("if");
  // if [ <expr> <RO> <expr> ] then <stat>
  // cout << "'if' identifier" << endl;
  // cout << "[ expected" << endl;
  if(this->token->tokenInstance == "if") {
    subRoot->tokens.push_back(this->token);
    this->token = this->getTokenFromScanner();
    if(this->token->tokenInstance == "[") {
      subRoot->tokens.push_back(this->token);
      Node *exprNode, *roNode;
      exprNode = expr();
      subRoot->nodes.push_back(exprNode);
      roNode = RO();
      subRoot->nodes.push_back(roNode);
      exprNode = expr();
      exprNode->nodes.push_back(exprNode);
      this->token = this->getTokenFromScanner();
      if(this->token->tokenInstance == "]") {
        subRoot->tokens.push_back(this->token);
        this->token = this->getTokenFromScanner();
        if(this->token->tokenInstance == "then") {
          subRoot->tokens.push_back(this->token);
          this->token = this->getTokenFromScanner();
          Node *statNode;
          statNode = stat();
          subRoot->nodes.push_back(statNode);
          return subRoot;
        }
        else
          this->throwError("Error: 'then' missing from if statement");
      }
      else
        this->throwError("Error: ']' missing from if statement");
    }
    else
      this->throwError("Error: '[' missing from if statement");
  }
  else
    this->throwError("Error: 'if' missing from if statement(?)");
}
Node* Parser::loop()
{
  Node *subRoot;
  subRoot = this->tree->createNode("loop");
  // loop [ <expr> <RO> <expr> ] <stat>
  // cout << "'loop' identifier" << endl;
  // cout << "[ expected" << endl;
  if(this->token->tokenInstance == "loop") {
    subRoot->tokens.push_back(this->token);
    this->token = this->getTokenFromScanner();
    if(this->token->tokenInstance == "[") {
      subRoot->tokens.push_back(this->token);
      this->token = this->getTokenFromScanner();
      Node *exprNode, *roNode;
      exprNode = expr();
      subRoot->nodes.push_back(exprNode);
      roNode = RO();
      subRoot->nodes.push_back(roNode);
      exprNode = expr();
      subRoot->nodes.push_back(exprNode);

      // Process token? subRoot->tokens.push_back(this->token);
      if(this->token->tokenInstance == "]") {
        subRoot->tokens.push_back(this->token);
        this->token = this->getTokenFromScanner();
        this->printToken(this->token);
        Node *statNode;
        statNode = stat();
        subRoot->nodes.push_back(statNode);
        return subRoot;
      }
      else
        this->throwError("Error: ']' missing from loop statement");
    }
    else
      this->throwError("Error: '[' missing from loop statement");
  }
  else
    this->throwError("Error: 'loop' missing from loop statement(?)");
}
Node* Parser::assign()
{
  Node *subRoot;
  subRoot = this->tree->createNode("assign");
  // assign Identifier := <expr>
  // cout << "'assign' keyword" << endl;
  // cout << "identifier expected" << endl;
  // cout << ":= expected" << endl;
  if(this->token->tokenInstance == "assign") {
    subRoot->tokens.push_back(this->token);
    this->token = this->getTokenFromScanner();
    if(this->token->tokenID == IDENT_tk) {
      subRoot->tokens.push_back(this->token);
      this->token = this->getTokenFromScanner();
      if(this->token->tokenInstance == ":=") {
        subRoot->tokens.push_back(this->token);
        this->token = this->getTokenFromScanner();
        Node *exprNode;
        exprNode = expr();
        subRoot->nodes.push_back(exprNode);
        return subRoot;
      }
      else
        this->throwError("Error: ':=' missing from assign statement");
    }
    else
      this->throwError("Error: Identifier expected after assign keyword.");
  }
  else
    this->throwError("Error: 'assign' missing from assign statement(?)");
}
Node* Parser::RO()
{
  Node *subRoot;
  subRoot = this->tree->createNode("RO");
  // => | =< | == | [==] (3 tokens) | %
  if (this->token->tokenInstance == "=>")
  {
    subRoot->tokens.push_back(this->token);
    this->token = this->getTokenFromScanner();
    return subRoot;
  }
  else if (this->token->tokenInstance == "=<")
  {
    subRoot->tokens.push_back(this->token);
    this->token = this->getTokenFromScanner();
    return subRoot;
  }
  else if (this->token->tokenInstance == "==")
  {
    subRoot->tokens.push_back(this->token);
    this->token = this->getTokenFromScanner();
    return subRoot;
  }
  else if (this->token->tokenInstance == "[" || this->token->tokenInstance == "==" || this->token->tokenInstance == "]")
  {
    subRoot->tokens.push_back(this->token);
    this->token = this->getTokenFromScanner();
    return subRoot;
  }
  else if (this->token->tokenInstance == "%")
  {
    subRoot->tokens.push_back(this->token);
    this->token = this->getTokenFromScanner();
    return subRoot;
  }
  else
  {
    this->throwError("Error: Symbol Did Not Match. Symbol: " + this->token->tokenInstance + "\n");
    return subRoot;
  }
}
Node* Parser::label()
{
  Node *subRoot;
  subRoot = this->tree->createNode("label");
  // void Identifier
  // cout << "'void' keyword expected, followed by Identifier" << endl;
  if(this->token->tokenInstance == "void") {
    subRoot->tokens.push_back(this->token);
    // Next token should be identifier, or error
    this->token = this->getTokenFromScanner();
    if(this->token->tokenID == IDENT_tk) {
      subRoot->tokens.push_back(this->token);
      this->token = this->getTokenFromScanner();
      return subRoot;
    }
    else {
      this->throwError("Error: next token is NOT identifier, we are NOT okay.");
    }
  }
  else {
    this->throwError("Error: 'void' not detected in label()");
  }
}
Node* Parser::_goto()
{
  Node *subRoot;
  subRoot = this->tree->createNode("goto");
  // proc Identifier
  if(this->token->tokenInstance == "proc") {
    subRoot->tokens.push_back(this->token);
    // Next token should be identifier, or error
    this->token = this->getTokenFromScanner();
    if(this->token->tokenID == IDENT_tk) {
      subRoot->tokens.push_back(this->token);
      this->token = this->getTokenFromScanner();
      return subRoot;
    }
    else {
      this->throwError("Error: next token is NOT identifier, we are NOT okay.");
    }
  }
  else {
    this->throwError("Error: 'proc' not detected in _goto()");
  }
}

void Parser::printToken(Token *token)
{
  Scanner scanner;

  if (!token)
    cout << "LOL that token doesn't exist." << endl;
  else
  {
    cout << "line " << token->lineNumber << ": "
         << token->tokenInstance
         << "\tid: " << token->tokenID << " "
         << scanner.tokenNames[token->tokenID] << "\n"
         << endl;
  }
}

Token* Parser::getTokenFromScanner() {
  Token *token;

  token = this->scanner->getToken(this->input);

  // cout << "new token: ";
  // this->printToken(token);

  return token;
}

bool Parser::isEofToken(Token *token) {
  if(token->tokenID == EOF_tk) {
    return true;
  }
  return false;
}

void Parser::throwError(string message) {
  throw invalid_argument(message);
}