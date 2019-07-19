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
#include <map>
#include <algorithm>
#include <cassert>

using namespace std;

#include "cirGate.h"
#include "cirDef.h"

// NO need to implement class ArrayNode
//


extern CirMgr *cirMgr;
// TODO: Define your own data members and member functions
class CirMgr
{
public:
  CirMgr()
  {
    Maximal_Variable_Index = 0;
    Input_Number = 0;
    Output_Number = 0;
    AIG_Number = 0;
  }
  ~CirMgr() {}

  // Access functions
  // return '0' if "gid" corresponds to an undefined gate.
  CirGate *getGate(unsigned gid) const
  {
    auto it = GateMap.find(gid);
    if (it == GateMap.end())
    {
      return NULL;
    }
    // if (it->second->type_name == UNDEF_GATE)
    // {
    //   return NULL;
    // }
    return it->second;
    // if (GateMap[gid]->type_name == UNDEF_GATE)
    // {
    //   return NULL;
    // }
  }
  // Member functions about circuit construction
  bool readCircuit(const string &);

  // Member functions about circuit reporting
  void printSummary() const;
  void printNetlist();
  void printPIs() const;
  void printPOs() const;
  void printFloatGates() const;
  void writeAag(ostream &) const;
  inline void printGateInformation(CirGate *) const;
  inline void printAIGGate(CirGate *, Array<unsigned> &) const;
  //datamembers set by myself
  unsigned Maximal_Variable_Index;
  unsigned Input_Number;
  unsigned Output_Number;
  unsigned AIG_Number;
  GateList source_list;
  GateList sink_list;
  map<unsigned, CirGate *> GateMap;
  mutable bool print_floating_fanin;
  mutable bool print_defined_but_not_used;
  mutable unsigned Counting_DFS_AIG_Number;
  static size_t counter_netlist;
  static bool is_UNDEF_0;
  static bool is_UNDEF_1;
  //some member functions

  bool add_PI(unsigned getId, unsigned lineno)
  {
    CirGate *pi = new PI_Gate(getId, lineno);
    GateMap[getId] = pi;
    source_list.push_back(pi);
    return true;
  }
  bool add_PO(unsigned input_id, unsigned lineno, bool inv)
  {
    unsigned sum_id = Maximal_Variable_Index + sink_list.size() + 1;
    sink_list.push_back(new PO_Gate(sum_id, lineno, inv));
    GateMap[sum_id] = sink_list.back();
    CirGate *prev = getGate(input_id); //link to the input gate
    if (prev == NULL)
    {
      prev = new UNDEF_Gate(input_id);
      GateMap[input_id] = prev;
    }
    // sink_list.back()->_faninList.push_back(prev);
    change_connection(GateMap[input_id], input_id, sink_list.back(), sum_id);
    return true;
  }
  bool add_AIG(unsigned gate_id, unsigned lineno, unsigned input1, unsigned input2, bool inv1, bool inv2)
  {
    CirGate *pAIG = new AIG_Gate(gate_id, lineno, inv1, inv2);
    if (GateMap[gate_id] == NULL)
      GateMap[gate_id] = pAIG;
    else
      replace_UNDEF_with_AIG(gate_id, pAIG);
    change_connection(GateMap[input1], input1, pAIG, gate_id);
    change_connection(GateMap[input2], input2, pAIG, gate_id);
  }
  bool myStr2Unsigned(const string &str, unsigned &num)
  {
    num = 0;
    size_t i = 0;
    bool valid = false;
    for (; i < str.size(); ++i)
    {
      if (isdigit(str[i]))
      {
        num *= 10;
        num += unsigned(str[i] - '0');
        valid = true;
      }
      else
        return false;
    }
    return valid;
  }
  bool change_connection(CirGate *input, unsigned input_id, CirGate *output, unsigned output_id)
  {
    if (input == NULL && output == NULL)
    {
      return false;
    }
    if (input == NULL)
    {
      input = new UNDEF_Gate(input_id);
      GateMap[input_id] = input;
    }
    else if (output == NULL)
    {
      output = new UNDEF_Gate(output_id);
      GateMap[output_id] = output;
    }

    input->_fanoutList.push_back(output);
    output->_faninList.push_back(input);
    return true;
  }
  bool change_connect(CirGate *input, CirGate *output)
  {
    assert(input != 0 && output != 0);
    input->_fanoutList.push_back(output);
    output->_faninList.push_back(input);
    return true;
  }
  bool replace_UNDEF_with_AIG(unsigned Gate_Id, CirGate *pAIG)
  {
    for (auto it = GateMap[Gate_Id]->_faninList.begin(); it != GateMap[Gate_Id]->_faninList.end(); ++it)
    {
      (*it)->_fanoutList.pop_back();
      change_connect(*it, pAIG);
    }
    for (auto it = GateMap[Gate_Id]->_fanoutList.begin(); it != GateMap[Gate_Id]->_fanoutList.end(); ++it)
    {
      if ((*it)->_faninList[0] == (*it)->_faninList[1])
      {
        (*it)->_faninList.clear();
        change_connect(pAIG, *it);
        change_connect(pAIG, *it);
        ++it;
      }
      else
      {
        if (GateMap[Gate_Id] == (*it)->_faninList.back())
        {
          (*it)->_faninList.pop_back();
          change_connect(pAIG, *it);
        }
        else
        {
          (*it)->_faninList.erase((*it)->_faninList.begin());
          change_connect(pAIG, *it);
          swap((*it)->_faninList[0], (*it)->_faninList[1]);
        }
      }
    }
    delete GateMap[Gate_Id];
    GateMap[Gate_Id] = pAIG;
    return true;
  }
  void Execute_DFS()
  {
    CirGate::setGlobalRef();
    for (auto it = sink_list.begin(); it != sink_list.end(); ++it)
    {
      gate_DFS(*it, 0);
    }
    counter_netlist = 0;
  }
  void Execute_DFS_for_writeout(Array<unsigned> &storage) const
  {
    CirGate::setGlobalRef();
    for (auto it = sink_list.begin(); it != sink_list.end(); ++it)
    {
      gate_DFS_for_writeout(*it, storage);
    }
    for(auto it = GateMap.begin();it!=GateMap.end();++it)
    {
      if(it->second->type_name == UNDEF_GATE)
      {
        it->second->setToglobalref();
      }
    }
    counter_netlist = 0;
  }
  void gate_DFS(CirGate *input_gate, size_t x)
  {
    if (!input_gate->isGlobalref())
    {
      size_t i = 0;
      for (; i < input_gate->_faninList.size(); ++i, ++x)
      {
        gate_DFS(input_gate->_faninList[i], i);
      }
      // input_gate->setToglobalref();
      if (input_gate->type_name == UNDEF_GATE)
      {
        if (x == 0)
          is_UNDEF_0 = true;
        else //if(x==1)
          is_UNDEF_1 = true;
      }
      else
      {
        input_gate->setToglobalref();
      }
      printGateInformation(input_gate);
    }
  }
  void gate_DFS_for_writeout(CirGate *input_gate, Array<unsigned> &storage) const
  {
    if (!input_gate->isGlobalref())
    {
      size_t i = 0;
      for (; i < input_gate->_faninList.size(); ++i)
      {
        gate_DFS_for_writeout(input_gate->_faninList[i], storage);
      }
      input_gate->setToglobalref();
      printAIGGate(input_gate, storage);
    }
  }
  template <class T>
  void swap(T &a, T &b)
  {
    T temp = a;
    a = b;
    b = temp;
  }

private:
};

#endif // CIR_MGR_H
