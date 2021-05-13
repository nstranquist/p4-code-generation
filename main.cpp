#include <string>
#include <istream>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdexcept>

#include "parser.h"
#include "node.h"
#include "printTree.h"
#include "symbolTable.h"

using namespace std;

// TODO: Account for the c-string error and <stdio.h> error Hauschild found in P1 (and fix it)
int main(int argc, char *argv[])
{
  // 1. Implement All Checks on inputs and stuff before calling parser
  istream *in;
  ifstream out;

  if (argc == 1)
  {
    in = &cin;
    cout << "\n0. Please enter the input for the program. Finish by entering a new line." << endl;
  }
  // Argument given, open the file and use it
  else if (argc == 2)
  {
    string fileName = argv[1];
    out.open(fileName.c_str());
    in = &out;

    cout << "\n0. Reading your file...\n\n";
    if (!out.is_open())
    {
      // Before error, check if .fs is being added or not
      if (fileName.find(".fs") == std::string::npos)
      {
        // try one more time to open the file
        fileName = fileName = ".fs";
        out.open(fileName.c_str());
        in = &out;

        if (!out.is_open())
        {
          cout << "Error: Your file could not be read. It probably does not exist." << endl;
          return 1;
        }
        else
        {
          cout << "proceeding with file" << endl;
        }
      }
      else
      {
        cout << "Error: Your file could not be read. It probably does not exist." << endl;
        return 1;
      }
    }
    else if (out.peek() == ifstream::traits_type::eof())
    {
      cout << "Error: Your file is empty, so it could not be used." << endl;
      return 1;
    }
    else
    {
      cout << "File has been opened successfully.\n\n";
    }
  }
  else
  {
    cout << "Error: Too many arguments given. Please retry with 1 or 2 arguments (including the command)." << endl;
    return 1;
  }

  // 2. Call parser() from parser.cpp
  Parser parser;
  Node *root_node;

  try {
    root_node = parser.parser(in);
    // 3. Call printTree() from printTree.cpp
    PrintTree printTree;
    printTree.printTree(root_node);

    printTree.semanticAnalyze(root_node);

    return 0;
  }
  catch (invalid_argument &e) {
    cerr << e.what() << endl;
    return -1;
  }
}
