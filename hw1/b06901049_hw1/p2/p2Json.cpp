/****************************************************************************
  FileName     [ p2Json.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include <regex>
#include <iomanip>
#include "p2Json.h"

using namespace std;

// Implement member functions of class Row and Table here
bool Json::read(const string &jsonFile)
{
  {
    fstream myfile;
    myfile.open(jsonFile, ios::in);
    string input, input2, want, want2;
    regex reg(".*\"(.*)(\")(.*)([0-9]*)");
    regex reg2("((-)?[0-9]+)");
    
    if (myfile.is_open())
    {
      
      while (getline(myfile, input, '\n'))
      {
        smatch sm, sm2;
        input2 = input;
        int count = 0;
        while (regex_search(input, sm, reg))
        {
          ++count;
          // cout << "Match " << count << ":\n";
          // cout << "The matches are:\n";
         
          want = sm[1].str();

          input = sm.suffix().str();
        }
        // cout << "input2" << input2 << endl;
        while (regex_search(input2, sm2, reg2))
        {
          // cout << "in while2" << input2 << endl;
          ++count;
          // cout << "Match " << count << ":\n";
          // cout << "The matches are:\n";
          for (int i = 0; i < sm2.size(); ++i)
          {
            // cout << i << ": [" << sm2[i] << ']' << endl;
          }
          want2 = sm2[1].str();

          input2 = sm2.suffix().str();
        }

        if (!want.empty())
        {
          // cout << "want is " << want << endl;
        }
        int want2_2 = 0;
        if (!want2.empty())
        {    
          stringstream ss(want2);        
          ss >> want2_2;      
          // cout << "want2_2 is " << want2_2 << endl;
          JsonElem Element_temp(want,want2_2);
         _obj.push_back(Element_temp);
        //  for(int i=0; i<_obj.size(); i++) cout << _obj[i] << " "<<endl;
        }         
         want.clear();
         want2.clear();
      }
      myfile.close();
      return true; // TODO
    }
    else
    {
      // cout << "Cannot open the file" << endl;
      return false;
    }
  }
}

ostream &
operator<<(ostream &os, const JsonElem &j)
{
  return (os << "\"" << j._key << "\" : " << j._value);
}

bool Json::print()
{
  if(_obj.size()>=1)
  {
  for(int i=0; i<_obj.size()-1; i++) 
  {
    cout << _obj[i] << ","<<endl;
  }
  cout << _obj[_obj.size()-1] <<endl;
  }
  else
  {
    cout<<endl;
  }
}
bool Json::Sum()
{
  sum=0;
  for(int i=0; i<_obj.size(); i++) 
  {
    sum += _obj[i].getint();
  }
  cout<<sum<<endl;
}
bool Json::ave()
{
  sum=0;
  for(int i=0; i<_obj.size(); i++) 
  {
    sum += _obj[i].getint();
  }
  double ave = (double)sum/(double)(_obj.size());
  cout<<fixed<<setprecision(1)<<ave<<endl;
}
bool Json::find_max()
{
  int max=_obj[0].getint();
  for(int i=1; i<_obj.size(); i++) 
  {
    if(_obj[i].getint()>max)
    max = _obj[i].getint();
  }
  cout<<max<<endl;

}
bool Json::find_min()
{
  int min=_obj[0].getint();
  for(int i=1; i<_obj.size(); i++) 
  {
    if(_obj[i].getint()<min)
    min = _obj[i].getint();
  }
  cout<<min<<endl;
}
bool Json::add_in(string name,string number)
{
  stringstream ss(number);
  ss>>convertion;
  JsonElem Element_temp2(name,convertion);
  _obj.push_back(Element_temp2);
}
bool Json::is_empty()
{
  if(_obj.size()==0)
  return true;
  else
  return false;
}