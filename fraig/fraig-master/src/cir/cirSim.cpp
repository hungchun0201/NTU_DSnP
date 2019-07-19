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
#include <climits>
#include <unordered_map>
#include <utility>
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
  if (_isSim == 0)
    initialSim();
  int steadyCount = 0, simCount = 0;
  while (steadyCount < 20)
  {
    bitset<64> pipattern[header[1]];
    generateIn(pipattern);
    steadyCount += simPattern();
    simCount++;
    if (_simLog == 0)
      continue;
    bitset<64> popattern[header[3]];
    for (int i = 0; i < header[3]; i++)
    {
      popattern[i] = bitset<64>(_gates[header[0] + i + 1]->getValue());
    }
    for (int i = 0; i < 64; i++)
    {
      for (int j = 0; j < header[1]; j++)
      {
        (*_simLog) << pipattern[j][i];
      }
      (*_simLog) << " ";
      for (int j = 0; j < header[3]; j++)
      {
        (*_simLog) << popattern[j][i];
      }
      (*_simLog) << endl;
    }
  }
  insertGroupNo();
  cout << simCount * 64 << " patterns simulated.\n";
  _isSim = 1;
}

void CirMgr::fileSim(ifstream &patternFile)
{
  if (_isSim == 0)
    initialSim();
  vector<string> input;
  input.resize(64);

  string buf;
  unsigned patternCount = 0;
  while (patternFile >> buf)
  {
    if (!checkPattern(buf) && patternCount % 64 != 0)
      simBatch(input, patternCount % 64);
    input[patternCount % 64] = buf;
    patternCount++;
    if (patternCount % 64 == 0)
      simBatch(input, 64);
  }
  insertGroupNo();
  cout << patternCount << " patterns simulated.\n";
  _isSim = 1;
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
int CirMgr::simPattern()
{
  int notChanged = 1;
  for (IdList::iterator it = _DFS.begin(); it != _DFS.end(); ++it)
  {
    _gates[*it]->sim();
    //cout<<*it<<" "<<_gates[*it]->getValue()<<" "<<_gates[*it]->getivValue()<<endl;
  }
  IdList newHeads;
  vector<IdList *> newGroups;
  for (size_t i = 0; i < _FEC.size(); i++)
  { //for each existing group;
    if (_FEC[i] == 0)
      continue;
    unordered_map<size_t, IdList *> resultMap;
    IdList *remains = new IdList(); //
    resultMap[_gates[i]->getValue()] = _FEC[i];
    for (IdList::iterator gateIt = _FEC[i]->begin(); gateIt != _FEC[i]->end(); ++gateIt)
    {//第二層的iterator是gateIt,在_FEC[i]的FECGroup中,取[]就是獲得unsigned
      if (*gateIt == 0)
        continue;
      unordered_map<size_t, IdList *>::iterator groupNum = resultMap.find(_gates[*gateIt / 2]->getValue());
      if (groupNum != resultMap.end())
      { //non-invert,have found
        if ((*groupNum).second == _FEC[i])
        { //剛剛好第一個位置的話 
        //groupNum是個hashmap的iterator
          remains->push_back(((*gateIt) / 2) * 2);
        }
        else
        {
          (*groupNum).second->push_back(((*gateIt) / 2) * 2);
        }
      }
      else//檢查invert的case
      {
        groupNum = resultMap.find((_gates[(*gateIt) / 2]->getivValue()));
        if (groupNum != resultMap.end())
        { //invert
          if ((*groupNum).second == _FEC[i])
          {
            remains->push_back(((*gateIt) / 2) * 2 + 1);
          }
          else
          {
            (*groupNum).second->push_back(((*gateIt) / 2) * 2 + 1);
          }
        }
        else
        { //new Group
          newHeads.push_back((*gateIt) / 2);//就是個存unsigned的IdList
          newGroups.push_back(new IdList());//存IdList*的
          resultMap[_gates[(*gateIt) / 2]->getValue()] = newGroups.back();
          //給result map一個新pattern當key，存新的IdList
          notChanged = 0;
        }
      }
    }
    //
    delete _FEC[i];
    if (remains->empty())
    {
      delete remains;
      _FEC[i] = 0;//NULL直接丟在FECGroup裡面 跑得時候移遇到null就contunue;
    }
    else
      _FEC[i] = remains;//remains是剛剛那些gropeNum
  }
  for (size_t i = 0; i < newHeads.size(); i++)
  {
    _FEC[newHeads[i]] = newGroups[i];
  }
  return notChanged;
}

void CirMgr::initialSim()
{
  IdList *list = new IdList();
  for (unsigned i = 0; i < header[0] + header[3] + 1; i++)
  {
    if ((_gates[i] == 0))
      continue;
    if (_gates[i]->isAig() && _gates[i]->getOrder() != ~0)
    {
      list->push_back(2 * i);
    }
  }
  _FEC[0] = list;
}

void
    CirMgr::generateIn(bitset<64> *pipattern)
{
  for (unsigned i = 0; i < header[1]; i++)
  {
    unsigned a[2] = {rnGen(INT_MAX), rnGen(INT_MAX)};
    size_t *ptr = (size_t *)a;
    pipattern[i] = bitset<64>(*ptr);
    _gates[_pis[i]]->setValue(*ptr);
  }
}

bool CirMgr::checkPattern(const string &buf)
{
  if (buf.size() != header[1])
  {
    cout << "Error: Pattern(" << buf << ") length(" << buf.size() << ")";
    cout << " does not match the number of inputs(";
    cout << header[1] << ") in a circuit!!\n";
    return 0;
  }
  for (unsigned i = 0; i < buf.size(); i++)
  {
    if (buf[i] != '0' && buf[i] != '1')
    {
      cout << "Error: Pattern(" << buf << ") contains a non-0/1 character('" << buf[i] << "').\n";
      return 0;
    }
  }
  return 1;
}

void CirMgr::insertGroupNo()
{
  for (size_t i = 0; i < _gates.size(); i++)
  {
    if (_gates[i] == 0)
      continue;
    _gates[i]->setGroupNo(i * 2);
  }
  for (size_t group = 0; group < _FEC.size(); group++)
  {
    if (_FEC[group] == 0)
      continue;
    for (size_t i = 0; i < _FEC[group]->size(); i++)
    {
      if ((*_FEC[group])[i] == 0)
        continue;
      _gates[(*_FEC[group])[i] / 2]->setGroupNo(2 * group + (*_FEC[group])[i] % 2);
    }
  }
}

void CirMgr::simBatch(vector<string> &input, unsigned patternCount)
{
  bitset<64> pipattern[header[1]];
  for (unsigned i = 0; i < 64; i++)
  { //translation
    for (unsigned j = 0; j < header[1]; j++)
    {
      pipattern[j][i] = (input[i][j] - '0');
    }
    if (i == patternCount - 1)
    { //filling
      for (unsigned j = 0; j < header[1]; j++)
      {
        for (unsigned i2 = patternCount; i2 < 64; i2++)
        {
          pipattern[j][i2] = 0;
        }
      }
      for (unsigned j = 0; j < header[1]; j++)
      {
        _gates[_pis[j]]->setValue(pipattern[j].to_ullong());
      }
      simPattern();
      if (_simLog == 0)
        continue;
      bitset<64> popattern[header[3]];
      for (unsigned j = 0; j < header[3]; j++)
      {
        popattern[j] = bitset<64>(_gates[header[0] + j + 1]->getValue());
      }
      for (unsigned line = 0; line <= i; line++)
      {
        (*_simLog) << input[line] << " ";
        for (unsigned j = 0; j < header[3]; j++)
        {
          (*_simLog) << (popattern[j][line % 64]);
        }
        (*_simLog) << endl;
      }
    }
  }
}
