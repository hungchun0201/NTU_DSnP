/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <fstream>
#include <cassert>
#include <cstring>
#include <regex>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

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

size_t CirMgr::counter_netlist = 0;
bool CirMgr::is_UNDEF_0 = false;
bool CirMgr::is_UNDEF_1 = false;

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
           << ": Cannot redefine constant (" << errInt << ")!!" << endl;
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
bool CirMgr::readCircuit(const string &fileName)
{
   fstream myfile;
   myfile.open(fileName, ios::in);
   string input;
   string *header = new string[5];
   int *header_int = new int[5];
   int input_to_int = 0;
   lineNo = 0;
   regex reg("^aag ([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+) ([0-9]+)");
   regex reg2("([0-9]+) ([0-9]+) ([0-9]+)");
   regex reg3("i([0-9]+) (.+)");
   regex reg4("o([0-9]+) (.+)");
   smatch sm;
   if (myfile.is_open())
   {
      //         ====initial holy CONST0====
      CirGate *Const0 = new CONST0_Gate();
      GateMap[0] = Const0;
      //         ====read First Line====
      getline(myfile, input, '\n');
      ++lineNo;
      if (regex_search(input, sm, reg))
      {
         for (int i = 0; i < 5; ++i)
         {
            header[i] = sm[i + 1].str();
            myStr2Int(header[i], header_int[i]);
            switch (i)
            {
            case 0:
               Maximal_Variable_Index = header_int[i];
               break;
            case 1:
               Input_Number = header_int[i];
               break;
            case 2:
               break;
            case 3:
               Output_Number = header_int[i];
               break;
            case 4:
               AIG_Number = header_int[i];
               break;
            }
         }
         delete[] header;
         delete[] header_int;
      }
      /*        ====forPI====         */
      for (unsigned i = 0; i < Input_Number; ++i)
      {
         if (getline(myfile, input, '\n'))
         {
            ++lineNo;
            myStr2Int(input, input_to_int);
            add_PI(input_to_int / 2, lineNo);
            // cout << input << endl;
         }
      }
      /*        ====forPo====         */
      for (unsigned i = 0; i < Output_Number; ++i)
      {
         if (getline(myfile, input, '\n'))
         {
            ++lineNo;
            myStr2Int(input, input_to_int);
            add_PO(input_to_int / 2, lineNo, input_to_int % 2);
         }
      }
      /*        ====forAIG====         */
      for (unsigned i = 0; i < AIG_Number; ++i)
      {
         unsigned gate_id, in1, in2;
         if (getline(myfile, input, '\n'))
         {
            lineNo++;
            // cout << input << endl;
            if (regex_search(input, sm, reg2))
            {
               for (int i = 0; i < 3; ++i)
               {
                  switch (i)
                  {
                  case 0:
                     myStr2Unsigned(sm[i + 1].str(), gate_id);
                     break;
                  case 1:
                     myStr2Unsigned(sm[i + 1].str(), in1);
                     break;
                  case 2:
                     myStr2Unsigned(sm[i + 1].str(), in2);
                     break;
                  }
                  // cout << "sm[" << i << "] : " << sm[i + 1] << endl;
               }
               add_AIG(gate_id / 2, lineNo, in1 / 2, in2 / 2, in1 % 2, in2 % 2);
            }
         }
      }
      //       ====forInputName====
      while (getline(myfile, input, '\n'))
      {
         unsigned temp = 0;
         if (regex_search(input, sm, reg3))
         {
            myStr2Unsigned(sm[1].str(), temp);
            source_list[temp]->_name = sm[2].str();
         }
         else
         {
            break;
         }
      }
      if (input == "c")
      {
         return true;
      }
      //       ====forOutputName====
      do
      {
         unsigned temp = 0;
         if (regex_search(input, sm, reg4))
         {
            myStr2Unsigned(sm[1].str(), temp);
            sink_list[temp]->_name = sm[2].str();
         }
         else
         {
            break;
         }
      } while (getline(myfile, input, '\n'));
      return true;
   }
   else
   {
      cerr << "cannot open file!" << endl;
      return false;
   }
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
   cout<<endl;
   cout << "Circuit Statistics" << endl;
   cout << "==================" << endl;
   cout << "  PI" << setw(12) << Input_Number << "" << endl;
   cout << "  PO" << setw(12) << Output_Number << "" << endl;
   cout << "  AIG" << setw(11) << AIG_Number << "" << endl;
   cout << "------------------" << endl;
   cout << "  Total" << setw(9) << AIG_Number + Output_Number + Input_Number << "" << endl;
}

void CirMgr::printNetlist()
{
   cout<<endl;
   Execute_DFS();
}

void CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for (auto it = source_list.begin(); it != source_list.end(); ++it)
   {
      cout << " " << (*it)->_Id;
   }
   cout << endl;
}

void CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for (auto it = sink_list.begin(); it != sink_list.end(); ++it)
   {
      cout << " " << (*it)->_Id;
   }
   cout << endl;
}

void CirMgr::printFloatGates() const
{
   print_floating_fanin = false;
   for (auto it = GateMap.begin(); it != GateMap.end(); ++it)
   {
      if (it->second->type_name == AIG_GATE || it->second->type_name == PO_GATE)
      {
         for (auto it2 = it->second->_faninList.begin(); it2 != it->second->_faninList.end(); ++it2)
         {
            if ((*it2)->type_name == UNDEF_GATE)
            {
               if (!print_floating_fanin)
               {
                  cout << "Gates with floating fanin(s):";
                  print_floating_fanin = true;
               }
               cout << " " << it->second->_Id;
               break;
            }
         }
      }
   }
   if (print_floating_fanin)
   {
      cout << endl;
   }
   print_defined_but_not_used = false;
   for (auto it = GateMap.begin(); it != GateMap.end(); ++it)
   {
      if (it->second->type_name == AIG_GATE || it->second->type_name == PI_GATE)
      {
         if (it->second->_fanoutList.empty())
         {
            if (!print_defined_but_not_used)
            {
               cout << "Gates defined but not used  :";
               print_defined_but_not_used = true;
            }
            cout << " " << it->second->_Id;
         }
      }
   }
   if (print_defined_but_not_used)
   {
      cout << endl;
   }
}

void CirMgr::writeAag(ostream &outfile) const
{
   Array<unsigned> storage;
   Execute_DFS_for_writeout(storage);
   unsigned Count_name = 0;
   outfile << "aag " << Maximal_Variable_Index << ' ' << Input_Number << " 0 " << Output_Number << ' ' << Counting_DFS_AIG_Number << endl;
   for (auto it = source_list.begin(); it != source_list.end(); ++it)
   {
      outfile << ((*it)->_Id) * 2 << endl;
   }
   for (auto it = sink_list.begin(); it != sink_list.end(); ++it)
   {
      if ((*it)->get_inverted())
         outfile << ((*it)->_faninList[0]->_Id) * 2 + 1 << endl;
      else
         outfile << ((*it)->_faninList[0]->_Id) * 2 << endl;
   }
   for (unsigned i = 0; i < storage.size(); ++i)
   {
      if (i % 3 == 2)
      {
         outfile << storage[i] << endl;
      }
      else
      {
         outfile << storage[i] << ' ';
      }
   }
   for (auto it = source_list.begin(); it != source_list.end(); ++it)
   {
      if ((*it)->_name != "")
      {
         outfile << 'i' << Count_name << ' ' << ((*it)->_name) << endl;
         ++Count_name;
      }
   }
   Count_name = 0;
   for (auto it = sink_list.begin(); it != sink_list.end(); ++it)
   {
      if ((*it)->_name != "")
      {
         outfile << 'o' << Count_name << ' ' << ((*it)->_name) << endl;
         ++Count_name;
      }
   }
   outfile<<"c"<<endl;
   outfile<<"AAG output by Chung-Yang (Ric) Huang"<<endl;
}

inline void CirMgr::printGateInformation(CirGate *input_gate) const
{
   if (input_gate->type_name == PI_GATE)
   {
      cout << "[" << counter_netlist << "] PI  " << input_gate->_Id;
      if (!input_gate->_name.empty())
         cout << " (" << input_gate->_name << ")";
      cout << endl;
      ++counter_netlist;
   }
   else if (input_gate->type_name == PO_GATE)
   {
      cout << "[" << counter_netlist << "] PO  " << input_gate->_Id << " ";
      if (input_gate->get_inverted())
      {
         if (is_UNDEF_0)
            cout << "*!" << input_gate->_faninList[0]->_Id;
         else
            cout << "!" << input_gate->_faninList[0]->_Id;
      }
      else
      {
         if (is_UNDEF_0)
            cout << "*" << input_gate->_faninList[0]->_Id;
         else
            cout << input_gate->_faninList[0]->_Id;
      }
      if (!input_gate->_name.empty())
         cout << " (" << input_gate->_name << ")";
      cout << endl;
      ++counter_netlist;
      is_UNDEF_0 = is_UNDEF_1 = false;
   }
   else if (input_gate->type_name == AIG_GATE)
   {
      cout << "[" << counter_netlist << "] AIG " << input_gate->_Id << " ";
      if (input_gate->get_inverted_0())
      {
         if (is_UNDEF_0)
            cout << "*!" << input_gate->_faninList[0]->_Id << " ";
         else
            cout << "!" << input_gate->_faninList[0]->_Id << " ";
      }
      else
      {
         if (is_UNDEF_0)
            cout << "*" << input_gate->_faninList[0]->_Id << " ";
         else
            cout << input_gate->_faninList[0]->_Id << " ";
      }
      if (input_gate->get_inverted_1())
      {
         if (is_UNDEF_1)
            cout << "*!" << input_gate->_faninList[1]->_Id;
         else
            cout << "!" << input_gate->_faninList[1]->_Id;
      }
      else
      {
         if (is_UNDEF_1)
            cout << "*" << input_gate->_faninList[1]->_Id;
         else
            cout << input_gate->_faninList[1]->_Id;
      }
      cout << endl;
      ++counter_netlist;
      is_UNDEF_0 = is_UNDEF_1 = false;
   }
   else if (input_gate->type_name == CONST_GATE)
   {
      cout << "[" << counter_netlist << "]"
           << " CONST0" << endl;
      ++counter_netlist;
   }
   else
   {
   }
}
inline void CirMgr::printAIGGate(CirGate *input_gate, Array<unsigned> &storage) const
{
   if (input_gate->type_name == AIG_GATE)
   {
      storage.push_back((input_gate->_Id) * 2);
      if (input_gate->get_inverted_0())
      {
         storage.push_back((input_gate->_faninList[0]->_Id) * 2 + 1);
      }
      else
      {
         storage.push_back((input_gate->_faninList[0]->_Id) * 2);
      }
      if (input_gate->get_inverted_1())
      {
         storage.push_back((input_gate->_faninList[1]->_Id) * 2 + 1);
      }
      else
      {
         storage.push_back((input_gate->_faninList[1]->_Id) * 2);
      }

      ++Counting_DFS_AIG_Number;
   }
}
