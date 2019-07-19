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
#include <regex>
#include <sstream>
#include <utility>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;
typedef stringstream ss;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr *cirMgr = 0;

enum CirParseError
{
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
static unsigned lineNo = 0; // in printint, lineNo needs to ++
static unsigned colNo = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err)
   {
   case EXTRA_SPACE:
      cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
           << ": Extra space character is detected!!" << endl;
      break;
   case MISSING_SPACE:
      cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
           << ": Missing space character!!" << endl;
      break;
   case ILLEGAL_WSPACE: // for non-space white space character
      cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
           << ": Illegal white space char(" << errInt
           << ") is detected!!" << endl;
      break;
   case ILLEGAL_NUM:
      cerr << "[ERROR] Line " << lineNo + 1 << ": Illegal "
           << errMsg << "!!" << endl;
      break;
   case ILLEGAL_IDENTIFIER:
      cerr << "[ERROR] Line " << lineNo + 1 << ": Illegal identifier \""
           << errMsg << "\"!!" << endl;
      break;
   case ILLEGAL_SYMBOL_TYPE:
      cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
           << ": Illegal symbol type (" << errMsg << ")!!" << endl;
      break;
   case ILLEGAL_SYMBOL_NAME:
      cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
           << ": Symbolic name contains un-printable char(" << errInt
           << ")!!" << endl;
      break;
   case MISSING_NUM:
      cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
           << ": Missing " << errMsg << "!!" << endl;
      break;
   case MISSING_IDENTIFIER:
      cerr << "[ERROR] Line " << lineNo + 1 << ": Missing \""
           << errMsg << "\"!!" << endl;
      break;
   case MISSING_NEWLINE:
      cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
           << ": A new line is expected here!!" << endl;
      break;
   case MISSING_DEF:
      cerr << "[ERROR] Line " << lineNo + 1 << ": Missing " << errMsg
           << " definition!!" << endl;
      break;
   case CANNOT_INVERTED:
      cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
           << ": " << errMsg << " " << errInt << "(" << errInt / 2
           << ") cannot be inverted!!" << endl;
      break;
   case MAX_LIT_ID:
      cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
           << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
           << endl;
      break;
   case REDEF_GATE:
      cerr << "[ERROR] Line " << lineNo + 1 << ": Literal \"" << errInt
           << "\" is redefined, previously defined as "
           << errGate->getTypeStr() << " in line " << errGate->getLineNo()
           << "!!" << endl;
      break;
   case REDEF_SYMBOLIC_NAME:
      cerr << "[ERROR] Line " << lineNo + 1 << ": Symbolic name for \""
           << errMsg << errInt << "\" is redefined!!" << endl;
      break;
   case REDEF_CONST:
      cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
           << ": Cannot redefine const (" << errInt << ")!!" << endl;
      break;
   case NUM_TOO_SMALL:
      cerr << "[ERROR] Line " << lineNo + 1 << ": " << errMsg
           << " is too small (" << errInt << ")!!" << endl;
      break;
   case NUM_TOO_BIG:
      cerr << "[ERROR] Line " << lineNo + 1 << ": " << errMsg
           << " is too big (" << errInt << ")!!" << endl;
      break;
   default:
      break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool CirMgr::ParseHeader(ifstream &aagf)
{ //only parsed first sentence
   smatch tok;
   regex aagheader("aag ([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+)");
   string header;
   if (!getline(aagf, header))
   {
      return false;
   }
   if (!regex_match(header, tok, aagheader))
   {
      // need to remove later
      return false;
   }
   lineNo++;
   ss tmp;
   for (size_t i = 0; i < 5; i++)
   {
      // cerr << tok[i].str() << endl;
      tmp << tok[i + 1].str() << " ";
   }
   tmp >> Circuit.maxid;
   tmp >> Circuit.inputs;
   tmp >> Circuit.latches;
   tmp >> Circuit.outputs;
   tmp >> Circuit.ands;
   // one for const 0
   Circuit.id2Gate = new CirGate *[Circuit.maxid + Circuit.outputs + 1]();
   Circuit.DFS_List_booltype.resize(Circuit.maxid + Circuit.outputs + 1, false);
   for (size_t i = 0; i < Circuit.inputs + 1; ++i)
   {
      Circuit.DFS_List_booltype.at(i) = true;
   }
   Circuit.has_sim = false;
   return true;
}
bool CirMgr::GenGates(ifstream &aagf)
{
   regex gateformat("([0-9]+)");
   smatch tok;
   string gate;
   int lit = 0;
   // Gen constant 0
   Circuit.id2Gate[0] = new ConstGate;
   // input
   Circuit.PI_list = new unsigned[Circuit.inputs]();
   for (size_t i = 0; i < Circuit.inputs; i++)
   {
      if (!getline(aagf, gate))
         return false;
      if (!regex_match(gate, tok, gateformat))
         return false;
      lineNo++;
      myStr2Int(tok[1], lit);
      Circuit.id2Gate[lit / 2] = new InputGate;
      Circuit.id2Gate[lit / 2]->setLineNo(i + 2);
      Circuit.PI_list[i] = lit / 2; //store all off the index of PI
   }
   // outputs
   for (size_t i = 0; i < Circuit.outputs; i++)
   {
      if (!getline(aagf, gate))
         return false;
      if (!regex_match(gate, tok, gateformat))
         return false;
      lineNo++;
      myStr2Int(tok[1], lit);
      Circuit.id2Gate[Circuit.maxid + 1 + i] = new OutputGate(lit);
      Circuit.id2Gate[Circuit.maxid + 1 + i]->setLineNo(i + 2 +
                                                        Circuit.inputs);
   }
   // AIG
   regex Andformat("([0-9]+) ([0-9]+) ([0-9]+)");
   for (size_t i = 0; i < Circuit.ands; i++)
   {
      // haven't add symbol parsing
      if (!getline(aagf, gate))
         return false;
      if (!regex_match(gate, tok, Andformat))
         return false;
      lineNo++;
      int lit[3];
      for (size_t j = 0; j < 3; j++)
      {
         myStr2Int(tok[j + 1].str(), lit[j]);
      }
      Circuit.id2Gate[lit[0] / 2] = new AndGate(lit[1], lit[2]); //store the literal of _fanin
      Circuit.id2Gate[lit[0] / 2]->setLineNo(i + 2 + Circuit.outputs +
                                             Circuit.inputs);
   }
   // symbols

   regex Symbolformat("([io])([0-9]+) ([^\n\r]+)");
   while (getline(aagf, gate))
   {
      if (gate == "c")
         break;
      if (!regex_match(gate, tok, Symbolformat))
         return false;
      lineNo++;
      if (tok[1] == "i")
      {
         int inputid = 0;
         myStr2Int(tok[2], inputid);
         InputGate *i =
             (InputGate *)Circuit.id2Gate[inputid + 1]; //Because id2Gate will return Cirgate* type
         i->setSymbol(tok[3].str());
      }
      else if (tok[1] == "o")
      {
         int outputid = 0;
         myStr2Int(tok[2], outputid);
         OutputGate *o =
             (OutputGate *)Circuit.id2Gate[Circuit.maxid + outputid + 1];
         o->setSymbol(tok[3].str());
      }
   }
   return true;
}
bool CirMgr::ConstructCir()
{
   for (size_t i = 0; i < Circuit.maxid + Circuit.outputs + 1; i++)
   { //i means the ID
      if (Circuit.id2Gate[i] != 0)
      {
         if (Circuit.id2Gate[i]->getType() == AIG_GATE)
         {
            unsigned *f = Circuit.id2Gate[i]->getFanin(); //return the _fanin(a unsigned*)
            //the _fanin of AIG_Gate only store the lit.
            // inputs _fanout only store the unsigned number,too.

            if (Circuit.id2Gate[f[0] / 2] == 0)           //if the fanin gate is not constructed before
               Circuit.id2Gate[f[0] / 2] = new UndefGate; //Theoritically,we have designed all valid gate.
            Circuit.id2Gate[f[0] / 2]->_fanout.push_back(i * 2 +
                                                         (f[0] % 2)); //Record the _fanout in the input gate
            if (Circuit.id2Gate[f[1] / 2] == 0)
               Circuit.id2Gate[f[1] / 2] = new UndefGate;
            Circuit.id2Gate[f[1] / 2]->_fanout.push_back(i * 2 +
                                                         (f[1] % 2));
            //NOTE!! _fanout list store the literal,so it contains the invert message
         }
         if (Circuit.id2Gate[i]->getType() == PO_GATE)
         {
            unsigned *f = Circuit.id2Gate[i]->getFanin();
            if (Circuit.id2Gate[*f / 2] == 0)
               Circuit.id2Gate[*f / 2] = new UndefGate; //same as AIG
            Circuit.id2Gate[*f / 2]->_fanout.push_back(i * 2 + (*f % 2));
         }
      }
   }
   return true;
}
bool CirMgr::readCircuit(const string &fileName)
{
   ifstream aagf(fileName);
   // finished read in the buf
   if (!aagf)
   {
      cerr << "Cannot open design \"" << fileName << "\"!!" << endl;
      aagf.close();
      return false;
   }
   if (!ParseHeader(aagf))
      return false;
   if (!GenGates(aagf))
      return false;

   if (!ConstructCir())
      return false;
   aagf.close();
   return true;
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
void CirMgr::printSummary() const
{
   cout << endl
        << "Circuit Statistics" << endl
        << "==================" << endl
        << setw(4) << "PI" << setw(12) << Circuit.inputs << endl
        << setw(4) << "PO" << setw(12) << Circuit.outputs << endl
        << setw(5) << "AIG" << setw(11) << Circuit.ands << endl
        << "------------------" << endl
        << setw(7) << "Total" << setw(9)
        << Circuit.inputs + Circuit.outputs + Circuit.ands << endl;
   return;
}
void CirMgr::printNetlistformat(unsigned id, unsigned prid)
{
   cout << "[" << prid << "] " << setiosflags(ios::left) << setw(4)
        << Circuit.id2Gate[id]->getTypeStr() << resetiosflags(ios::left); //clean the effect of ios::left
   return;
}

/**********************
 * DFS_part           *
 * 
 * 
 * *******************/

void CirMgr::DFSTravPO(unsigned id, unsigned &prid, DFS_utilize usage = PRINT)
{                                                 //prid means the index of netlist
   unsigned *c = Circuit.id2Gate[id]->getFanin(); // variable c means the vector of _fanin in current id
   if (Circuit.id2Gate[id]->getType() == PO_GATE)
   {
      // cout<<"PO_Gate_fanin :"<<*c<<endl;
      if (!Circuit.id2Gate[*c / 2]->isGlobalref())
         DFSTravPO(*c / 2, prid, usage);
      //if we didn't run this gate,ignore it,keep going down
      if (usage == PRINT)
      {
         SymbolGate *s = dynamic_cast<SymbolGate *>(Circuit.id2Gate[id]);
         printNetlistformat(id, prid);
         cout << id << " "
              << ((Circuit.id2Gate[*c / 2]->getType() == UNDEF_GATE) ? "*" : "") //if _fanin is UNDEF
              << ((*c % 2 == 1) ? "!" : "")                                      //if _fanin is inverted
              << *c / 2
              << ((s->getSymbol() == 0)
                      ? string("")
                      : (string(" (") + string(s->getSymbol()) + string(")")))
              << endl;
      }
      else if (usage == SWEEP)
      {
         Circuit.DFS_List_booltype.at(id) = true;
      }
      else if (usage == OPTIMIZE)
      {
         Circuit.DFS_List_unsignedtype.push_back(id);
      }
      else if (usage == SIMULATION)
      {
         unsigned *fanin = Circuit.id2Gate[id]->_fanin;
         size_t sim0 = Circuit.id2Gate[*fanin / 2]->getSim_pattern();
         if (fanin[0] % 2 == 1)
         {
            sim0 = ~sim0;
         }
         Circuit.id2Gate[id]->sim_pattern = sim0;
      }
      else if (usage == FRAIG)
      {
         Var v = solver.newVar();
         Circuit.id2Gate[id]->setVar(v);
      }
   }
   else if (Circuit.id2Gate[id]->getType() == PI_GATE)
   {
      if (usage == PRINT)
      {
         printNetlistformat(id, prid);
         SymbolGate *s = dynamic_cast<SymbolGate *>(Circuit.id2Gate[id]);
         cout << id
              << ((s->getSymbol() == 0)
                      ? string("")
                      : (string(" (") + string(s->getSymbol()) + string(")"))) //cout name
              << endl;
      }

      else if (usage == SWEEP)
      {
         Circuit.DFS_List_booltype.at(id) = true;
      }
      else if (usage == OPTIMIZE)
      {
         Circuit.DFS_List_unsignedtype.push_back(id);
      }
      else if (usage == FRAIG)
      {
         Var v = solver.newVar();
         Circuit.id2Gate[id]->setVar(v);
      }
   }
   else if (Circuit.id2Gate[id]->getType() == AIG_GATE)
   {
      if (!Circuit.id2Gate[c[0] / 2]->isGlobalref())
      {
         DFSTravPO(c[0] / 2, prid, usage);
      }
      if (!Circuit.id2Gate[c[1] / 2]->isGlobalref())
      {
         DFSTravPO(c[1] / 2, prid, usage);
      }
      if (usage == PRINT)
      {
         printNetlistformat(id, prid);
         cout << id << " "
              << ((Circuit.id2Gate[c[0] / 2]->getType() == UNDEF_GATE) ? "*"
                                                                       : "")
              << ((c[0] % 2 == 1) ? "!" : "") << c[0] / 2 << " "
              << ((Circuit.id2Gate[c[1] / 2]->getType() == UNDEF_GATE) ? "*"
                                                                       : "")
              << ((c[1] % 2 == 1) ? "!" : "") << c[1] / 2 << endl;
      }
      else if (usage == SWEEP)
      {
         Circuit.DFS_List_booltype.at(id) = true;
      }
      else if (usage == OPTIMIZE)
      {
         Circuit.DFS_List_unsignedtype.push_back(id);
      }
      else if (usage == STRASH)
      {
         Strash_fanin temp(Circuit.id2Gate[id]->_fanin[0], Circuit.id2Gate[id]->_fanin[1]);
         auto it3 = Circuit.DFS_List_hash.find(temp);
         if (it3 == Circuit.DFS_List_hash.end())
         {
            Circuit.DFS_List_hash.insert({temp, id});
         }
         else
         {
            mergeAndDeleteGate(id*2, (it3->second)*2, STRASH);
         }
      }
      else if (usage == SIMULATION)
      {
         unsigned *fanin = Circuit.id2Gate[id]->_fanin;
         size_t sim0 = Circuit.id2Gate[fanin[0] / 2]->getSim_pattern();
         size_t sim1 = Circuit.id2Gate[fanin[1] / 2]->getSim_pattern();
         if (fanin[0] % 2 == 1)
         {
            sim0 = ~sim0;
         }
         if (fanin[1] % 2 == 1)
         {
            sim1 = ~sim1;
         }
         size_t sim_cur = Circuit.id2Gate[id]->sim_pattern = sim0 & sim1;
         auto it_sim = Circuit.FECgroups.find(sim_cur);
         if (Circuit.has_sim == false)
         {
            if (it_sim != Circuit.FECgroups.end()) //find the existed FEC
            {
               it_sim->second->push_back(id * 2);
               goto end;
            }
            it_sim = Circuit.FECgroups.find(~sim_cur);
            if (it_sim != Circuit.FECgroups.end()) //find the existed IFEC
            {
               it_sim->second->push_back(id * 2 + 1);
               goto end;
            }
            else //construct a hash
            {
               IdList *temp = new IdList(1, id * 2);
               Circuit.FECgroups.insert({sim_cur, temp});
            }
         end:;
         }
      }
      else if (usage == FRAIG)
      {
         Circuit.DFS_List_unsignedtype.push_back(id);
         Var v = solver.newVar();
         Circuit.id2Gate[id]->setVar(v);
      }
   }
   else if (Circuit.id2Gate[id]->getType() == UNDEF_GATE)
   {
      Circuit.id2Gate[id]->setRefToGlobalRef();
      if (usage == SWEEP)
      {
         Circuit.DFS_List_booltype.at(id) = true;
      }
      else if (usage == OPTIMIZE)
      {
         Circuit.DFS_List_unsignedtype.push_back(id);
      }
      return;
   }
   else if (Circuit.id2Gate[id]->getType() == CONST_GATE)
   {
      if (usage == PRINT)
      {
         printNetlistformat(id, prid);
         cout << id << endl;
      }
      else if (usage == SWEEP)
      {
         Circuit.DFS_List_booltype.at(id) = true;
      }
      else if (usage == OPTIMIZE)
      {
         Circuit.DFS_List_unsignedtype.push_back(id);
      }
      else if(usage == FRAIG)
      {
         Var v = solver.newVar();
         Circuit.id2Gate[id]->setVar(v);
      }
   }
   else
   {
      // gate unknown type
      printNetlistformat(id, prid);
      cout << "0" << endl;
   }
   if (!(Circuit.id2Gate[id] == NULL))
      Circuit.id2Gate[id]->setRefToGlobalRef();
   prid++;
   return;
}

void CirMgr::printNetlist()
{
   CirGate::setGlobalref();
   static unsigned printid = 0;
   printid = 0;
   cout << endl;

   //travel DFS by different PO!!
   for (size_t i = Circuit.maxid + 1; i < Circuit.maxid + Circuit.outputs + 1;
        i++)
   {
      DFSTravPO(i, printid);
   }
}

bool CirMgr::DFS_for_sweep()
{
   Circuit.DFS_List_unsignedtype.clear();
   static unsigned printid = 0;
   printid = 0;
   CirGate::setGlobalref();
   Circuit.DFS_List_booltype.resize(Circuit.maxid + Circuit.outputs + 1, false);
   for (size_t i = Circuit.maxid + 1; i < Circuit.maxid + Circuit.outputs + 1;
        i++)
   {
      DFSTravPO(i, printid, SWEEP);
   }
   return true;
}

bool CirMgr::DFS_for_optimize()
{
   Circuit.DFS_List_unsignedtype.clear();
   static unsigned printid = 0;
   printid = 0;
   CirGate::setGlobalref();
   Circuit.DFS_List_unsignedtype.reserve(Circuit.maxid + Circuit.outputs + 1);
   for (size_t i = Circuit.maxid + 1; i < Circuit.maxid + Circuit.outputs + 1;
        i++)
   {
      DFSTravPO(i, printid, OPTIMIZE);
   }
}

bool CirMgr::DFS_for_strash()
{
   Circuit.DFS_List_hash.clear();
   static unsigned printid = 0;
   printid = 0;
   CirGate::setGlobalref();
   Circuit.DFS_List_hash.reserve(Circuit.maxid);
   for (size_t i = Circuit.maxid + 1; i < Circuit.maxid + Circuit.outputs + 1;
        i++)
   {
      DFSTravPO(i, printid, STRASH);
   }
}
/******************
 * print PI and PO
******************/

void CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for (size_t i = 0; i < Circuit.inputs; i++)
   {
      cout << " " << Circuit.PI_list[i];
   }
   cout << endl;
}

void CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for (size_t i = Circuit.maxid + 1; i < Circuit.maxid + Circuit.outputs + 1;
        i++)
   {
      if (Circuit.id2Gate[i] != 0 && Circuit.id2Gate[i]->getType() == PO_GATE)
         cout << " " << i;
   }
   cout << endl;
}

void CirMgr::printFloatGates() const
{
   bool flag = false;
   ss outs;
   outs << "Gates with floating fanin(s):";
   for (size_t i = 0; i < Circuit.maxid + Circuit.outputs + 1; i++)
   {
      if (Circuit.id2Gate[i] != 0)
      {
         unsigned *c = Circuit.id2Gate[i]->getFanin();
         if (Circuit.id2Gate[i]->getType() == PO_GATE &&
             Circuit.id2Gate[*c / 2]->getType() == UNDEF_GATE)
         {
            outs << " " << i;
            flag = true;
         }
         else if (Circuit.id2Gate[i]->getType() == AIG_GATE)
         {
            if (Circuit.id2Gate[c[0] / 2]->getType() == UNDEF_GATE ||
                Circuit.id2Gate[c[1] / 2]->getType() == UNDEF_GATE)
            {
               outs << " " << i;
               flag = true;
            }
         }
      }
   }
   if (flag)
      cout << outs.str() << endl;
   outs.str("");
   flag = false;
   outs << "Gates defined but not used  :";
   for (size_t i = 1; i < Circuit.maxid + 1; i++)
   {
      if (Circuit.id2Gate[i] != 0)
      {
         if (Circuit.id2Gate[i]->getFanout().empty())
         {
            outs << " " << i;
            flag = true;
         }
      }
   }
   if (flag)
      cout << outs.str() << endl;
   return;
}
void CirMgr::writeAag(ostream &outfile) const
{
   outfile << "aag " << Circuit.maxid << " " << Circuit.inputs << " "
           << Circuit.latches << " " << Circuit.outputs << " ";
   //outfile << trueaig << endl;
   CirGate::setGlobalref();
   vector<unsigned> AIGlist;
   for (size_t i = Circuit.maxid + 1; i < Circuit.maxid + Circuit.outputs + 1;
        i++)
   {
      Circuit.writeAig(i, AIGlist);
   }
   outfile << AIGlist.size() / 3 << endl;
   for (size_t i = 0; i < Circuit.inputs; i++)
   {
      outfile << Circuit.PI_list[i] * 2 << endl;
   }
   for (size_t i = Circuit.maxid + 1; i < Circuit.outputs + Circuit.maxid + 1;
        i++)
   {
      outfile << *(Circuit.id2Gate[i]->getFanin()) << endl;
   }
   for (size_t i = 0; i < AIGlist.size();
        i += 3)
   {
      outfile << AIGlist[i] << " " << AIGlist[i + 1] << " " << AIGlist[i + 2] << endl;
   }
   for (size_t i = 0; i < Circuit.inputs; i++)
   {
      SymbolGate *s =
          dynamic_cast<SymbolGate *>(Circuit.id2Gate[Circuit.PI_list[i]]);
      if (s->getSymbol() != 0)
      {
         outfile << "i" << i << " " << s->getSymbol() << endl;
      }
   }
   for (size_t i = Circuit.maxid + 1; i < Circuit.maxid + Circuit.outputs + 1;
        i++)
   {
      SymbolGate *s = dynamic_cast<SymbolGate *>(Circuit.id2Gate[i]);
      if (s->getSymbol() != 0)
      {
         outfile << "o" << i - (Circuit.maxid + 1) << " " << s->getSymbol()
                 << endl;
      }
   }
   outfile << "c" << endl
           << "AAG output by b06901048 Justin Chen" << endl;
}

void CirMgr::ParsedCir::writeAig(int id, vector<unsigned> &AIGlist) const
{
   unsigned *c = id2Gate[id]->getFanin();
   if (id2Gate[id]->getType() == PO_GATE)
   {
      if (!id2Gate[*c / 2]->isGlobalref())
         writeAig(*c / 2, AIGlist);
   }
   else if (id2Gate[id]->getType() == AIG_GATE)
   {
      if (!id2Gate[c[0] / 2]->isGlobalref())
      {
         writeAig(c[0] / 2, AIGlist);
      }
      if (!id2Gate[c[1] / 2]->isGlobalref())
      {
         writeAig(c[1] / 2, AIGlist);
      }
      AIGlist.push_back(id * 2);
      AIGlist.push_back(c[0]);
      AIGlist.push_back(c[1]);
      //outfile << id * 2 << " " << c[0] << " " << c[1] << endl;
   }
   id2Gate[id]->setRefToGlobalRef();
   return;
}

void CirMgr::printFECPairs()
{
   writeToLog(cout);
}

void CirMgr::writeGate(ostream &outfile, CirGate *g) const
{
}
/***************************
 * utilize
 * *************************/
bool CirMgr::Delete_gate(unsigned gateId) //for AIG
{
   unsigned *fanin_list = Circuit.id2Gate[gateId]->_fanin;
   if (fanin_list != NULL)
   {
      for (int i = 0; i < 2; ++i)
      {
         if (fanin_list[i] / 2 > gateId)
            erase_fanout_data(Circuit.id2Gate[fanin_list[i] / 2]->_fanout, gateId);
         // divide by 2 is essential!!! Because all value store in _fanin and _fanout
         //are LITERAL!!!
      }
   }
   cout << "Sweeping: " << Circuit.id2Gate[gateId]->getTypeStr()
        << "(" << gateId << ") removed..."
        << endl;
   if (Circuit.id2Gate[gateId]->getType() == AIG_GATE)
      --Circuit.ands;
   Circuit.id2Gate[gateId] = NULL;
}
