/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>

#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
template <class T>
class Array
{
public:
  // TODO decide the initial value for _isSorted
  Array() : _data(0), _size(0), _capacity(0) {}
  ~Array() { delete[] _data; }

  // DO NOT add any more data member or function for class iterator
  class iterator
  {
    friend class Array;

  public:
    iterator(T *n = 0) : _node(n) {}
    iterator(const iterator &i) : _node(i._node) {}
    ~iterator() {} // Should NOT delete _node

    // TODO: implement these overloaded operators
    const T &operator*() const { return (*_node); }
    T &operator*() { return (*_node); }
    iterator &operator++()
    {
      _node++;
      return (*this);
    }
    iterator operator++(int)
    {
      iterator temp(_node);
      _node++;
      return temp;
    }
    iterator &operator--()
    {
      _node--;
      return (*this);
    }
    iterator operator--(int)
    {
      iterator temp(_node);
      _node--;
      return temp;
    }

    iterator operator+(int i) const //TODO:
    {
      return (_node + i);
    }
    iterator &operator+=(int i)
    {
      _node = _node + i;
      return (*this);
    }

    iterator &operator=(const iterator &i)
    {
      _node = i._node;
      return (*this);
    }
    bool operator!=(const iterator &i) const
    {
      if (_node == i._node)
      {
        return false;
      }
      return true;
    }
    bool operator==(const iterator &i) const
    {
      if (_node == i._node)
      {
        return true;
      }
      return false;
    }

  private:
    T *_node;
  };

  // TODO: implement these functions
  iterator begin() const { return _data; }
  iterator end() const { return (_data + _size); }
  bool empty() const
  {
    if (_size == 0)
    {
      return true;
    }
    return false;
  }
  size_t size() const { return _size; }

  T &operator[](size_t i) { return _data[i]; }
  const T &operator[](size_t i) const { return _data[i]; }

  void push_back(const T &x)
  {
    if (_capacity == 0)
    {
      _data = new T(x);
      _capacity = 1;
      _size++;
    }
    else if (_size + 1 > _capacity)
    {
      T *temp_store = new T[_capacity];
      for (int i = 0; i < _capacity; ++i)
      {
        temp_store[i] = _data[i];
      }
      if (_size == 1)
      {
        delete _data;
      }
      else
      {
        delete[] _data;
      }
      _capacity *= 2;
      _data = new T[_capacity];
      for (int i = 0; i < (_capacity / 2); ++i)
      {
        _data[i] = temp_store[i];
      }
      delete[] temp_store;
      _data[_size] = x;
      _size++;
    }
    else
    {
      _data[_size] = x;
      _size++;
    }
  }
  void pop_front()
  {
    if (_size == 0)
      return;
    // iterator temp(_data+_size-1);
    // iterator temp2(_data);
    // cout<<"temp = "<<*temp<<endl;
    // cout<<"temp2 = "<<*temp2<<endl;
    // temp2 = temp;
    // cout<<"temp = "<<*temp<<endl;
    // cout<<"temp2 = "<<*temp2<<endl;
    // cout<<"_data[0] = "<<_data[0]<<endl;
    _data[0] = _data[_size - 1];
    _size--;
  }
  void pop_back()
  {
    if (_size == 0)
      return;
    _size--;
  }

  bool erase(iterator pos)
  {
    if (_size == 0)
    {
      return false;
    }
    T *position = pos._node;
    *position = _data[_size - 1];
    _size--;
    return true;
  }
  bool erase(const T &x)
  {
    for (int i = 0; i < _size; i++)
    {
      if (_data[i] == x)
      {
        _data[i] = _data[_size - 1];
        _size--;
        return true;
      }
    }
    return false;
  }

  iterator find(const T &x)
  {
    for (int i = 0; i < _size; i++)
    {
      if (_data[i] == x)
      {
        iterator temp(_data + i);
        return temp;
      }
    }
    return end();
  }
  bool find_bool(const T &x)
  {
    for (int i = 0; i < _size; i++)
    {
      if (_data[i] == x)
      {
        return true;
      }
    }
    return false;
  }

  void clear()
  {
    if (_capacity == 1)
    {
      delete _data;
    }
    else if (_size != 0)
      delete[] _data;
    _size = _capacity = 0;
  }

  // [Optional TODO] Feel free to change, but DO NOT change ::sort()
  void sort() const
  {
    if (!empty())
      ::sort(_data, _data + _size);
  }

  // Nice to have, but not required in this homework...
  // void reserve(size_t n) { ... }
  // void resize(size_t n) { ... }

private:
  // [NOTE] DO NOT ADD or REMOVE any data member
  T *_data;
  size_t _size;           // number of valid elements
  size_t _capacity;       // max number of elements
  mutable bool _isSorted; // (optionally) to indicate the array is sorted

  // [OPTIONAL TODO] Helper functions; called by public member functions
};
class CirGate
{
  friend class CirMgr;

public:
  CirGate(GateType t = GateType::UNDEF_GATE, string the_name = "", unsigned id = 0, unsigned lineno = 0) : type_name(t), _name(the_name), _Id(id), _lineNo(lineno) {}
  virtual ~CirGate() {}

  // Basic access methods
  string getTypeStr() const
  {
    switch (type_name)
    {
    case 0:
      return "UNDEF-GATE";
    case 1:
      return "PI-GATE";
    case 2:
      return "PO-GATE";
    case 3:
      return "AIG-GATE";
    case 4:
      return "CONST-GATE";
    }
  }
  unsigned getLineNo() const { return _lineNo; }

  // Printing functions
  virtual void printGate() const {};
  virtual bool get_inverted() const = 0;
  virtual bool get_inverted_0() const = 0;
  virtual bool get_inverted_1() const = 0;
  void reportGate() const;
  void reportFanin(int level)  ;
  void reportFanout(int level) ;
  void reporting(int level, CirGate *cur_gate, bool fanin_direction) ;
  void printInformation(CirGate *, int) const;
  int CountingNumDigits(unsigned number) const
  {
    int digits = 0;
    while (number)
    {
      number /= 10;
      digits++;
    }
    return digits;
  }

  GateList _faninList;
  GateList _fanoutList;

  unsigned _lineNo;
  unsigned _Id;
  string _name;
  GateType type_name;

  static unsigned global_ref;
  mutable unsigned _ref;

  static void setGlobalRef()
  {
    ++global_ref;
  }
  void setToglobalref()
  {
    _ref = global_ref;
  }
  bool isGlobalref()
  {
    return (_ref == global_ref);
  }
  static int _depth;
  mutable Array<unsigned> _need_inverted;
  static bool compare_for_sort(CirGate* a, CirGate* b)
  {
    return a->_Id < b->_Id;
  }

private:
protected:
};

class PI_Gate : public CirGate
{
public:
  PI_Gate(unsigned input_id, unsigned input_lineno)
  {
    // this=CirGate(PI_GATE, "", input_id, input_lineno);
    _Id = input_id;
    _lineNo = input_lineno;
    type_name = GateType::PI_GATE;
    _ref = 0;
  }
  void printGate() const
  {
    cout << "==================================================" << endl;
    cout << "= PI(" << _Id << ")";
    if (_name != "")
    {
      cout << "\"" << _name << "\"";
      cout << ", line " << _lineNo << setw(35 - CountingNumDigits(_Id) - CountingNumDigits(_lineNo) - _name.size()) << "=" << endl;
    }
    else
      cout << ", line " << _lineNo << setw(37 - CountingNumDigits(_Id) - CountingNumDigits(_lineNo)) << "=" << endl;
    cout << "==================================================" << endl;
  }
  bool get_inverted() const
  {
    return false;
  }
  bool get_inverted_0() const { return false; }
  bool get_inverted_1() const { return false; }
};
class PO_Gate : public CirGate
{
public:
  PO_Gate(unsigned input_id, unsigned input_lineno, bool inv)
  {
    // this=CirGate(PI_GATE, "", input_id, input_lineno);
    _Id = input_id;
    _lineNo = input_lineno;
    type_name = GateType::PO_GATE;
    _inverted = inv;
    _ref = 0;
  }
  bool _inverted;
  bool get_inverted() const
  {
    return _inverted;
  }
  void printGate() const
  {
    cout << "==================================================" << endl;
    cout << "= PO(" << _Id << ")";
    if (_name != "")
    {
      cout << "\"" << _name << "\"";
      cout << ", line " << _lineNo << setw(35 - CountingNumDigits(_Id) - CountingNumDigits(_lineNo) - _name.size()) << "=" << endl;
    }
    else
      cout << ", line " << _lineNo << setw(37 - CountingNumDigits(_Id) - CountingNumDigits(_lineNo)) << "=" << endl;
    cout << "==================================================" << endl;
  }
  bool get_inverted_0() const { return false; }
  bool get_inverted_1() const { return false; }
};
class AIG_Gate : public CirGate
{
public:
  AIG_Gate(unsigned gate_id, unsigned lineno, bool inv1, bool inv2)
  {
    _Id = gate_id;
    _lineNo = lineno;
    type_name = GateType::AIG_GATE;
    _inverted[0] = inv1;
    _inverted[1] = inv2;
    _ref = 0;
  }
  bool _inverted[2];
  void printGate() const
  {
    cout << "==================================================" << endl;
    cout << "= AIG(" << _Id << "), line " << _lineNo << setw(36 - CountingNumDigits(_Id) - CountingNumDigits(_lineNo)) << "=" << endl;
    cout << "==================================================" << endl;
  }
  bool get_inverted() const
  {
    return false;
  }
  bool get_inverted_0() const { return _inverted[0]; }
  bool get_inverted_1() const { return _inverted[1]; }
};
class CONST0_Gate : public CirGate
{
public:
  CONST0_Gate()
  {
    _Id = 0;
    _lineNo = 0;
    type_name = GateType::CONST_GATE;
    _ref = 0;
  }
  void printGate() const
  {
    cout << "==================================================" << endl;
    cout << "= CONST(0), line 0                               =" << endl;
    cout << "==================================================" << endl;
  }
  bool get_inverted() const
  {
    return false;
  }
  bool get_inverted_0() const { return false; }
  bool get_inverted_1() const { return false; }
};
class UNDEF_Gate : public CirGate
{
public:
  UNDEF_Gate(unsigned input_id)
  {
    // this=CirGate(PI_GATE, "", input_id, input_lineno);
    _Id = input_id;
    _lineNo = 0;
    type_name = GateType::UNDEF_GATE;
    _ref = 0;
  }
  void printGate() const
  {
    cout << "==================================================" << endl;
    cout << "= UNDEF(" << _Id << "), line 0" << setw(34 - CountingNumDigits(_Id) - 1) << "=" << endl;
    cout << "==================================================" << endl;
  }
  bool get_inverted() const
  {
    return false;
  }
  bool get_inverted_0() const { return false; }
  bool get_inverted_1() const { return false; }
};

#endif // CIR_GATE_H
