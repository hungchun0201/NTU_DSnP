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
#include <unordered_map>
#include <utility>


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
void
CirMgr::strash()
{
  unordered_map<size_t,unsigned> faninmap;
  for(IdList::iterator it=_DFS.begin();it!=_DFS.end();it++){
    if(_gates[*it]==NULL || _gates[*it]->isAig()==false)continue;
    if(faninmap.find(_gates[*it]->getInPair())==faninmap.end()){
      faninmap[_gates[*it]->getInPair()]=_gates[*it]->getGateNo();
    }
    else {
      unsigned gate=(*(faninmap.find(_gates[*it]->getInPair()))).second ;
      _gates[*it]->replace(2*gate);
      cout<<"Strashing: "<<gate<<" merging "<<_gates[*it]->getGateNo()<<"...\n";
      delete _gates[*it];
      _gates[*it]=0;
      _aigCount--;
    }
  }
  searchGates(_DFS);
}

void
CirMgr::fraig()
{
   //version 2
   ///* 
   IdList baseList;
   setBase(baseList);
    for(unsigned i=0;i<_DFS.size();){
        if(!_gates[_DFS[i]]->isAig() ){i++;continue;}//checkAig
        unsigned groupLit=_gates[_DFS[i]]->getGroupNo();//Grouplit
        unsigned target=_DFS[i];//gateNumber
        unsigned baseLit=baseList[groupLit/2];
        //if(_DFS[i]==0)continue;
        if(groupLit/2==target && _FEC[target]==0){i++;continue;}//if no FEC
        //cout<<"Target: "<<target<<", Base: "<< baseList[groupNo/2]/2<<endl;
        if(baseLit/2==target){i++;continue;}// target is base
        //if head==base
        if(groupLit/2==baseLit/2){//if head==base
            if(fraigPair(groupLit,target*2)){//fraig head and target
               for(unsigned idx=0;idx<_FEC[groupLit/2]->size();idx++){
                 if((*_FEC[groupLit/2])[idx]/2==target){//delete target in FEC
                   (*_FEC[groupLit/2])[idx]=0;
                   break;
                  }
                }
               cleanFEC();
               setBase(baseList);
               i=0;
               continue;
            }
            else {
               insertGroupNo();
               setBase(baseList);
               i++;
            }
        }
        //if head!=base
        else {
           if(target==groupLit/2){//target==head
              if(fraigPair(baseLit,target*2)){//fraig head and base 
                for(unsigned idx=0;idx<_FEC[groupLit/2]->size();idx++){//find next head;
                  if((*_FEC[groupLit/2])[idx]!=0){
                     _FEC[(*_FEC[groupLit/2])[idx]/2]=_FEC[groupLit/2];//set new group
                     for(unsigned idx2=idx;idx2<_FEC[groupLit/2]->size();idx2++){// set group
                       if((*_FEC[groupLit/2])[idx2]==0)continue;
                       (*_FEC[groupLit/2])[idx2]=((*_FEC[groupLit/2])[idx2]/2)*2+(((*_FEC[groupLit/2])[idx2]%2)^((*_FEC[groupLit/2])[idx]%2));
                       _gates[(*_FEC[groupLit/2])[idx2]/2]->setGroupNo(2*((*_FEC[groupLit/2])[idx]/2)+((*_FEC[groupLit/2])[idx2]%2)^((*_FEC[groupLit/2])[idx]%2));
                     }
                     (*_FEC[groupLit/2])[idx]=0;
                     _FEC[groupLit/2]=0;
                     break;
                  }
                  if(idx==_FEC[groupLit/2]->size()-1){
                    delete _FEC[groupLit/2];
                    _FEC[groupLit/2]=0;
                  }
                }
                cleanFEC();
                setBase(baseList);
                i=0;
                continue;
              }
              else {
                 insertGroupNo();
                 setBase(baseList);
                 i++;
              }
           }
           else {//target !=head
              if(fraigPair(baseLit,target*2+groupLit%2)){//fraig base and target
               for(unsigned idx=0;idx<_FEC[groupLit/2]->size();idx++){
                 if((*_FEC[groupLit/2])[idx]/2==target){//delete targer from fec
                   (*_FEC[groupLit/2])[idx]=0;
                   break;
                  }
                 if(idx==_FEC[groupLit/2]->size()-1){
                    delete _FEC[groupLit/2];
                    _FEC[groupLit/2]=0;
                 }
               }
               cleanFEC();
               setBase(baseList);
               i=0;
               continue;
             }
             else {
               insertGroupNo();
               setBase(baseList);
               i++;
             }
           }
        }
      }
      for(FECGroups::iterator it=_FEC.begin();it!=_FEC.end();it++){//remove fecs
        if(*it!=0){
          delete *it;
          *it=0;
        }
      }
      insertGroupNo();
      //_isSim=0;
  //*/
   //my version 1;
   /*for(unsigned i=0;i<_FEC.size();i++){
      while( _FEC[i]!=0 && (*_FEC[i]).size()>=2){
         if(_gates[(*_FEC[i])[0]/2]->getOrder()<_gates[(*_FEC[i])[1]/2]->getOrder()){
           if(fraigPair((*_FEC[i])[0],(*_FEC[i])[1])){_FEC[i]->erase(++(_FEC[i]->begin()));}
          }
         else {
           if(fraigPair((*_FEC[i])[1],(*_FEC[i])[0])){_FEC[i]->erase(_FEC[i]->begin());}
         }
        }
      
      if(_FEC[i]!=0 && !_FEC[i]->empty()){
         if(i==0 || _gates[i]->getOrder()<_gates[(*_FEC[i])[0]/2]->getOrder()){
           if(fraigPair(i*2,(*_FEC[i])[0])){delete _FEC[i];_FEC[i]=0;}
          }
         else {
           if(fraigPair((*_FEC[i])[0],2*i)){delete _FEC[i];_FEC[i]=0;}
         }
      }
    }
    insertGroupNo();
    _isSim=0;
   //searchGates(_DFS);
   */
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
bool 
CirMgr::fraigPair(unsigned gateLit1,unsigned gateLit2){
   assert(gateLit1/2!=gateLit2/2);
   //cout<<"FraigPair: "<<gateLit1/2<<" "<<gateLit2/2<<endl;
   SatSolver sat;
   sat.initialize();
   Var cand=sat.newVar();
   IdList undefList;
   _gates[0]->setGateCNF(sat,undefList);
   //for(IdList::iterator it=_pis.begin();it!=_pis.end();it++){
         //_gates[*it]->setGateCNF(sat);
         //cout<<"SETTING: "<<*it<< " to "<<_gates[*it]->getValue()<<endl;
    //}
   _gates[gateLit1/2]->setGateCNF(sat,undefList);
   _gates[gateLit2/2]->setGateCNF(sat,undefList);
   sat.addXorCNF(cand,_gates[gateLit1/2]->getVar(),gateLit1%2,_gates[gateLit2/2]->getVar(),gateLit2%2);
   
   sat.assumeRelease();
   sat.assumeProperty(_gates[0]->getVar(),false);
   sat.assumeProperty(cand, true);
   bool result = sat.assumpSolve();
   if(result){
      for(IdList::iterator it=_pis.begin();it!=_pis.end();it++){
         if(_gates[*it]->getFlag())_gates[*it]->setValue(size_t(sat.getValue(_gates[*it]->getVar())));
         //cout<<"SETTING: "<<*it<< " to "<<_gates[*it]->getValue()<<endl;
        }
      for(IdList::iterator it=undefList.begin();it!=undefList.end();it++){
        _gates[*it]->setValue(size_t(sat.getValue(_gates[*it]->getVar())));
      }
      
      for(GateList::iterator it=_gates.begin();it!=_gates.end();it++){
         if(*it!=0)(*it)->setFlag(0);
        } 
      simPattern(); 
      for(IdList::iterator it=undefList.begin();it!=undefList.end();it++){
        _gates[*it]->setValue(0);
      }
      //if(gateLit1/2==1081 ||gateLit1/2==8333){
      //  cout<<"HAHAHAHAH\n";
     // }
      //cout<<"SAT: "<<gateLit1<<"  "<<gateLit2<<endl;
   }
   else{// Merge;
      for(GateList::iterator it=_gates.begin();it!=_gates.end();it++){
       if(*it!=0)(*it)->setFlag(0);
      }
      _gates[gateLit2/2]->replace(2*(gateLit1/2)+(gateLit1%2)^(gateLit2%2));
      delete _gates[gateLit2/2];
      _gates[gateLit2/2]=0;
      _aigCount--;
      searchGates(_DFS);
      cout<<"Fraig: ";
      //if(gateLit1%2)cout<<"!";
      cout<<gateLit1/2<<" merging ";
      if(gateLit2%2^gateLit1%2)cout<<"!";
      cout<<gateLit2/2<<"..."<<endl;
    }
  
  return !result;
}

void 
CirMgr::setBase(IdList& baseList){
   baseList.resize(header[0]+1,0);
   for(unsigned groupNo=1;groupNo<_FEC.size();groupNo++){
       if(_FEC[groupNo]==0)continue;
       if(_FEC[groupNo]->empty())continue;
       unsigned base=groupNo*2;
       for(unsigned i=0;i<_FEC[groupNo]->size();i++){
         if((*_FEC[groupNo])[i]==0)continue;
         if(_gates[(*_FEC[groupNo])[i]/2]->getOrder()<_gates[base/2]->getOrder())base=(*_FEC[groupNo])[i];
       }
       baseList[groupNo]=base;
   }
  }
void
CirMgr::cleanFEC(){
  for(unsigned i=0;i<_FEC.size();i++){
    if(_FEC[i]==0)continue;
    /*
    if(_gates[i]!=0 && _gates[i]->notUsed()){
      unsigned newHead=0;
      unsigned headpos;
      for(unsigned j=0;j<_FEC[i]->size();j++){
        if((*_FEC[i])[j]!=0){
           newHead=(*_FEC[i])[j];
           (*_FEC[i])[j]=0;
           headpos=j;
           break;
        }
        else { 
          if(j==_FEC[i]->size()-1){
            delete _FEC[i];
            _FEC[i]=0;
          }
        }
      }
      if(newHead!=0){
         for(unsigned idx=headpos+1;idx<_FEC[i]->size();idx++){
            if((*_FEC[i])[idx]==0){continue;}
            (*_FEC[i])[idx]=((*_FEC[i])[idx]/2)*2+(newHead%2)^((*_FEC[i])[idx]%2);
          }
         _FEC[newHead/2]=_FEC[i];
         _FEC[i]=0;
      }
      continue;
    }
    */
    for(unsigned j=0;j<_FEC[i]->size();j++){
      if((*_FEC[i])[j]==0)continue;
      if(_gates[(*_FEC[i])[j]/2]->getOrder()==-1)(*_FEC[i])[j]=0;
    }
  }
}