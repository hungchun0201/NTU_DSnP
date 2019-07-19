/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest; // defined in memTest.cpp

bool initMemCmd()
{
  if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
        cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
        cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
        cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)))
  {
    cerr << "Registering \"mem\" commands fails... exiting" << endl;
    return false;
  }
  return true;
}

//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string &option)
{
  // check option
  string token;
  if (!CmdExec::lexSingleOption(option, token))
    return CMD_EXEC_ERROR;
  if (token.size())
  {
    int b;
    if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj))))
    {
      cerr << "Illegal block size (" << token << ")!!" << endl;
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
    }
#ifdef MEM_MGR_H
    mtest.reset(toSizeT(b));
#else
    mtest.reset();
#endif // MEM_MGR_H
  }
  else
    mtest.reset();
  return CMD_EXEC_DONE;
}

void MTResetCmd::usage(ostream &os) const
{
  os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void MTResetCmd::help() const
{
  cout << setw(15) << left << "MTReset: "
       << "(memory test) reset memory manager" << endl;
}

//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string &option)
{
  // TODO:
  // MemMgr(65536);
  // MemMgr<MemTest>mem_mgr(65536);
  // mem_mgr;

  vector<string> options;
  if (!CmdExec::lexOptions(option, options))
    return CMD_EXEC_ERROR;
  if (options.empty())
    return CmdExec::errorOption(CMD_OPT_MISSING, "");
  int testing_num = 0;
  size_t index_of_a = 0;
  int value_after_a = 0;
  bool is_array = false;
  int times = 0;
  for (size_t i = 0, n = options.size(); i < n; ++i) //we can also initialize two variables in for loop!!
  {
    testing_num = 0;
    if (myStrNCmp("-Array", options[i], 2) == 0)
    {
      if (i == n - 1)
      {
        cout << "Missing option after (" << options[i] << ")!!" << endl;
        return CMD_EXEC_ERROR;
      }
      else
      {
        index_of_a = i;
        if (!myStr2Int(options[i + 1], testing_num))
        {
          return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i + 1]);
        }
        else if (myStr2Int(options[i + 1], testing_num) && testing_num <= 0)
        {
          return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i + 1]);
        }
        value_after_a = testing_num;
        is_array = true;
      }
    }
    else if (!myStr2Int(options[i], testing_num))
    {
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
    }
    else if (myStr2Int(options[i], testing_num) && testing_num <= 0)
    {
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
    }
  }
  if (!is_array)
  {
    if (options.size() == 1)
    {
      mtest.newObjs((size_t)testing_num);
      return CMD_EXEC_DONE;
    }
    else
    {
      return CmdExec::errorOption(CMD_OPT_EXTRA, options[2]);
    }
  }
  else
  {
    if (options.size() == 2)
    {
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
    }
    else if (options.size() == 3)
    {
      try
      {
        if (index_of_a == 0)
        {
          assert(myStr2Int(options[2], times));
          mtest.newArrs(times, value_after_a);
          return CMD_EXEC_DONE;
        }
        else
        {
          assert(myStr2Int(options[0], times));
          mtest.newArrs(times, value_after_a);
          return CMD_EXEC_DONE;
        }
      }
      catch (bad_alloc)
      {
        return CMD_EXEC_ERROR;
      }
    }
    else
    {
      if (index_of_a <= 1)
      {
        return CmdExec::errorOption(CMD_OPT_EXTRA, options[3]);
      }
      else
      {
        return CmdExec::errorOption(CMD_OPT_EXTRA, options[1]);
      }
    }
  }
  // Use try-catch to catch the bad_alloc exception
}

void MTNewCmd::usage(ostream &os) const
{
  os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void MTNewCmd::help() const
{
  cout << setw(15) << left << "MTNew: "
       << "(memory test) new objects" << endl;
}

//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string &option)
{
  // TODO:
  vector<string> options;
  if (!CmdExec::lexOptions(option, options))
    return CMD_EXEC_ERROR;
  if (options.empty())
    return CmdExec::errorOption(CMD_OPT_MISSING, "");
  int testing_num = 0;
  size_t index_of_i = 0;
  int value_after_i = 0;
  size_t index_of_r = 0;
  int value_after_r = 0;
  size_t index_of_a = 0;
  bool has_set_i = false;
  bool has_set_r = false;
  bool has_set_a = false;
  int times = 0;
  for (size_t i = 0, n = options.size(); i < n; ++i) //we can also initialize two variables in for loop!!
  {
    testing_num = 0;
    if (myStrNCmp("-Index", options[i], 2) == 0)
    {
      if (has_set_i == true || has_set_r == true)
      {
        return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
      }
      if (i == n - 1)
      {
        cout << "Missing option after (" << options[i] << ")!!" << endl;
        return CMD_EXEC_ERROR;
      }
      else
      {
        index_of_i = i;
        if (!myStr2Int(options[i + 1], testing_num))
        {
          return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i + 1]);
        }
        else if (myStr2Int(options[i + 1], testing_num) && testing_num < 0)
        {
          return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i + 1]);
        }
        value_after_i = testing_num;
        // if (myStr2Int(options[i + 1], testing_num) && mtest.getObjListSize() <= testing_num)
        // {
        //   cerr << "Size of object list (" << mtest.getObjListSize() << ") is <= " << value_after_i << "!!" << endl;
        //   return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[index_of_r + 1]);
        // }
        has_set_i = true;
        i += 1;
        continue;
      }
    }
    else if (myStrNCmp("-Random", options[i], 2) == 0)
    {
      if (has_set_i == true || has_set_r == true)
      {
        return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
      }
      if (i == n - 1)
      {
        cout << "Missing option after (" << options[i] << ")!!" << endl;
        return CMD_EXEC_ERROR;
      }
      else
      {
        index_of_r = i;
        if (!myStr2Int(options[i + 1], testing_num))
        {
          return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i + 1]);
        }
        else if (myStr2Int(options[i + 1], testing_num) && testing_num <= 0)
        {
          return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i + 1]);
        }
        value_after_r = testing_num;
        has_set_r = true;
        i += 1;
        continue;
      }
    }
    else if (myStrNCmp("-Array", options[i], 2) == 0)
    {
      if (has_set_a == true)
      {
        return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
      }
      index_of_a = i;
      has_set_a = true;
    }
    else if (!myStr2Int(options[i], testing_num))
    {
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
    }
    else if (myStr2Int(options[i], testing_num) && testing_num <= 0)
    {
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
    }
    else
    {
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
    }
  }
  if (!has_set_a)
  {
    if (options.size() != 2)
    {
      if (options.size() >= 3)
      {
        return CmdExec::errorOption(CMD_OPT_EXTRA, options[3]);
      }
    }
    if (has_set_i)
    {
      if (mtest.getObjListSize() <= value_after_i)
      {
        cerr << "Size of object list (" << mtest.getObjListSize() << ") is <= " << value_after_i << "!!" << endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[index_of_i + 1]);
      }
      mtest.deleteObj(value_after_i);
      return CMD_EXEC_DONE;
    }
    else if (has_set_r)
    {
      if (mtest.getObjListSize() == 0)
      {
         cerr <<"Size of object list is 0!!"<< endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[index_of_r]);
      }
      for (size_t i = 0; i < value_after_r; i++)
      {
        mtest.deleteObj(rnGen(mtest.getObjListSize()));
      }
      return CMD_EXEC_DONE;
    }
    else
    {
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
    }
  }
  else
  {
    if (options.size() >= 4)
    {
      if (index_of_i <= 1)
      {
        if (index_of_a == 0)
        {
          return CmdExec::errorOption(CMD_OPT_EXTRA, options[3]);
        }
        else if (index_of_a == 2)
        {
          return CmdExec::errorOption(CMD_OPT_EXTRA, options[3]);
        }
        else if (index_of_a >= 3)
        {
          if (index_of_i == 0)
            return CmdExec::errorOption(CMD_OPT_EXTRA, options[2]);
          else if (index_of_i == 1)
            return CmdExec::errorOption(CMD_OPT_EXTRA, options[0]);
        }
      }
      else
      {
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
      }
    }
    if (has_set_i)
    {
      if (mtest.getArrListSize() <= value_after_i)
      {
        cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " << value_after_i << "!!" << endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[index_of_i + 1]);
      }
      mtest.deleteArr(value_after_i);
      return CMD_EXEC_DONE;
    }
    else if (has_set_r)
    {
      if (mtest.getArrListSize() == 0)
      {
        cerr <<"Size of array list is 0!!"<< endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[index_of_r]);
      }
      for (size_t i = 0; i < value_after_r; i++)
      {
        mtest.deleteArr(rnGen(mtest.getArrListSize()));
      }
      return CMD_EXEC_DONE;
    }
    else
    {
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
    }
  }
}

void MTDeleteCmd::usage(ostream &os) const
{
  os << "Usage: MTDelete <-Index (size_t objId) | "
     << "-Random (size_t numRandId)> [-Array]" << endl;
}

void MTDeleteCmd::help() const
{
  cout << setw(15) << left << "MTDelete: "
       << "(memory test) delete objects" << endl;
}

//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string &option)
{
  // check option
  if (option.size())
    return CmdExec::errorOption(CMD_OPT_EXTRA, option);
  mtest.print();

  return CMD_EXEC_DONE;
}

void MTPrintCmd::usage(ostream &os) const
{
  os << "Usage: MTPrint" << endl;
}

void MTPrintCmd::help() const
{
  cout << setw(15) << left << "MTPrint: "
       << "(memory test) print memory manager info" << endl;
}
