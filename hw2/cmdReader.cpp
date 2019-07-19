/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream &);
ParseChar getChar(istream &);

//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void CmdParser::readCmd()
{
  if (_dofile.is_open())
  {
    readCmdInt(_dofile);
    _dofile.close();
  }
  else
    readCmdInt(cin);
}

void CmdParser::readCmdInt(istream &istr)
{
  resetBufAndPrintPrompt();

  while (1)
  {
    ParseChar pch = getChar(istr);
    if (pch == INPUT_END_KEY)
      break;
    switch (pch)
    {
    case LINE_BEGIN_KEY:
    case HOME_KEY:
      moveBufPtr(_readBuf);
      break;
    case LINE_END_KEY:
    case END_KEY:
      moveBufPtr(_readBufEnd);
      break;
    case BACK_SPACE_KEY:
      moveBufPtr(_readBufPtr - 1);
      deleteChar();
      break;
    case DELETE_KEY:
      deleteChar();
      break;
    case NEWLINE_KEY:
      addHistory();
      cout << char(NEWLINE_KEY);
      resetBufAndPrintPrompt();
      break;
    case ARROW_UP_KEY:
      moveToHistory(_historyIdx - 1);
      break;
    case ARROW_DOWN_KEY:
      moveToHistory(_historyIdx + 1);
      break;
    case ARROW_RIGHT_KEY:
      moveBufPtr(_readBufPtr + 1);
      break;
    case ARROW_LEFT_KEY:
      moveBufPtr(_readBufPtr - 1);
      break;
    case PG_UP_KEY:
      moveToHistory(_historyIdx - PG_OFFSET);
      break;
    case PG_DOWN_KEY:
      moveToHistory(_historyIdx + PG_OFFSET);
      break;
    case TAB_KEY:
    {
      insertChar(' ', TAB_POSITION);
    //   printf("%p\n", _readBuf);
    // printf("%p\n", _readBufPtr);
      int temp = _readBufPtr-_readBuf;
    //   cout<<temp<<endl;
      if((temp)%8!=0)
      {
        for(int i=0;i<(temp%8);i++)
        {
          moveBufPtr(_readBufPtr - 1);
          deleteChar();                     
        }
      }
      break;
    }
    case INSERT_KEY: // not yet supported; fall through to UNDEFINE
    case UNDEFINED_KEY:
      mybeep();
      break;
    default: // printable character
      insertChar(char(pch));
      break;
    }
#ifdef TA_KB_SETTING
    taTestOnly();
#endif
  }
}

// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool CmdParser::moveBufPtr(char *const ptr)
{
  char *tempcomparison = _readBuf;
  // cout << "ptr     ";
  //     printf("%p\n",ptr);
  // cout << "_readBufPtr     ";
  //     printf("%p\n", _readBufPtr);
  //     cout << "_readBufEnd     ";
  //     printf("%p\n", _readBufEnd);
  //     cout << "_readBuf     ";
  //     printf("%p\n", tempcomparison);
  if (ptr < tempcomparison || ptr > _readBufEnd)
  {
    mybeep();
    return false;
  }

  else
  {
    if (ptr < _readBufPtr)
      while (ptr < _readBufPtr)
      {
        cout << '\b';
        _readBufPtr--;
      }
    else
      while (ptr > _readBufPtr)
      {
        cout << *_readBufPtr;
        _readBufPtr++;
      }
  }
  return true;
}

// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool CmdParser::deleteChar()
{
  // TODO:...
  if (_readBufPtr == _readBufEnd)
  {
    mybeep();
    return false;
  }
  int actual_legnth = 0;
  char *_counter = _readBufEnd - 1;
  while (_counter > _readBufPtr)
  {
    _counter--;
    actual_legnth++;
  }
  memmove(_counter, _readBufPtr + 1, actual_legnth);
  *(_readBufEnd - 1) = '\0';
  _readBufEnd--;
  for (int i = 0; i < actual_legnth; i++)
  {
    cout << *_counter;
    _counter++;
  }
  cout << ' ';
  for (int i = 0; i <= actual_legnth; i++)
  {
    cout << '\b';
  }
  return true;
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
//
void CmdParser::insertChar(char ch, int repeat)
{
  assert(repeat >= 1);
  int actual_legnth = 0;
  char *_counter = _readBufEnd - 1;
  while (_counter >= _readBufPtr)
  {
    _counter--;
    actual_legnth++;
  }
  _counter++;
  _counter += repeat;
  memmove(_counter, _readBufPtr, actual_legnth);
  int temp_counter = 0;
  while (temp_counter != repeat)
  {
    *_readBufPtr = ch;
    cout << ch;
    (_readBufPtr++);
    _readBufEnd++;
    temp_counter++;
  }
  for (int i = 0; i < actual_legnth; i++)
  {
    cout << *_counter;
    _counter++;
  }
  for (int i = 0; i < actual_legnth; i++)
  {
    cout << '\b';
  }
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void CmdParser::deleteLine()
{

  moveBufPtr(_readBufEnd);
     for (char* temp = _readBufEnd; temp > _readBuf; temp--) {
      cout << ' ';
      cout<<'\b';
      cout<<'\b';
      _readBufPtr--;
   }
   cout<<' ';
   cout<<'\b';
  // while (_readBufEnd >= _readBuf)
  // {
  //   cout << ' ';
  //   if(_readBufEnd=_readBuf)
  //   {
      
  //   }
  //   cout << '\b';
  //   _readBufEnd--;
  // }
  _readBufEnd=_readBuf;
  moveBufPtr(_readBuf);
  *_readBufEnd = 0;
}

// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void CmdParser::moveToHistory(int index)
{
  if(index==_historyIdx)
  {
    return;
  }
  if (index < _historyIdx)
  {
    if (_historyIdx == 0)
    {
      mybeep();
      return;
    }
    if(index<0)
    {
      index=0;
    }
    if (_historyIdx == _history.size())
    {
      *_readBufEnd=0;
      _history.push_back(_readBuf);
      _tempCmdStored = true;
    }
    _historyIdx = index;
    retrieveHistory();
  }
    
  else
  {
    if(_historyIdx == _history.size())
    {
      mybeep();
      return;
    }
    if(index>=_history.size())
    {
      index = _history.size()-1;
    }
    if(_tempCmdStored==true&&(index==_history.size()-1))
    {
      _history.pop_back();
      _tempCmdStored=0;
    }
    _historyIdx = index;
    retrieveHistory();

  }
}

// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void CmdParser::addHistory()
{
  string wanna_add = _readBuf;
  size_t before_space = wanna_add.find_first_not_of(' ');
  // cout<<"OH    "<<before_space<<endl;
  size_t last_space = wanna_add.find_last_not_of(' ');
  // cout<<"last    "<<last_space;
  wanna_add.erase(last_space + 1, (wanna_add.size() - last_space + 1));
  wanna_add.erase(0, before_space);
  if (_tempCmdStored)
  {
    _history.pop_back();
    _tempCmdStored = false;
  }
  if (wanna_add.size() != 0)
  {
    _history.push_back(wanna_add);
  }
  _historyIdx = _history.size();
  // clean the _readBuf
  for(int i = 0;_readBuf[i]!='\0';i++)
  {
    _readBuf[i]='\0';
  }

  // moveBufPtr(_readBufEnd);
  // while(_readBufPtr>_readBuf)
  // {
  //     moveBufPtr(_readBufPtr - 1);
  //     deleteChar();
  // }
}

// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void CmdParser::retrieveHistory()
{
  deleteLine();
  strcpy(_readBuf, _history[_historyIdx].c_str());
  cout << _readBuf;
  _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}
