/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
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

  void clear()
  {
    if (_capacity == 1)
    {
      delete _data;
    }
    else if(_size!=0)
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

#endif // ARRAY_H
