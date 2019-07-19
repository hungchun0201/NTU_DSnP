/****************************************************************************
  FileName     [ p2Json.h]
  PackageName  [ p2 ]
  Synopsis     [ Header file for class Json JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef P2_JSON_H
#define P2_JSON_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <fstream>

using namespace std;

class JsonElem
{
public:
  // TODO: define constructor & member functions on your own
  JsonElem() {}
  JsonElem(const string &k, int v) : _key(k), _value(v) {}
  int getint() { return _value; }

  friend ostream &operator<<(ostream &, const JsonElem &);

private:
  string _key; // DO NOT change this definition. Use it to store key.
  int _value;  // DO NOT change this definition. Use it to store value.
};

class Json
{
public:
  // TODO: define constructor & member functions on your own
  int sum=0;
  int convertion=0;
  bool read(const string &);
  bool print();
  bool Sum();
  bool ave();
  bool find_max();
  bool find_min();
  bool add_in(string ,string);
  bool is_empty();

private:
  vector<JsonElem> _obj; // DO NOT change this definition.
                         // Use it to store JSON elements.
};

#endif // P2_TABLE_H
