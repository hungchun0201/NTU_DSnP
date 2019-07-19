/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include <regex>
#include "p2Json.h"

using namespace std;

int main()
{
  Json json;

  // Read in the csv file. Do NOT change this part of code.
  string jsonFile;
  cout << "Please enter the file name: ";
  cin >> jsonFile;
  if (json.read(jsonFile))
    cout << "File \"" << jsonFile << "\" was read in successfully." << endl;
  else
  {
    cerr << "Failed to read in file \"" << jsonFile << "\"!" << endl;
    exit(-1); // jsonFile does not exist.
  }

  // TODO read and execute commands
  // cout << "Enter command: ";
  string command;
  while (true)
  {
    regex test("^ADD");
    cout << "Enter command:" << endl;
    // cout << "Enter command: ";
    cin >> command;

    
    {
      if (command == "PRINT")
      {
        cout << '{' << endl;
        json.print();
        cout << '}' << endl;
      }
      else if (command == "SUM")
      {
        if (json.is_empty())
        {
          cout << "Error: No element found!!" << endl;
          goto again;
        }
        json.Sum();
      }
      else if (command == "AVE")
      {
        if (json.is_empty())
        {
          cout << "Error: No element found!!" << endl;
          goto again;
        }
        json.ave();
      }
      else if (command == "MAX")
      {
        if (json.is_empty())
        {
          cout << "Error: No element found!!" << endl;
          goto again;
        }
        json.find_max();
      }
      else if (command == "MIN")
      {
        if (json.is_empty())
        {
          cout << "Error: No element found!!" << endl;
          goto again;
        }
        json.find_min();
      }
      else if (command == "EXIT")
      {
        return 0;
      }
      else if (regex_match(command, test))
      {
        string name, number;
        cin >> name;
        cin >> number;
        json.add_in(name, number);
      }
      else
      {
        again:;
      }
    }
  }
}
