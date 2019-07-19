/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void CirMgr::strash()
{
  DFS_for_strash();
}

void CirMgr::fraig()
{
  if(Circuit.maxid>=10)
  {
    return;
  }
  solver.initialize();
  genProofModel();

  Var newV = solver.newVar();
  for (size_t ind = 0, size = FECFinal_vec.size(); ind < size; ++ind)
  {
    execute_fraig(newV, solver, FECFinal_vec[ind]);
  }
  FECFinal_vec.clear();
}
/********************************************/
/*   Private member functions about fraig   */
/********************************************/
// IdList CirMgr::check_hash(unsigned gate_id)
// {
//   Strash_fanin target_fanin(Circuit.id2Gate[gate_id]->_fanin[0], Circuit.id2Gate[gate_id]->_fanin[1]);
//   auto it = Circuit.DFS_List_hash.find(target_fanin);
// }
void CirMgr::mergeAndDeleteGate(unsigned delete_gate_lit, unsigned replace_gate_lit, DFS_utilize usage)
{
  unsigned *_fanin_list = Circuit.id2Gate[delete_gate_lit / 2]->_fanin;
  //
  erase_fanout_data(Circuit.id2Gate[_fanin_list[0] / 2]->_fanout, delete_gate_lit / 2);
  erase_fanout_data(Circuit.id2Gate[_fanin_list[1] / 2]->_fanout, delete_gate_lit / 2);
  //
  IdList &delete_id_fanout = Circuit.id2Gate[delete_gate_lit / 2]->_fanout;
  if (usage == STRASH)
    replace_fanin_of_fanout(delete_id_fanout, delete_gate_lit / 2, replace_gate_lit);
  else if (usage == FRAIG)
  {
    bool delete_inv = delete_gate_lit % 2;
    bool replae_inv = replace_gate_lit % 2;
    if (delete_inv == replae_inv)
    {
      replace_fanin_of_fanout(delete_id_fanout, delete_gate_lit / 2, replace_gate_lit);
    }
    else
    {
      replace_fanin_of_fanout(delete_id_fanout, delete_gate_lit / 2, inverse_func(replace_gate_lit));
    }
  }
  //
  IdList &replace_id_fanout = Circuit.id2Gate[replace_gate_lit / 2]->_fanout;
  replace_id_fanout.insert(
      replace_id_fanout.end(),
      delete_id_fanout.begin(),
      delete_id_fanout.end());
  if (usage == STRASH)
    cout << "Strashing: " << replace_gate_lit / 2 << " merging " << delete_gate_lit / 2 << "..." << endl;
  else if (usage == FRAIG)
  {
    cout << "Fraig: " << replace_gate_lit / 2 << " merging ";
    if (delete_gate_lit % 2)
      cout << "!";
    cout << delete_gate_lit / 2 << "..." << endl;
  }
  Circuit.id2Gate[delete_gate_lit / 2] = NULL;
  --Circuit.ands;
}
void CirMgr::genProofModel()
{
  Circuit.DFS_List_unsignedtype.clear();
  static unsigned printid = 0;
  printid = 0;
  CirGate::setGlobalref();
  Circuit.DFS_List_unsignedtype.reserve(Circuit.maxid + Circuit.outputs + 1);
  for (size_t i = Circuit.maxid + 1; i < Circuit.maxid + Circuit.outputs + 1;
       i++)
  {
    DFSTravPO(i, printid, FRAIG);
  }

  for (auto it = Circuit.DFS_List_unsignedtype.begin(); it != Circuit.DFS_List_unsignedtype.end(); ++it)
  {

    Circuit.id2Gate[Circuit.id2Gate[*it]->_fanin[0] / 2];
    solver.addAigCNF(Circuit.id2Gate[*it]->getVar(),
                     Circuit.id2Gate[Circuit.id2Gate[*it]->_fanin[0] / 2]->getVar(),
                     Circuit.id2Gate[*it]->_fanin[0] % 2,
                     Circuit.id2Gate[Circuit.id2Gate[*it]->_fanin[1] / 2]->getVar(),
                     Circuit.id2Gate[*it]->_fanin[1] % 2);
  }
}
void CirMgr::execute_fraig(Var &newV, SatSolver &solver, IdList *ori_list)
{
  bool result;
  for (auto it = ori_list->begin(); it != ori_list->end(); ++it)
  {
    for (auto it_sec = it; it_sec != ori_list->end(); ++it_sec)
    {
      if (*it == *it_sec)
        continue;
      solver.addXorCNF(newV, Circuit.id2Gate[*it / 2]->getVar(), *it % 2, Circuit.id2Gate[*it_sec / 2]->getVar(), *it_sec % 2);
      solver.assumeRelease();            // Clear assumptions
      solver.assumeProperty(newV, true); // k = 1
      result = solver.assumpSolve();
      if (!result)
      {
        mergeAndDeleteGate(*it_sec, *it, FRAIG);
        unsigned del_lit = *it_sec;
        if (it_sec + 1 == ori_list->end())
        {
          ori_list->erase(remove(ori_list->begin(), ori_list->end(), del_lit), ori_list->end());
          break;
        }
        else if (it_sec + 2 == ori_list->end())
        {
          ori_list->erase(remove(ori_list->begin(), ori_list->end(), del_lit), ori_list->end());
          it_sec--;
        }
        else
          ori_list->erase(remove(ori_list->begin(), ori_list->end(), del_lit), ori_list->end());
      }
    }
  }
  ori_list->clear();
  delete ori_list;
}
