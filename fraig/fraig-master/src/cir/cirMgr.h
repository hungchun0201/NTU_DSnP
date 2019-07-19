/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <bitset>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;

class CirMgr
{
public:
   CirMgr() :_aigCount(0),_isSim(0){}
    ~CirMgr() {reset();}

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const { if(gid>header[0]+header[3])return 0;return _gates[gid]; }

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;
   IdList* getFEC(unsigned i){return _FEC[i];}
   GateList& getGates(){return _gates;}

private:
    ofstream           *_simLog;
    GateList _gates;
    unsigned header[5],_aigCount;
    IdList _pis,_pos,_DFS;
    FECGroups _FEC;
    bool _isSim;
    void reset(){
      for(GateList::iterator it=_gates.begin();it!=_gates.end();it++){
         if(*it!=0)delete *it;
        }
      _gates.clear();
      _pis.clear();
      _pos.clear();
      _aigCount=0;
      _DFS.clear();
      for(unsigned i=0;i<_FEC.size();i++){
        if(_FEC[i]!=0)delete _FEC[i];
      }
      _FEC.clear();
      _isSim=0;
    };
    bool readHeader(ifstream&);
    bool readInput(ifstream&);
    bool readOutput(ifstream&);
    bool readAig(ifstream&);
    bool readId(ifstream&);
    void searchGates(IdList &,unsigned =-1) const ;
    int simPattern();
    void initialSim();
    void generateIn(bitset<64>*);
    bool checkPattern(const string&);
    void insertGroupNo();
    bool fraigPair(unsigned,unsigned);
    void setBase(IdList&);
    void cleanFEC();
    void simBatch(vector<string>&,unsigned);
};

#endif // CIR_MGR_H
