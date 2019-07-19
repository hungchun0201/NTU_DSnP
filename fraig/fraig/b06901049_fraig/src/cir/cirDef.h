/****************************************************************************
  FileName     [ cirDef.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic data or var for cir package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2010-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_DEF_H
#define CIR_DEF_H

#include <vector>
#include <algorithm>
#include <unordered_map>

using namespace std;

class CirGate;
class CirMgr;
class Strash_fanin : public pair<unsigned,unsigned>
{
  public:
  Strash_fanin(unsigned num1,unsigned num2)
  {
    this->first = num1;
    this->second = num2;
  }
  bool operator==(const Strash_fanin &comp) const
    {
        if (this->first == comp.first&&this->second == comp.second) return true;
        else if ( this->first == comp.second&&this->second == comp.first) return true;
        else return false;
    }
};
class Strash_hash_func
{
  public:
  size_t operator()(const Strash_fanin& _fanin_pair)const
  {
    return hash<unsigned>()(_fanin_pair.first)+hash<unsigned>()(_fanin_pair.second);
  }
};
typedef unordered_map<Strash_fanin,unsigned,Strash_hash_func> Hashtable;


typedef vector<CirGate *> GateList;
typedef vector<unsigned> IdList;



enum GateType
{
  UNDEF_GATE = 0,
  PI_GATE = 1,
  PO_GATE = 2,
  AIG_GATE = 3,
  CONST_GATE = 4,

  TOT_GATE
};

enum DFS_utilize
{
  PRINT = 0,
  SWEEP = 1,
  OPTIMIZE = 2,
  STRASH = 3,
  SIMULATION = 4,
  FRAIG = 5,
};

#endif // CIR_DEF_H
