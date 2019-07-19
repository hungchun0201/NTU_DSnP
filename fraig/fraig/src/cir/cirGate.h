/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include "cirDef.h"
#include "sat.h"

using namespace std;

class CirGate;
bool compare_IdList(IdList &i, IdList &);
//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
// Not yet remembering the lineNo.
class CirGate
{
public:
  friend class CirMgr;
  CirGate() : _ref(0), _lineNo(0), _fanin(0), sim_pattern(0) {}
  virtual ~CirGate() { delete[] _fanin; }

  static unsigned executing_id;
  //add in fraig
  bool isAig() { return false; }

  // Basic access methods
  // string getTypeStr() const { return ""; }
  virtual string getTypeStr() const = 0;
  virtual GateType getType() const = 0;
  unsigned getLineNo() const { return _lineNo; }
  void setLineNo(unsigned n) { _lineNo = n; }
  void setVar(Var v){_v = v;}
  Var getVar(){return _v;}
  unsigned *getFanin() const { return _fanin; }
  vector<unsigned> &getFanout() { return _fanout; } //alias of _fanout,so we can change it directly
  static void setGlobalref() { _globalref++; }
  bool isGlobalref() const { return (_globalref == _ref); }
  void setRefToGlobalRef() const { _ref = _globalref; }
  void print_Value()const;
  string printBinSimVal(const size_t &val) const;

  size_t getSim_pattern()
  {
    return sim_pattern;
  }

  // Printing functions
  virtual void printGate() const = 0;
  //add gateId for report gate
  void reportGate(int id,CirMgr* mgr) const;
  void reportFanin(int level, int id) const;
  void reportFanout(int level, int id) const;
  size_t sim_pattern;

private:
  void FaninDFS(int, unsigned, unsigned) const;
  void FanoutDFS(int, unsigned, unsigned) const;

protected:
  // using literal
  vector<unsigned> _fanout;
  unsigned *_fanin;
  unsigned _lineNo;
  mutable unsigned _ref;
  static unsigned _globalref;
  Var _v;
};
class SymbolGate : public CirGate
{
public:
  SymbolGate() : CirGate(), _symbol(0) {}
  virtual ~SymbolGate() { delete[] _symbol; }
  void setSymbol(const string &s)
  {
    _symbol = new char[s.size() + 1]();
    strcpy(_symbol, s.c_str());
  }
  const char *getSymbol() const { return _symbol; }

private:
  char *_symbol;
};
class InputGate : public SymbolGate
{
public:
  InputGate() : SymbolGate() {}
  string getTypeStr() const { return "PI"; }
  GateType getType() const { return PI_GATE; }
  void printGate() const { cout << getTypeStr(); }

private:
  // using literal
};
class OutputGate : public SymbolGate
{
public:
  OutputGate(unsigned in) : SymbolGate() { _fanin = new unsigned(in); } //store exactly the num in .aag
  string getTypeStr() const { return "PO"; }
  GateType getType() const { return PO_GATE; }
  void printGate() const { cout << getTypeStr(); }

private:
  // using literal
};
class AndGate : public CirGate
{
public:
  AndGate() : CirGate() {}
  AndGate(unsigned fan1, unsigned fan2) : CirGate()
  {
    _fanin = new unsigned[2];
    _fanin[0] = fan1;
    _fanin[1] = fan2;
  }
  void printGate() const { cout << getTypeStr(); }
  string getTypeStr() const { return "AIG"; }
  GateType getType() const { return AIG_GATE; }

private:
  // using literal
};
class ConstGate : public CirGate
{
public:
  ConstGate() : CirGate() {}
  void printGate() const { cout << getTypeStr(); }
  string getTypeStr() const { return "CONST"; }
  GateType getType() const { return CONST_GATE; }

private:
  // using literal
};

class UndefGate : public CirGate
{
public:
  UndefGate() : CirGate() {}
  void printGate() const { cout << getTypeStr(); }
  string getTypeStr() const { return "UNDEF"; }
  GateType getType() const { return UNDEF_GATE; }

private:
};

#endif // CIR_GATE_H
