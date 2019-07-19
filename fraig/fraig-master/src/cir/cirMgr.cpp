/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
   reset();
   ifstream infile(fileName);
   if(!infile){return false;}
   lineNo=0;
   if(!readHeader(infile))return 0;
   if(!readInput(infile))return 0;
   if(!readOutput(infile))return 0;
   if(!readAig(infile))return 0;
   if(!readId(infile))return 0;
   searchGates(_DFS);
   _FEC.resize(header[0]+1,0);
   infile.close();
   return 1;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
  cout<<"\nCircuit Statistics\n";
  cout<<"==================\n";
  cout<<"  PI  "<<setw(10)<<_pis.size()<<endl;
  cout<<"  PO  "<<setw(10)<<_pos.size()<<endl;
  cout<<"  AIG "<<setw(10)<<_aigCount<<endl;
  cout<<"------------------\n";
  cout<<"  Total "<<setw(8)<<_pis.size()+_aigCount+_pos.size()<<endl;
  return ;
}
void
CirMgr::printNetlist() const
{
    lineNo=0;
    cout<<endl;
    for(IdList::const_iterator it=_DFS.begin();it!=_DFS.end();++it){
      _gates[*it]->printGate(lineNo);
      lineNo++;
    }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(IdList::const_iterator it=_pis.begin();it!=_pis.end();it++){
      cout<<" "<<*it;
   }
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(unsigned i=0;i<header[3];i++){
      cout<<" "<<i+1+header[0];
   }
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   for(GateList::const_iterator i=_gates.begin();i!=_gates.end();i++){
      if((*i)==NULL)continue;
      if((*i)->isFloat()){
           cout<<"Gates with floating fanin(s):";
           for(GateList::const_iterator it=_gates.begin();it!=_gates.end();it++){
            if((*it)==NULL)continue;
            if((*it)->isFloat())cout<<" "<<(*it)->getGateNo();
          }
          cout<<endl;
          break;
      }
    }
   
   
   for(GateList::const_iterator i=_gates.begin();i!=_gates.end();i++){
       if((*i)==NULL)continue;
       if((*i)->notUsed()){
           cout<<"Gates defined but not used  :";
           for(GateList::const_iterator it=_gates.begin();it!=_gates.end();it++){
            if((*it)==NULL)continue;
            if((*it)->notUsed())cout<<" "<<(*it)->getGateNo();
          }
          cout<<endl;
          break;
      }
    }
}

void
CirMgr::printFECPairs() const
{
   unsigned count=0;
   for(size_t i=0;i<_FEC.size();i++){
      if(_FEC[i]==0)continue;
      if(_FEC[i]->empty())continue;
      cout<<"["<<count<<"] "<<i;
      for(IdList::iterator it2=_FEC[i]->begin();it2!=_FEC[i]->end();it2++){
          if(*it2==0)continue;
          cout<<" ";
          if((*it2)%2)cout<<"!";
          cout<<(*it2)/2;
        }
      cout<<endl;
      count++;
   }
}

void
CirMgr::writeAag(ostream& outfile) const
{
   unsigned aig=0;
   for(unsigned i=0;i<_DFS.size();i++){
     if(_gates[_DFS[i]]->isAig())aig++;
   }
   outfile<<"aag";
   for(unsigned i=0;i<=3;i++){
      outfile<<" "<<header[i];
    }
   outfile<<" "<<aig<<endl;
   for(unsigned i=0;i<_pis.size();i++){
      outfile<<2*_pis[i]<<endl;
    } 
   for(unsigned i=0;i<header[3];i++) {
      outfile<<_gates[header[0]+i+1]->getSource()<<endl;
    }
   for(unsigned i=0;i<_DFS.size();i++){
      if(_gates[_DFS[i]]->isAig()==false)continue;
      outfile<<2*_DFS[i];
      IdList fanin= _gates[_DFS[i]]->getInNo();
      for(IdList::iterator it=fanin.begin();it!=fanin.end();it++){
         outfile<<" "<<*it;
      }
      outfile<<endl;  
    }
   for(unsigned i=0;i<_pis.size();i++){
      string id=_gates[_pis[i]]->getId();
      if(id.size()!=0){
         outfile<<"i"<<i<<" "<<id<<endl;
      }
    } 
   for(unsigned i=0;i<header[3];i++){
      string id=_gates[i+1+header[0]]->getId();
      if(id.size()!=0){
         outfile<<"o"<<i<<" "<<id<<endl;
      }
    }
}


void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
    IdList subdfs;
    searchGates(subdfs,g->getGateNo());
    for(unsigned i=0;i<subdfs.size();i++){
        _gates[subdfs[i]]->setFlag();
    }
   unsigned aig=0,pi=0,max=0;
   for(unsigned i=0;i<subdfs.size();i++){
     if(subdfs[i]>max)max=subdfs[i];
     if(_gates[subdfs[i]]->getType()==PI_GATE){pi++;_gates[subdfs[i]]->setFlag(1);}
     if(_gates[subdfs[i]]->isAig())aig++;
   }
   outfile<<"aag";
   outfile<<" "<<max;
   outfile<<" "<<pi;
   outfile<<" "<<0;
   outfile<<" "<<1;
   outfile<<" "<<aig<<endl;
   for(unsigned i=0;i<_pis.size();i++){
      if(_gates[_pis[i]]->getFlag()){
         outfile<<2*_pis[i]<<endl;
      } 
   }
   outfile<<2*(g->getGateNo())<<endl;
   for(unsigned i=0;i<subdfs.size();i++){
     if(_gates[subdfs[i]]->isAig()){
        outfile<<2*subdfs[i];
        IdList fanin=_gates[subdfs[i]]->getInNo();
        for(IdList::iterator it=fanin.begin();it!=fanin.end();it++){
          outfile<<" "<<*it;
          }
        outfile<<endl;
     }
    }
   unsigned input=0;
   for(unsigned i=0;i<_pis.size();i++){
     if(_gates[_pis[i]]->getFlag()){
      if(_gates[_pis[i]]->getId().size()!=0)outfile<<"i"<<input<<" "<<_gates[_pis[i]]->getId()<<endl;
      input++;
      _gates[_pis[i]]->setFlag();
     }
    }
    outfile<<"o0 "<<g->getGateNo()<<endl;
}

void 
CirMgr::searchGates(IdList & result,unsigned thisGate) const {
   if(thisGate==-1){
     for(unsigned i=0;i<_gates.size();i++){
        if(_gates[i]!=0)_gates[i]->setOrder(-1);
      }
     result.clear();
      for(unsigned i=0;i<header[3];i++){
         searchGates(result,i+1+header[0]);
      }
      for(unsigned i=0;i<result.size();i++){
        _gates[result[i]]->setFlag();
      }
   }
   else {
     if(_gates[thisGate]==0)return;
     if(_gates[thisGate]->getFlag())return;
     
     //if(thisGate==0)return ;
     if(_gates[thisGate]->getType()!=UNDEF_GATE)_gates[thisGate]->setFlag(1);
     IdList fanin=_gates[thisGate]->getInNo();
     for(IdList::iterator it=fanin.begin();it!=fanin.end();it++){
        searchGates(result,*it/2);
      }
    
     if(_gates[thisGate]->getType()!=UNDEF_GATE){
       result.push_back(thisGate);
       _gates[thisGate]->setOrder(result.size());
     }
   }
}

bool 
CirMgr::readHeader(ifstream& ifs){
   ifs>>buf;
   for(int i=0;i<5;i++){
      ifs>>header[i];
   }
   _gates.resize(header[0]+header[3]+1,0);
   _gates[0]=new ConstGate(0,0);
   lineNo++;
   return true;
}

bool 
CirMgr::readInput(ifstream& ifs){
   unsigned num;
   for(int i=0;i<header[1];i++){
      ifs>>num;
      lineNo++;
      _gates[num/2]=new PiGate(lineNo,num/2);
      _pis.push_back(num/2);
    }
   return true;
}

bool
CirMgr::readOutput(ifstream& ifs){
   unsigned num;
   for(unsigned i=0;i<header[3];i++){
      lineNo++;
      ifs>>num;
      _gates[header[0]+i+1]=new PoGate(lineNo,header[0]+i+1);
      _gates[header[0]+i+1]->addIn(num);
      _pos.push_back(num);
    }
    return true;
}

bool
CirMgr::readAig(ifstream& ifs){
   unsigned num[3];
   for(unsigned i=0;i<header[4];i++){
      lineNo++;
      ifs>>num[0];ifs>>num[1];ifs>>num[2];
      _gates[num[0]/2]=new AigGate(lineNo,num[0]/2);
      _gates[num[0]/2]->addIn(num[1]);
      _gates[num[0]/2]->addIn(num[2]);
      _aigCount++;
    }
   for(unsigned i=0;i<_gates.size();i++){
      if(_gates[i]==NULL)continue;
      IdList inList=_gates[i]->getInNo();
      for(IdList::iterator it=inList.begin();it!=inList.end();it++){
         if(_gates[(*it)/2]==0){
           _gates[(*it)/2]=new UndefGate(0,(*it)/2);
         }
         _gates[(*it)/2]->addOut(2*i+(*it)%2);
      }
    }
   return true;
}

bool
CirMgr::readId(ifstream& ifs){
   ifs.get(*buf);
   while(true){
      lineNo++;
      ifs.getline(buf,1024, ' ');
      int num;
      if(buf[0]=='i'){
         myStr2Int(buf+1,num);
         unsigned gateNo=_pis[num];
         ifs.getline(buf,1024);
         _gates[gateNo]->setId(buf);
      }
      else if(buf[0]=='o'){
         myStr2Int(buf+1,num);
         unsigned gateNo=header[0]+1+num;
         ifs.getline(buf,1024);
         _gates[gateNo]->setId(buf);
      }
      else return true;
   }

}