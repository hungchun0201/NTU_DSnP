/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <regex>
#include <map>
#include <unordered_map>
#include <stdio.h>
#include <random>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void CirMgr::randomSim()
{
  random_device rd;
  mt19937_64 gen(rd());
  size_t times;

  if (Circuit.maxid > 50000)
  {
    uniform_int_distribution<size_t> unif(0.95 * Circuit.maxid / 64, 1.15 * Circuit.maxid / 64);
    times = unif(gen);
  }
  else if (10000 < Circuit.maxid && Circuit.maxid <= 50000)
  {
    uniform_int_distribution<size_t> unif(1.2 * Circuit.maxid / 64, 1.5 * Circuit.maxid / 64);
    times = unif(gen);
  }
  else if (5000 < Circuit.maxid && Circuit.maxid <= 10000)
  {
    uniform_int_distribution<size_t> unif(1.85 * Circuit.maxid / 64, 2.15 * Circuit.maxid / 64);
    times = unif(gen);
  }
  else if (1000 < Circuit.maxid && Circuit.maxid <= 5000)
  {
    uniform_int_distribution<size_t> unif(2.15 * Circuit.maxid / 64, 2.35 * Circuit.maxid / 64);
    times = unif(gen);
  }
  else if(Circuit.maxid<30)
  {
    uniform_int_distribution<size_t> unif(20 * Circuit.maxid / 64, 64* Circuit.maxid / 64);
    times = unif(gen);
  }
  else
  {
    uniform_int_distribution<size_t> unif(10 * Circuit.maxid / 64, 20 * Circuit.maxid / 64);
    times = unif(gen);
  }

  for (size_t i = 0; i < times; ++i)
  {
    for (size_t ind = 0; ind < Circuit.inputs; ++ind)
    {
      Circuit.id2Gate[Circuit.PI_list[ind]]->sim_pattern = gen();
    }
    ConstructSimCircuit_and_FECgroups();
  }
  cout << times * 64 << " patterns simulated." << endl;
  sort(FECFinal_vec.begin(), FECFinal_vec.end(), CirMgr::compare_IdList);
  Circuit.FECgroups.clear();
  for (size_t ind = 0; ind < FECFinal_vec.size(); ++ind)
  {
    auto it = FECFinal_vec[ind]->begin();
    if (*it % 2 == 1)
    {
      for (; it != FECFinal_vec[ind]->end(); ++it)
      {
        *it = inverse_func(*it);
      }
    }
    for (auto it = FECFinal_vec[ind]->begin(); it != FECFinal_vec[ind]->end(); ++it)
    {

      if (*it % 2 == 0)
      {
        break;
      }
      if (it == --FECFinal_vec[ind]->end())
      {
        for (auto it_in = FECFinal_vec[ind]->begin(); it_in != FECFinal_vec[ind]->end(); ++it_in)
        {
          *it -= 1;
        }
      }
    }
    it = FECFinal_vec[ind]->begin();
    size_t key = Circuit.id2Gate[*it / 2]->getSim_pattern();
    Circuit.FECgroups.insert({key, FECFinal_vec[ind]});
  }
  if (_simLog != 0)
  {
    writeToLog(*_simLog);
  }

  return;
}

void CirMgr::fileSim(ifstream &patternFile)
{
  string line;
  unsigned lineNo = 0;
  Circuit.has_sim = false;
  // sort(Circuit.PI_list, Circuit.PI_list + Circuit.inputs);
  line.reserve(Circuit.inputs);
  while (patternFile >> line)
  {

    if (line.size() != Circuit.inputs)
    {
      cout << "Error: Pattern(" << line << ") length("
           << line.size() << ") does not match the number of inputs("
           << Circuit.inputs << ") in a circuit!!" << endl;
      return;
    }
    for (size_t i = 0; i < line.size(); ++i)
    {
      if (line[i] != '0' && line[i] != '1')
      {
        cout << "Error: Pattern(" << line << ") contains a non-0/1 character(‘"
             << line[i] << "’)." << endl;
        return;
      }
      else
      {
        if (lineNo % 64 == 0)
          Circuit.id2Gate[Circuit.PI_list[i]]->sim_pattern = 0;
        if (line[i] == '1')
          Circuit.id2Gate[Circuit.PI_list[i]]->sim_pattern += pow(2, lineNo % 64);
        // if (i == 0)
        //   cout << Circuit.PI_list[i] << "   " << Circuit.id2Gate[Circuit.PI_list[i]]->sim_pattern << endl;
      }
    }
    if (lineNo % 64 == 63)
    {
      ConstructSimCircuit_and_FECgroups();
    }
    ++lineNo;
  }
  if (lineNo % 64 != 0)
  {
    ConstructSimCircuit_and_FECgroups();
  }
  cout << lineNo << " patterns simulated." << endl;
  sort(FECFinal_vec.begin(), FECFinal_vec.end(), CirMgr::compare_IdList);
  Circuit.FECgroups.clear();
  for (size_t ind = 0; ind < FECFinal_vec.size(); ++ind)
  {
    auto it = FECFinal_vec[ind]->begin();
    if (*it % 2 == 1)
    {
      for (; it != FECFinal_vec[ind]->end(); ++it)
      {
        *it = inverse_func(*it);
      }
    }
    for (auto it = FECFinal_vec[ind]->begin(); it != FECFinal_vec[ind]->end(); ++it)
    {

      if (*it % 2 == 0)
      {
        break;
      }
      if (it == --FECFinal_vec[ind]->end())
      {
        for (auto it_in = FECFinal_vec[ind]->begin(); it_in != FECFinal_vec[ind]->end(); ++it_in)
        {
          *it -= 1;
        }
      }
    }
    it = FECFinal_vec[ind]->begin();
    size_t key = Circuit.id2Gate[*it / 2]->getSim_pattern();
    Circuit.FECgroups.insert({key, FECFinal_vec[ind]});
  }
  if (_simLog != 0)
  {
    writeToLog(*_simLog);
  }

  return;
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
void CirMgr::patternTrans(vector<string> &pat, int times = 64)
{
  for (int i = times - 1; i >= 0; i--)
  {
    for (unsigned j = 0; j < Circuit.inputs; ++j)
    {
      Circuit.id2Gate[Circuit.PI_list[i]]->sim_pattern = (Circuit.id2Gate[Circuit.PI_list[i]]->sim_pattern << 1) | (size_t)(pat[i][j] - '0');
    }
  }
}
void CirMgr::ConstructSimCircuit_and_FECgroups()
{
  Circuit.FECgroups.clear();
  Circuit.FECgroups.insert({0, new IdList(1, 0)});
  static unsigned printid = 0;
  printid = 0;
  CirGate::setGlobalref();
  for (size_t i = Circuit.maxid + 1; i < Circuit.maxid + Circuit.outputs + 1;
       i++)
  {
    DFSTravPO(i, printid, SIMULATION);
  }

  if (Circuit.has_sim == false)
  {
    for (auto it = Circuit.FECgroups.begin(); it != Circuit.FECgroups.end(); ++it)
    {
      if (it->second->size() >= 2)
      {
        sort(it->second->begin(), it->second->end());
        FECFinal_vec.push_back(it->second);
      }
    }
    sort(FECFinal_vec.begin(), FECFinal_vec.end(), CirMgr::compare_IdList);
    Circuit.has_sim = true;
  }
  else
  {
    vector<IdList *> newFECFinal_vec;
    size_t size = FECFinal_vec.size();
    for (size_t ind = 0; ind < size; ++ind)
    {
      if (FECFinal_vec[ind] == NULL)
        continue;
      unordered_map<size_t, IdList *> Count_map;
      for (auto it = FECFinal_vec[ind]->begin(); it != FECFinal_vec[ind]->end(); ++it)
      {
        size_t &key = Circuit.id2Gate[*it / 2]->sim_pattern;
        auto it_find = Count_map.find(key);
        if (it_find != Count_map.end())
        {
          it_find->second->push_back(*it);
        }
        else
        {
          it_find = Count_map.find(~key);
          if (it_find != Count_map.end())
          {
            it_find->second->push_back(*it);
          }
          else
          {
            newFECFinal_vec.push_back(new IdList(1, *it));
            Count_map[key] = newFECFinal_vec.back();
          }
        }
      } //construct a hashmap in the fecgroup
      delete FECFinal_vec[ind];

      for (auto it = Count_map.begin(); it != Count_map.end(); ++it)
      {
        if (it->second->size() <= 1)
        {
          newFECFinal_vec.erase(remove(newFECFinal_vec.begin(), newFECFinal_vec.end(), it->second), newFECFinal_vec.end());
          delete it->second;
        }
      }
      //if the size of FECgroup == 1,don't push_back
    } //do the above step for every fecgroup
    // for (auto it2 = newFECFinal_vec.begin(); it2 != newFECFinal_vec.end(); ++it2)
    // {
    //   if ((*it2)->size() > 1)
    //   {
    //     FECFinal_vec.push_back((*it2));
    //   }
    // }
    FECFinal_vec.clear();
    FECFinal_vec = move(newFECFinal_vec);
  }
}
inline size_t CirMgr::pow(unsigned base, int times)
{
  size_t temp = 1;
  for (int i = 0; i < times; ++i)
  {
    temp *= base;
  }
  return temp;
}
void CirMgr::writeToLog(ostream &simLog)
{

  for (size_t ind = 0; ind < FECFinal_vec.size(); ++ind)
  {
    if (FECFinal_vec[ind] == 0)
      continue;
    simLog << "[" << ind << "]";
    for (auto it = FECFinal_vec[ind]->begin(); it != FECFinal_vec[ind]->end(); ++it)
    {
      simLog << " ";
      if (*it % 2 == 1)
        simLog << "!";
      simLog << *it / 2;
    }
    simLog << endl;
  }
}
void CirMgr::Consult_FECGroup(unsigned id)
{
  if (Circuit.id2Gate[id]->getType() != AIG_GATE && Circuit.id2Gate[id]->getType() != CONST_GATE)
  {
    return;
  }
  size_t key = Circuit.id2Gate[id]->getSim_pattern();
  auto it = Circuit.FECgroups.find(key);
  if (it != Circuit.FECgroups.end())
  {
    auto inner = find(it->second->begin(), it->second->end(), 2 * id);
    if (inner != it->second->end())
    {
      for (auto print = it->second->begin(); print != it->second->end(); ++print)
      {
        if (*print != id * 2)
        {
          cout << " ";
          if (*print % 2 == 1)
            cout << "!";
          cout << *print / 2;
        }
      }
    }
  }
  it = Circuit.FECgroups.find(~key);
  if (it != Circuit.FECgroups.end())
  {

    {
      for (auto print = it->second->begin(); print != it->second->end(); ++print)
      {
        if (*print != id * 2 + 1)
        {
          cout << " ";
          if (*print % 2 == 0)
            cout << "!";
          cout << *print / 2;
        }
      }
    }
  }
}