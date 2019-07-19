/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"
#include "sat.h"
#include <utility>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
public:
   CirGate(unsigned n,unsigned g):_lineNo(n),_gateNo(g),_flag(0),_value(0),_groupNo(2*g),_DFSorder(~0){}
   virtual ~CirGate() {}

   // Basic access methods
   string getTypeStr() const { string type[5]={"UNDEF","PI","PO","AIG","CONST"}; return type[_type]; }
   unsigned getLineNo() const { return _lineNo;}
   unsigned getGateNo()const {return _gateNo;}
   bool isAig() const { return _type==AIG_GATE; }
   bool isUnusedUndef(){return _type==UNDEF_GATE && _oNoList.empty();}
   // Printing functions
   virtual void printGate(unsigned & ) const = 0;
   void reportGate() const;
   void reportFanin(int ,unsigned =0,bool =0) const;
   void reportFanout(int ,unsigned =0,bool =0) const;
   void addOut(unsigned p){_oNoList.push_back(p);};
   void addIn(unsigned p){_iNoList.push_back(p);};
   bool isFloat() const ;
   bool notUsed() const {if(_type==UNDEF_GATE || _type==CONST_GATE ||_type==PO_GATE)return 0;return _oNoList.size()==0;};
   IdList& getInNo()  {return _iNoList;}
   GateType getType()const {return _type;}
   bool getFlag()const {return _flag;}
   void setFlag(bool i=0)const{_flag=i;}
   unsigned getSource(){return _iNoList[0];}
   virtual void setId(const string& s) {}
   void setValue(size_t v){_value=v;}
   virtual void sim(){};
   size_t getValue(){return _value;}
   size_t getivValue(){return ~_value;}
   virtual string getId()const {return string();}
   void sweepOut(bool *);
   bool optimize();
   void replaceIn(unsigned,unsigned,bool);//(ori,new,inv)
   void replaceOut(unsigned,const IdList&,bool);
   void replace(unsigned);
   bool optimizeOutput(unsigned);
   size_t getInPair(){
      unsigned a[2];
      if(_iNoList.size()<2)return 0;
      if(_iNoList[0]<_iNoList[1]){a[0]=_iNoList[0];a[1]=_iNoList[1];}
      else {a[0]=_iNoList[1];a[1]=_iNoList[0];}
      size_t *ptr=(size_t*)a;
      return *ptr;
   }
   void setGroupNo(unsigned i){_groupNo=i;}
   unsigned getGroupNo(){return _groupNo;} 
   void setVar(Var v){_var=v;}
   Var getVar(){return _var;}
   virtual void setGateCNF(SatSolver& s,IdList&){}
   void setOrder(unsigned o){_DFSorder=o;}
   unsigned getOrder(){return _DFSorder;}
private:
   
protected:
   const unsigned _lineNo,_gateNo; 
   GateType _type;
   IdList _iNoList,_oNoList;
   mutable bool _flag;
   size_t  _value;
   unsigned _groupNo;
   Var _var;
   unsigned _DFSorder;
};

class UndefGate: public CirGate{
public:
   UndefGate(unsigned n,unsigned g) :CirGate(n,g){ _type=UNDEF_GATE;}
   ~UndefGate(){}
   void printGate(unsigned &) const;
   void setGateCNF(SatSolver& sat,IdList&);
private: 
};
  
class PiGate: public CirGate{
public:
   PiGate(unsigned n,unsigned g) :CirGate(n,g){ _type=PI_GATE;}
   ~PiGate(){}
   void setId(const string& s){_id=s;}
   string getId()const {return _id;}
   void printGate(unsigned &) const;
  private:
   void setGateCNF(SatSolver& sat,IdList&);
   string _id;
};

class PoGate: public CirGate{
public:
   PoGate(unsigned n,unsigned g) :CirGate(n,g){ _type=PO_GATE;}
   ~PoGate(){}
   void setId(const string& s){_id=s;}
   void sim();
   string getId()const {return _id;}
   void printGate(unsigned &) const;
  private:
    string _id;
};

class AigGate: public CirGate{
public: 
   AigGate(unsigned n,unsigned g) :CirGate(n,g){ _type=AIG_GATE;}
   void sim();
   ~AigGate(){}
   void printGate(unsigned &) const;
   void setGateCNF(SatSolver& sat,IdList&);
private:
};

class ConstGate: public CirGate{
public:
   ConstGate(unsigned n,unsigned g) :CirGate(n,g){_type=CONST_GATE;}
   ~ConstGate(){}
   void printGate(unsigned &) const;
   void setGateCNF(SatSolver& sat,IdList&);
private:
};




#endif // CIR_GATE_H
