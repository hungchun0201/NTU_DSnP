/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T>
class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
  friend class DList<T>;
  friend class DList<T>::iterator;

  DListNode(const T &d, DListNode<T> *p = 0, DListNode<T> *n = 0) : _data(d), _prev(p), _next(n) {}

  // [NOTE] DO NOT ADD or REMOVE any data member
  T _data;
  DListNode<T> *_prev;
  DListNode<T> *_next;
};

template <class T>
class DList
{
public:
  // TODO decide the initial value for _isSorted
  DList()
  {
    _head = new DListNode<T>(T());
    _head->_prev = _head->_next = _head; // _head is a dummy node!IMPORTANT!
  }
  ~DList()
  {
    clear();
    delete _head;
  }

  // DO NOT add any more data member or function for class iterator
  class iterator
  {
    friend class DList;

  public:
    iterator(DListNode<T> *n = 0) : _node(n) {}
    iterator(const iterator &i) : _node(i._node) {}
    ~iterator() {} // Should NOT delete _node

    // TODO implement these overloaded operators
    const T &operator*() const { return _node->_data; }
    T &operator*() { return _node->_data; }
    iterator &operator++()
    {
      _node = _node->_next;
      return *(this);
    }
    iterator operator++(int)
    {
      iterator temp(_node);
      _node = _node->_next;
      return temp;
    }
    iterator operator+(int i) const //TODO
    {
      iterator temp(_node);
      for (int x = 0; x < i; x++)
      {
        ++temp;
      }
      return temp;
    }
    iterator operator-(int i) const //TODO
    {
      iterator temp(_node);
      for (int x = 0; x < i; x++)
      {
        --temp;
      }
      return temp;
    }
    iterator &operator--()
    {
      _node = _node->_prev;
      return *(this);
    }
    iterator operator--(int)
    {
      iterator temp(_node);
      _node = _node->_prev;
      return temp;
    }

    iterator &operator=(const iterator &i)
    {
      _node = i._node;
      return *(this);
    }

    bool operator!=(const iterator &i) const
    {
      if (_node == i._node)
      {
        return false;
      }
      else
      {
        return true;
      }
    }
    bool operator==(const iterator &i) const
    {
      if (_node == i._node)
      {
        return true;
      }
      else
      {
        return false;
      }
    }

  private:
    DListNode<T> *_node;
  };

  // TODO: implement these functions
  iterator begin() const { return _head->_next; }
  iterator end() const { return _head; }
  bool empty() const
  {
    if (_head->_next == _head)
    {
      return true;
    }
    return false;
  }
  size_t size() const
  {
    size_t counter = 0;
    DListNode<T> *tempNode = _head;
    while (tempNode->_next != _head)
    {
      counter++;
      tempNode = tempNode->_next;
    }
    return counter;
  }

  void push_back(const T &x)
  {
    DListNode<T> *storage = new DListNode<T>(x, _head->_prev, _head);
    _head->_prev->_next = storage;
    _head->_prev = storage;
  }
  void pop_front()
  {
    if (empty())
    {
      return;
    }
    _head->_next->_next->_prev = _head;
    DListNode<T> *temp = _head->_next->_next;
    delete _head->_next;
    _head->_next = temp;
  }
  void pop_back()
  {
    if (empty())
    {
      return;
    }
    _head->_prev->_prev->_next = _head;
    DListNode<T> *temp = _head->_prev->_prev;
    delete _head->_prev;
    _head->_prev = temp;
  }

  // return false if nothing to erase
  bool erase(iterator pos)
  {
    if (pos == _head)
    {
      return false;
    }
    pos._node->_prev->_next = pos._node->_next;
    pos._node->_next->_prev = pos._node->_prev;
    delete pos._node;
    return true;
  }
  bool erase(const T &x)
  {
    for (iterator temp = begin(); temp != end(); temp++)
    {
      if (temp._node->_data == x)
      {
        erase(temp);
        return true;
      }
    }
    return false;
  }

  iterator find(const T &x)
  {
    iterator temp = begin();
    for (; temp != end(); temp++)
    {
      if (temp._node->_data == x)
      {
        return temp;
      }
    }
    return end();
  }

  void clear()
  {
    for (iterator temp = begin(); temp != end(); temp++)
    {
      delete temp._node;
    }
    _head->_prev = _head->_next = _head;
  } // delete all nodes except for the dummy node

  void sort() //quick sort
  {
    if (empty())
    {
      return;
    }
    quick_sort(begin(), end() - 1);
  }
  void swap(T *data1, T *data2)
  {
    T temp = *data1;
    *data1 = *data2;
    *data2 = temp;
  }
  iterator partition(iterator it1, iterator pivot)
  {
    T pivot_value = *pivot;
    iterator i = it1 - 1;
    for (iterator j = it1; j != pivot; ++j)
    {
      if (*j <= pivot_value)
      {
        i++;
        swap(&(*i), &(*j));
      }
    }
    i++;
    swap(&(*i), &(*pivot));
    return i;
  }
  void quick_sort(iterator first, iterator _end)
  {
    if (_end != end() && first != _end && first != _end + 1)
    {
      iterator p = partition(first, _end);
      quick_sort(first, p - 1);
      quick_sort(p + 1, _end);
    }
  }
  void printdlist()
  {
    cout << "printing DList...." << endl;
    for (iterator it1 = begin(); it1 != end(); ++it1)
    {
      cout << *it1 << endl;
    }
  }

private:
  // [NOTE] DO NOT ADD or REMOVE any data member
  DListNode<T> *_head;    // = dummy node if list is empty
  mutable bool _isSorted; // (optionally) to indicate the array is sorted

  // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
