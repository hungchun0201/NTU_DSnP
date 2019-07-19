/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
  cout<<"================================================================================"<<endl;
  string s="= ";
  s=s+getTypeStr()+"("+to_string(_gateNo)+")";
  if(getId().size()!=0){
     s=s+"\""+getId()+"\"";
    }
  s=s+","+" line "+to_string(_lineNo);
  cout<<s<<endl;
  //FEC;
  if(isAig()){
  cout<<"= FECs:";
  if(_gateNo!=_groupNo/2){
    cout<<" ";
    if(_groupNo%2)cout<<"!";
    cout<<_groupNo/2;
  }
  IdList* group=cirMgr->getFEC(_groupNo/2);
  if(group!=0){
    for(size_t i=0;i<group->size();i++){
      if((*group)[i]/2!=_gateNo){
        cout<<" ";
        if((*group)[i]%2 ^ _groupNo%2)cout<<"!";
        cout<<(*group)[i]/2;
      }
    }
  }
  cout<<endl;
  }
  cout<<"= Value: ";
  size_t val=_value;
  bool v[64];
  for(int i=1;i<=64;i++){
    v[i-1]=val%2;
    val=val/2;
  }
  for(int i=1;i<=64;i++){
    cout<<v[64-i];
    if(i%8==0 && i!=64)cout<<"_";
  }
  cout<<endl;
  cout<<"================================================================================"<<endl;
}

void
CirGate::reportFanin(int level,unsigned blanks,bool inv) const
{ 
   if(blanks==0 && _type==UNDEF_GATE && _oNoList.size()==0)return ;
   GateList& list=cirMgr->getGates();
   assert (level >= 0);
   for(unsigned i=0;i<blanks;i++)cout<<" ";
   if(inv)cout<<"!";
   cout<<getTypeStr()<<" "<<_gateNo;
   if(level==0){cout<<endl;return ;}
   if(_flag){if(_type==AIG_GATE)cout<<" (*)";cout<<endl;return ;}
   cout<<endl;
   
   for(IdList::const_iterator it=_iNoList.begin();it!=_iNoList.end();it++){
      (list)[*it/2]->reportFanin(level-1,blanks+2,*it%2);
   }
   _flag=1;
   if(blanks==0){
      for(GateList::iterator it=(list).begin();it!=(list).end();it++){
         if(*it!=0)(*it)->setFlag(0);
      }
   } 
}

void
CirGate::reportFanout(int level,unsigned blanks,bool inv) const
{
   GateList& list=cirMgr->getGates();
   assert (level >= 0);
   for(unsigned i=0;i<blanks;i++)cout<<" ";
   if(inv)cout<<"!";
   cout<<getTypeStr()<<" "<<_gateNo;
   if(level==0){cout<<endl;return ;}
   if(_flag){if(_type==AIG_GATE)cout<<" (*)";cout<<endl;return ;}
   cout<<endl;
   for(IdList::const_iterator  it=_oNoList.begin();it!=_oNoList.end();it++){
      (list)[*it/2]->reportFanout(level-1,blanks+2,*it%2);
   }
   _flag=1;
   if(blanks==0){
      for(GateList::const_iterator it=(list).begin();it!=(list).end();it++){
          if(*it!=0)(*it)->setFlag(0);
      }
   } 
}
 

bool
CirGate::isFloat()const {
   if(_type==PI_GATE || _type==CONST_GATE ||_type==UNDEF_GATE)return 0;
   for(IdList::const_iterator it=_iNoList.begin();it!=_iNoList.end();it++){
      GateList& list=cirMgr->getGates();
      if(list[*it/2]->getType()==UNDEF_GATE)return 1;
    }
    //if(_iNoList.size()<2)return 1;
    return false;
}

void 
UndefGate::printGate(unsigned& l)const{
}

void 
PiGate::printGate(unsigned& l)const{
   cout<<"["<<l<<"] PI  "<<_gateNo;
   if(_id.size()!=0){ cout<<" ("<<_id<<")";}
   cout<<endl;
}

void 
PoGate::printGate(unsigned &l)const{
  cout<<"["<<l<<"] PO  "<<_gateNo;
  for(IdList::const_iterator it=_iNoList.begin();it!=_iNoList.end();it++ ){
      cout<<" ";
      if(cirMgr->getGate((*it)/2)->getType()==UNDEF_GATE){cout<<"*";}
      if((*it)%2){cout<<"!";}
      cout<<(*it)/2;
  }
  if(_id.size()!=0){ cout<<" ("<<_id<<")";}
  cout<<endl;
}

void 
ConstGate::printGate(unsigned &l)const{
   cout<<"["<<l<<"] CONST0"<<endl;
}

void
AigGate::printGate(unsigned &l)const{
  cout<<"["<<l<<"] AIG "<<_gateNo;
  for(IdList::const_iterator it=_iNoList.begin();it!=_iNoList.end();it++ ){
      cout<<" ";
      if(cirMgr->getGate((*it)/2)->getType()==UNDEF_GATE){cout<<"*";}
      if((*it)%2){cout<<"!";}
      cout<<(*it)/2;
  }
  cout<<endl;
}

void 
CirGate::sweepOut(bool * list){
  for(IdList::iterator it=_oNoList.begin();it!=_oNoList.end();){
      if(list[*it/2]==0){
          *it=_oNoList.back();
          _oNoList.pop_back();
      }
      else{
        ++it;
      }
  }
}

bool
CirGate::optimize(){
   if(_iNoList[0]==0 || _iNoList[1]==0){
       for(IdList::iterator it=_oNoList.begin();it!=_oNoList.end();++it){
          cirMgr->getGate((*it)/2)->replaceIn(_gateNo,0,0);
        }
       for(unsigned i=0;i<2;i++){
         if(_iNoList[i]!=0){
            cirMgr->getGate(_iNoList[i]/2)->replaceOut(_gateNo,IdList(),0);
         }
       }
        cirMgr->getGate(0)->replaceOut(_gateNo,_oNoList,0);
       return optimizeOutput(0);
   } 
   if(_iNoList[0]==1 && _iNoList[1]==1){
       for(IdList::iterator it=_oNoList.begin();it!=_oNoList.end();++it){
          cirMgr->getGate((*it)/2)->replaceIn(_gateNo,0,1);
        }
        cirMgr->getGate(0)->replaceOut(_gateNo,_oNoList,1);
       return optimizeOutput(1);
    }
   if(_iNoList[0]==_iNoList[1]){
       for(IdList::iterator it=_oNoList.begin();it!=_oNoList.end();++it){
          cirMgr->getGate((*it)/2)->replaceIn(_gateNo,_iNoList[0]/2,_iNoList[0]%2);
        }
       cirMgr->getGate(_iNoList[0]/2)->replaceOut(_gateNo,_oNoList,_iNoList[0]%2);
       return optimizeOutput(_iNoList[0]);
      }
   if(_iNoList[0]/2==_iNoList[1]/2){
       for(IdList::iterator it=_oNoList.begin();it!=_oNoList.end();++it){
          cirMgr->getGate((*it)/2)->replaceIn(_gateNo,0,0);
        }
       cirMgr->getGate(0)->replaceOut(_gateNo,_oNoList,1);
       cirMgr->getGate(_iNoList[0]/2)->replaceOut(_gateNo,IdList(),0);

       return optimizeOutput(0);
    }  
    if(_iNoList[0]==1){
       for(IdList::iterator it=_oNoList.begin();it!=_oNoList.end();++it){
          cirMgr->getGate((*it)/2)->replaceIn(_gateNo,_iNoList[1]/2,_iNoList[1]%2);
        }
       cirMgr->getGate(0)->replaceOut(_gateNo,IdList(),0);
       cirMgr->getGate(_iNoList[1]/2)->replaceOut(_gateNo,_oNoList,_iNoList[1]%2);
       return optimizeOutput(_iNoList[1]);
      }
   if(_iNoList[1]==1){
       for(IdList::iterator it=_oNoList.begin();it!=_oNoList.end();++it){
          cirMgr->getGate((*it)/2)->replaceIn(_gateNo,_iNoList[0]/2,_iNoList[0]%2);
        }
       cirMgr->getGate(_iNoList[1]/2)->replaceOut(_gateNo,IdList(),0);
       cirMgr->getGate(_iNoList[0]/2)->replaceOut(_gateNo,_oNoList,_iNoList[0]%2);
       
       return optimizeOutput(_iNoList[0]);
      }
    return 0;
  }
  
void 
CirGate::replaceIn(unsigned idx,unsigned nidx,bool inv){
  assert(nidx!=_gateNo);
   for(IdList::iterator it=_iNoList.begin();it!=_iNoList.end();it++){
      if((*it)/2==idx){
          *it=nidx*2+unsigned(bool((*it)%2)^inv);
      }
   }
}

void CirGate::replaceOut(unsigned idx,const IdList& list  ,bool inv){
  assert(idx!=_gateNo);
  for(unsigned i=0;i<_oNoList.size();){
      if(_oNoList[i]/2==idx){
          _oNoList[i]=_oNoList.back();
          _oNoList.pop_back();
        }
      else i++;
    }
  for(IdList::const_iterator it=list.begin();it!=list.end();it++){
     assert(*it/2!=_gateNo);
     _oNoList.push_back(((*it)/2)*2+unsigned((bool((*it)%2))^inv));
  }
}

bool
CirGate::optimizeOutput(unsigned sourceLit){
   cout<<"Simplifying: "<<sourceLit/2<<" merging ";
   if(sourceLit%2)cout<<"!";
   cout<<_gateNo<<"...\n";
   return 1;
  }

void 
CirGate::replace(unsigned newLit){
   for(IdList::iterator it=_oNoList.begin();it!=_oNoList.end();++it){
      cirMgr->getGate((*it)/2)->replaceIn(_gateNo,newLit/2,newLit%2);
    }
   cirMgr->getGate(_iNoList[0]/2)->replaceOut(_gateNo,IdList(),0);
   cirMgr->getGate(_iNoList[1]/2)->replaceOut(_gateNo,IdList(),0);
   cirMgr->getGate(newLit/2)->replaceOut(_gateNo,_oNoList,newLit%2);
  }
void 
AigGate::sim(){
   size_t v1=cirMgr->getGate(_iNoList[0]/2)->getValue(),v2=cirMgr->getGate(_iNoList[1]/2)->getValue();
   if(_iNoList[0]%2)v1=~v1;
   if(_iNoList[1]%2)v2=~v2;
   _value=v1&v2;
  }
void 
PoGate::sim(){
   _value=cirMgr->getGate(_iNoList[0]/2)->getValue();
   if(_iNoList[0]%2)_value=~_value;
  }
  
void 
AigGate::setGateCNF(SatSolver& sat,IdList& list){
     if(_flag)return;
     _flag=1;
     //cout<<"Setting "<<_gateNo<<endl;
     _var=sat.newVar();
     cirMgr->getGate(_iNoList[0]/2)->setGateCNF(sat,list);
     cirMgr->getGate(_iNoList[1]/2)->setGateCNF(sat,list);
     sat.addAigCNF(_var,cirMgr->getGate(_iNoList[0]/2)->getVar(),_iNoList[0]%2,cirMgr->getGate(_iNoList[1]/2)->getVar(),_iNoList[1]%2);
}

void 
PiGate::setGateCNF(SatSolver& sat,IdList& list){
   if(_flag)return;
   _flag=1;
   _var=sat.newVar();
  }
  
void 
ConstGate::setGateCNF(SatSolver& sat,IdList& list){
   if(_flag)return;
   _flag=1;
   _var=sat.newVar();
  }
void 
UndefGate::setGateCNF(SatSolver& sat,IdList& list){
   if(_flag)return;
   _flag=1;
   list.push_back(_gateNo);
   _var=sat.newVar();
}