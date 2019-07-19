/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void CirMgr::sweep()
{
  DFS_for_sweep();
  for (size_t i = Circuit.inputs + 1; i < Circuit.maxid + 1; ++i)
  {
    if (Circuit.id2Gate[i] != 0 && !Circuit.DFS_List_booltype.at(i))
    {
      Delete_gate(i);
    }
  }
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void CirMgr::optimize()
{
  DFS_for_optimize();
  for (auto it = Circuit.DFS_List_unsignedtype.begin();
       it != Circuit.DFS_List_unsignedtype.end(); ++it)
  {
    if (Circuit.id2Gate[*it]->getType() == AIG_GATE)
    {
      unsigned *fanin_list = Circuit.id2Gate[*it]->_fanin;
      IdList fanout_list = Circuit.id2Gate[*it]->_fanout;
      /********************
       * one of intput is 0
       * **************** */
      if (fanin_list[0] == 0 || fanin_list[1] == 0)
      {
        replace_fanin_of_fanout(fanout_list, *it, 0);
        //changed the gate's fanout's fanin
        append_fanout(0, *it);
        if (fanin_list[0] == 0)
        {
          erase_fanout_data(Circuit.id2Gate[fanin_list[1] / 2]->_fanout, *it);
        }
        else if (fanin_list[1] == 0)
        {
          erase_fanout_data(Circuit.id2Gate[fanin_list[0] / 2]->_fanout, *it);
        }
        printOptimizeInformation(0, *it);
        --Circuit.ands;
        Circuit.id2Gate[*it] = NULL;
      }
      /********************
       * two of intputs are the same
       * **************** */
      else if (fanin_list[0] == fanin_list[1])
      {
        replace_fanin_of_fanout(fanout_list, *it, fanin_list[0]);
        append_fanout(fanin_list[0], *it);
        printOptimizeInformation(fanin_list[0], *it);
        --Circuit.ands;
        Circuit.id2Gate[*it] = NULL;
      }
      /********************
       * one of intput is 1
       * **************** */
      else if (fanin_list[0] == 1 || fanin_list[1] == 1)
      {
        if (fanin_list[0] == 1)
        {
          replace_fanin_of_fanout(fanout_list, *it, fanin_list[1]);
          erase_fanout_data(Circuit.id2Gate[fanin_list[0] / 2]->_fanout, *it);
          append_fanout(fanin_list[1], *it);
          printOptimizeInformation(fanin_list[1], *it);
        }
        else if (fanin_list[1] == 1)
        {
          replace_fanin_of_fanout(fanout_list, *it, fanin_list[0]);
          append_fanout(fanin_list[0], *it);
          erase_fanout_data(Circuit.id2Gate[fanin_list[1] / 2]->_fanout, *it);
          printOptimizeInformation(fanin_list[0], *it);
        }
        --Circuit.ands;
        Circuit.id2Gate[*it] = NULL;
      }
      /****************************
       * two of intput are inverted
       * *************************** */
      else if (fanin_list[0] / 2 == fanin_list[1] / 2 && fanin_list[0] != fanin_list[1])
      {
        replace_fanin_of_fanout(fanout_list, *it, 0);
        append_fanout(0, *it);
        erase_fanout_data(Circuit.id2Gate[fanin_list[0] / 2]->_fanout, *it);
        printOptimizeInformation(0, *it);
        --Circuit.ands;
        Circuit.id2Gate[*it] = NULL;
      }
    }
  }
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
bool CirMgr::replace_fanin_of_fanout(IdList &fanout, unsigned init_id, unsigned after_lit)
{
  for (auto it2 = fanout.begin(); it2 != fanout.end(); ++it2)
  {
    //it2 : NOTE!!! _fanout_list always store lit!!
    //Therefore,*it2 is lit not ID
    //Why we store lit instead of ID because of cirg -fanout
    //traversal different gate's _fanin in _fanout_list
    unsigned *fanin = Circuit.id2Gate[*it2 / 2]->_fanin;
    //fanin[0] also stored the lit resulting from cirg -fanin
    for (int i = 0; i < 2; ++i)
    {
      if (fanin[0] == init_id * 2 + i)
      {
        if (i == 0) //not_gate is not connected
        {
          fanin[0] = after_lit;
        }
        else //inverted
          after_lit % 2 == 0 ? fanin[0] = after_lit + 1 : fanin[0] = after_lit - 1;
      }
      else if (fanin[1] == init_id * 2 + i)
      {
        if (i == 0)
        {
          fanin[1] = after_lit;
        }
        else
          after_lit % 2 == 0 ? fanin[1] = after_lit + 1 : fanin[1] = after_lit - 1;
      }
      else
      {
      }
    }
  }
  return true;
}

bool CirMgr::erase_fanout_data(vector<unsigned> &vec, unsigned _data)
{
  vector<unsigned>::iterator it;
  it = std::remove(vec.begin(), vec.end(), _data * 2);
  it = std::remove(vec.begin(), it, _data * 2 + 1);
  // for(auto it = vec.begin();it!=vec.end())
  if (!(it == vec.end()))
  {
    vec.erase(it, vec.end());
    return true;
  }
  else
  {
    return false;
  }
}
inline void CirMgr::printOptimizeInformation(unsigned result_lit, unsigned gate_id)
{
  cout << "Simplifying: " << result_lit / 2 << " merging ";
  if (result_lit % 2 == 1)
  {
    cout << "!";
  }
  cout << gate_id << "..." << endl;
}
bool CirMgr::append_fanout(unsigned target_lit, unsigned delete_id)
{
  //in some cases,the _fanin of origin gate may be inverted
  //if so,we have to change the _fanout lit
  IdList &target_fanout = Circuit.id2Gate[target_lit / 2]->_fanout;
  IdList &delete_fanout = Circuit.id2Gate[delete_id]->_fanout;
  erase_fanout_data(target_fanout, delete_id);
  if (target_lit % 2)
  {
    for (auto it = delete_fanout.begin(); it != delete_fanout.end(); ++it)
    {
      *it = inverse_func(*it);
    }
  }
  target_fanout.insert(
      target_fanout.end(),
      delete_fanout.begin(),
      delete_fanout.end());

  return true;
}

inline void CirMgr::inverse_func_ref(unsigned &init_lit)
{
  init_lit % 2 == 0 ? ++init_lit : --init_lit;
}