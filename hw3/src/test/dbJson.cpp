/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream &
operator<<(ostream &os, const DBJsonElem &j)
{
  os << "\"" << j._key << "\" : " << j._value;
  return os;
}

istream &operator>>(istream &is, DBJson &j)
{
  // TODO: to read in data from Json file and store them in a DB
  // - You can assume the input file is with correct JSON file format
  // - NO NEED to handle error file format
  string input;
  regex reg("[ \t]*\"([\\w]+)\"[ \t]*:{1}[ \t]*(-?[0-9]+)[ \t]*");
  smatch sm;

  while (getline(is, input, '\n'))
  {
    
    bool match = regex_search(input, sm, reg);
    // cout << (match ? "Matched" : "NOT") << endl;
    // cout << input << endl;
    if (!match)
    {
      if(input == "}")
      {
        return is;
      }
      continue;
    }
    // for (int i = 0; i < (int)sm.size(); ++i)
    // {
    //   cout << "sm[" << i << "] = " << sm[i] << endl;
    // }
    int value = 0;
    stringstream ss(sm[2].str());
    ss >> value;
    // cout << "the value is: " << value << endl;
    DBJsonElem temp(sm[1].str(), value);
    j._obj.push_back(temp);
    // for (int i = 0; i < (int)j._obj.size(); i++)
    //   cout << j._obj[i] << " " << endl;
  }

  //assert(j._obj.empty());
  return is;
}

ostream &operator<<(ostream &os, const DBJson &j)
{
  //TODO
  
  for (int i = 0; i < (int)j._obj.size(); i++)
    os <<"\t"<< j._obj[i] << " " << endl;
  
  return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
void DBJson::reset()
{
  // TODO
  _obj.clear();
}

// return false if key is repeated
bool DBJson::add(const DBJsonElem &elm)
{
  // TODO:
  cout << "please enter the key and value" << endl;
  string key_add;
  int value_add;

  key_add = elm.key();
  value_add = elm.value();
  for (int i = 0; i < (int)_obj.size(); i++)
  {
    if (key_add == _obj[i].key())
    {
      cout << "the key is repeat" << endl;
      return false;
    }
  }
  cout << key_add << "\t" << value_add << endl;
  _obj.push_back(elm);
  return true;
}

// return NAN if DBJson is empty
float DBJson::ave() const
{
  // TODO:
  float ave = 0.0;
  if (_obj.size() == 0)
  {
    return NAN;
  }
  else
    ave = (float)sum() / (float)(_obj.size());
  //cout<<fixed<<setprecision(2)<<ave<<endl;
  return ave;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int DBJson::max(size_t &idx) const
{
  // TODO
  int maxN = INT_MIN;
  if (_obj.empty())
  {
    idx = _obj.size();
    return INT_MIN;
  }
  else
  {
    maxN = _obj[0].value();
    idx = 0;
    for (int i = 1; i < (int)_obj.size(); i++)
    {
      if (_obj[i].value() > maxN)
      {
        maxN = _obj[i].value();
        idx = i;
      }
    }
    cout << maxN << endl;
    cout << "index" << idx << endl;
  }
  return maxN;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int DBJson::min(size_t &idx) const
{
  // TODO

  int minN = INT_MAX;
  if (_obj.empty())
  {
    idx = _obj.size();
    return INT_MAX;
  }
  else
  {
    minN = _obj[0].value();
    idx = 0;
    for (int i = 1; i < (int)_obj.size(); i++)
    {
      if (_obj[i].value() < minN)
      {
        minN = _obj[i].value();
        idx = i;
      }
    }
    cout << minN << endl;
    cout << "index" << idx << endl;
    return minN;
  }
}

void DBJson::sort(const DBSortKey &s)
{
  // Sort the data according to the order of columns in 's'
  ::sort(_obj.begin(), _obj.end(), s);
}

void DBJson::sort(const DBSortValue &s)
{
  // Sort the data according to the order of columns in 's'
  ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int DBJson::sum() const
{
  // TODO:
  int s = 0;
  for (int i = 0; i < (int)_obj.size(); ++i)
    s += _obj[i].value();
  return s;
}
