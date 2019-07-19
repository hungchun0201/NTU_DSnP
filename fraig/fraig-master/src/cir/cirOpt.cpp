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
void
CirMgr::sweep()
{
  bool used[header[0]+header[3]+1]={0};
  for(IdList::iterator it=_DFS.begin();it!=_DFS.end();it++){
     used[*it]=1;
    }
  for(size_t i=0;i<header[0]+header[3]+1;i++){
    if(used[i]==0){
       if(_gates[i]!=0 && _gates[i]->isAig()){ 
         delete _gates[i];
         _gates[i]=0;
         _aigCount--;
         cout<<"Sweeping: AIG("<<i<<") removed...\n";
       }
    }
  }
  for(size_t i=0;i<header[0]+header[3]+1;i++){
     if(_gates[i]==NULL)continue;
     _gates[i]->sweepOut(used);
  }
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
   for(IdList::iterator it=_DFS.begin();it!=_DFS.end();++it){
      if(!_gates[*it]->isAig())continue;
      if(_gates[*it]->optimize()){
         delete _gates[*it];
           _gates[*it]=0;
           _aigCount--;
      }
    }
    for(GateList::iterator it=_gates.begin();it!=_gates.end();it++){
      if(*it==0)continue;
      if((*it)->isUnusedUndef()){
        delete *it;
        *it=0;
      }
    }
   searchGates(_DFS);
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/