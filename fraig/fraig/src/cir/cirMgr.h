/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>

using namespace std;

#include "cirDef.h"
#include "sat.h"
extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
  friend class CirGate;
public:
  CirMgr() {}
  ~CirMgr() {}

  // Access functions
  // return '0' if "gid" corresponds to an undefined gate.
  CirGate *getGate(unsigned gid) const
  {
    return (gid <= Circuit.maxid + Circuit.outputs) ? Circuit.id2Gate[gid] : 0;
  }

  // Member functions about circuit construction
  bool readCircuit(const string &);

  // Member functions about circuit reporting
  void printSummary() const;
  void printNetlist();
  void printPIs() const;
  void printPOs() const;
  void printFloatGates() const;
  void printFECPairs(); //new added
  void writeAag(ostream &) const;
  class ParsedCir
  {
  public:
    ParsedCir()
        : maxid(0),
          inputs(0),
          outputs(0),
          ands(0),
          id2Gate(0),
          latches(0),
          PI_list(0) {}
    ~ParsedCir()
    {
      // for (size_t i = 0; i < maxid + outputs; i++)
      // {
      //    if (id2Gate[i] != 0)
      //       delete id2Gate[i];
      // }
      delete[] id2Gate;
      delete[] PI_list;
    }
    void writeAig(int id, vector<unsigned> &AIGlist) const;
    size_t inputs, outputs, ands, maxid, latches;
    //stored with id
    unsigned *PI_list;
    mutable vector<bool> DFS_List_booltype;
    mutable IdList DFS_List_unsignedtype;
    mutable Hashtable DFS_List_hash;
    unordered_map<size_t, IdList *> FECgroups;
    bool has_sim;
    //index is id
    CirGate **id2Gate;
    //every gate is stored by a pointer,Cirgate*.
    //using Cirgate** as a vector to store all of the gate.
  };
  ParsedCir Circuit;
  vector<IdList *> FECFinal_vec;
  void writeAig(int id, vector<unsigned> &AIGlist) const;
  //
  void writeGate(ostream &outfile, CirGate *g) const;

  // Member functions about circuit optimization
  void sweep();
  void optimize();

  // Member functions about simulation
  void randomSim();
  void fileSim(ifstream &);
  void setSimLog(ofstream *logFile) { _simLog = logFile; }

  // Member functions about fraig
  void strash();
  void printFEC() const;
  void fraig();

  void Consult_FECGroup(unsigned id);
  

private:
  ofstream *_simLog;
  SatSolver solver;
  void DFSTravPO(unsigned, unsigned &, DFS_utilize);
  void printNetlistformat(unsigned, unsigned);
  bool ParseHeader(ifstream &);
  void HeaderError(string &errstr);
  bool GenGates(ifstream &);
  bool ConstructCir();
  bool Delete_gate(unsigned);
  bool DFS_for_sweep();
  bool DFS_for_optimize();
  bool DFS_for_strash();
  
  

  //utilize for optimize and strash
  bool append_fanout(unsigned target_lit, unsigned delete_id);
  bool replace_fanin_of_fanout(IdList &fanout, unsigned init_id, unsigned after_lit);
  bool erase_fanout_data(vector<unsigned> &vec, unsigned _data);
  inline void printOptimizeInformation(unsigned, unsigned);
  void mergeAndDeleteGate(unsigned, unsigned,DFS_utilize);

  void patternTrans(vector<string> &, int);
  void ConstructSimCircuit_and_FECgroups();
  void writeToLog(ostream &);
  static bool compare_IdList(IdList *id1, IdList *id2)
  {
    return (id1->front() < id2->front());
  }
  static bool compare_map(pair<size_t, size_t> lhs, pair<size_t, size_t> rhs)
  {
    return (lhs.second > rhs.second);
  }

  inline unsigned inverse_func(unsigned init_lit)
  {
    return (init_lit % 2 == 0 ? init_lit + 1 : init_lit - 1);
  }
  inline void inverse_func_ref(unsigned &);
  inline size_t pow(unsigned, int);

//fraig
  void genProofModel();
  void execute_fraig(Var&,SatSolver&,IdList*);
};

class fEC
{
};
#endif // CIR_MGR_H
