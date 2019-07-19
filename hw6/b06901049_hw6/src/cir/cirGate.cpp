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

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/

unsigned CirGate::global_ref = 0;
int CirGate::_depth = 0;

void CirGate::reportGate() const
{
   printGate();
}

void CirGate::reportFanin(int level) 
{
   assert(level >= 0);
   // _need_inverted --;
   // _has_printed = 0;
   setGlobalRef();
   if (level > 0)
   {
      if (get_inverted())
         _faninList[0]->_need_inverted.push_back(_depth+1);
      if (get_inverted_0())
          _faninList[0]->_need_inverted.push_back(_depth+1);
      if (get_inverted_1())
          _faninList[1]->_need_inverted.push_back(_depth+1);
   }

   // ++_has_printed;
   if (type_name == UNDEF_GATE)
   {
      cout << "UNDEF " << _Id;
   }
   else if (type_name == PI_GATE)
   {
      cout << "PI " << _Id;
   }
   else if (type_name == PO_GATE)
   {
      cout << "PO " << _Id;
   }
   else if (type_name == AIG_GATE)
   {
      cout << "AIG " << _Id;
   }
   else if (type_name == CONST_GATE)
   {
      cout << "CONST " << _Id;
   }
   cout << endl;
   if (level > 0)
   {
      for (auto it = _faninList.begin(); it != _faninList.end(); ++it)
      {
         reporting(level, (*it), true);
         --_depth;
      }
   }

   _depth = 0;
   _ref = global_ref;
}

void CirGate::reportFanout(int level)
{
   assert(level >= 0);
   setGlobalRef();
   if (level > 0)
   {
      for (auto it = _fanoutList.begin(); it != _fanoutList.end(); ++it)
      {
         if ((*it)->type_name == PO_GATE)
         {
            if ((*it)->get_inverted())
            {
               (*it)->_need_inverted.push_back(_depth+1);
            }
         }
         else if ((*it)->type_name == AIG_GATE)
         {
            if ((*it)->_faninList[0]->_Id == _Id)
            {
               if ((*it)->get_inverted_0())
               {
                  (*it)->_need_inverted.push_back(_depth+1);
               }
            }
            else
            {
               if ((*it)->get_inverted_1())
               {
                  (*it)->_need_inverted.push_back(_depth+1);
               }
            }
         }
      }
   }

   if (type_name == UNDEF_GATE)
   {
      cout << "UNDEF " << _Id;
   }
   else if (type_name == PI_GATE)
   {
      cout << "PI " << _Id;
   }
   else if (type_name == PO_GATE)
   {
      cout << "PO " << _Id;
   }
   else if (type_name == AIG_GATE)
   {
      cout << "AIG " << _Id;
   }
   else if (type_name == CONST_GATE)
   {
      cout << "CONST " << _Id;
   }
   cout << endl;
   if (level > 0)
   {
      sort(_fanoutList.begin(),_fanoutList.end(),compare_for_sort);
      for (auto it = _fanoutList.begin(); it != _fanoutList.end(); ++it)
      {
         reporting(level, (*it), false);
         --_depth;
      }
   }

   _depth = 0;
   _ref = global_ref;
}

void CirGate::reporting(int level, CirGate *cur_gate, bool fanin_direction) 
{
   ++_depth;
   for (int i = 0; i < _depth; ++i)
   {
      cout << "  ";
   }
   printInformation(cur_gate,level);
   if (_depth < level && cur_gate->_ref != global_ref)
   {

      if (fanin_direction)
      {
         if (cur_gate->get_inverted())
            cur_gate->_faninList[0]->_need_inverted.push_back(_depth+1);
         if (cur_gate->get_inverted_0())
            cur_gate->_faninList[0]->_need_inverted.push_back(_depth+1);
         if (cur_gate->get_inverted_1())
            cur_gate->_faninList[1]->_need_inverted.push_back(_depth+1);
         for (auto it = cur_gate->_faninList.begin(); it != cur_gate->_faninList.end(); ++it)
         {
            reporting(level, (*it), fanin_direction);
            --_depth;
         }
         if (!cur_gate->_faninList.empty())
            cur_gate->_ref = global_ref;
      }
      else
      {
         for (auto it = cur_gate->_fanoutList.begin(); it != cur_gate->_fanoutList.end(); ++it)
         {
            if ((*it)->type_name == PO_GATE)
            {
               if ((*it)->get_inverted())
               {
                  (*it)->_need_inverted.push_back(_depth+1);
               }
            }
            else if ((*it)->type_name == AIG_GATE)
            {
               if (cur_gate == (*it)->_faninList[0])
               {
                  if ((*it)->get_inverted_0())
                  {
                     (*it)->_need_inverted.push_back(_depth+1);
                  }
               }
               else
               {
                  if ((*it)->get_inverted_1())
                  {
                     (*it)->_need_inverted.push_back(_depth+1);
                  }
               }
            }
         }
         // GateList temp = cur_gate->_fanoutList;
         // sort(temp.begin(),temp.end(),compare_for_sort);
         // for (auto it = temp.begin(); it != temp.end(); ++it)
         // {
         //    reporting(level, (*it), fanin_direction);
         //    --_depth;
         // }
         sort(cur_gate->_fanoutList.begin(),cur_gate->_fanoutList.end(),compare_for_sort);
         for (auto it = cur_gate->_fanoutList.begin(); it != cur_gate->_fanoutList.end(); ++it)
         {
            reporting(level, (*it), fanin_direction);
            --_depth;
         }
         if (!cur_gate->_fanoutList.empty())
            cur_gate->_ref = global_ref;
      }
   }
   else
   {
      return;
   }
   return;
}
void CirGate::printInformation(CirGate *cur_gate,int level) const
{

   if (cur_gate->_need_inverted.find_bool(_depth))
   {
      cout << "!";
      cur_gate->_need_inverted.erase(_depth);
   }

   if (cur_gate->type_name == UNDEF_GATE)
   {
      cout << "UNDEF " << cur_gate->_Id;
   }
   else if (cur_gate->type_name == PI_GATE)
   {
      cout << "PI " << cur_gate->_Id;
   }
   else if (cur_gate->type_name == PO_GATE)
   {
      cout << "PO " << cur_gate->_Id;
   }
   else if (cur_gate->type_name == AIG_GATE)
   {
      cout << "AIG " << cur_gate->_Id;
   }
   else if (cur_gate->type_name == CONST_GATE)
   {
      cout << "CONST " << cur_gate->_Id;
   }
   if (cur_gate->_ref == global_ref&&_depth!=level)
   {
      cout << " (*)";
   }

   cout << endl;
}
