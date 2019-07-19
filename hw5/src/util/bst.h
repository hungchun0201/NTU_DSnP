/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T>
class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
  // TODO design your own class!!
  friend class BSTree<T>;
  friend class BSTree<T>::iterator;
  BSTreeNode(const T &d, BSTreeNode<T> *l = 0, BSTreeNode<T> *r = 0) : _data(d), _left(l), _right(r) {}

  // [NOTE] DO NOT ADD or REMOVE any data member
  T _data;
  BSTreeNode<T> *_left;
  BSTreeNode<T> *_right;
};

template <class T>
class BSTree
{
public:
  // TODO design your own class!!
  BSTree() : _root(0)
  {
  }
  ~BSTree()
  {
    clear();
    delete _root;
  }
  class iterator
  {
    friend class BSTree;

  public:
    iterator(BSTreeNode<T> *n = 0, BSTreeNode<T> *n2 = 0) : _node(n), _root_in_iter(n2) {}
    iterator(const iterator &i) : _node(i._node), _root_in_iter(i._root_in_iter) {}
    ~iterator() {} // Should NOT delete _node

    // TODO implement these overloaded operators
    const T &operator*() const { return _node->_data; }
    T &operator*() { return _node->_data; }
    iterator &operator++()
    {
      // cout<<"_node's value"<<_node->_data<<endl;
      if (_node->_right != NULL)
      {
        BSTreeNode<T> *finding_min = _node->_right;
        while (finding_min->_left != NULL)
        {
          finding_min = finding_min->_left;
        }
        _node = finding_min;
        // iterator the_min(finding_min);
        // return the_min;
        return *(this);
      }
      else
      {
        BSTreeNode<T> *successor = NULL;
        BSTreeNode<T> *_root_temp = _root_in_iter;

        while (_root_temp != NULL)
        {
          if (_node->_data < _root_temp->_data)
          {
            successor = _root_temp;
            _root_temp = _root_temp->_left;
          }
          else if (_node->_data >= _root_temp->_data)
          {
            if (_node == _root_temp)
            {
              break;
            }
            _root_temp = _root_temp->_right;
          }
        }
        _node = successor;
        // iterator succ(successor, _root_in_iter);
        // return succ;
        return *(this);
      }
    }
    iterator operator++(int)
    {
      iterator temp(_node, _root_in_iter);
      if (_node->_right != NULL)
      {
        BSTreeNode<T> *finding_min = _node->_right;
        while (finding_min->_left != NULL)
        {
          finding_min = finding_min->_left;
        }
        _node = finding_min;
        return temp;
      }
      else
      {
        BSTreeNode<T> *successor = NULL;
        BSTreeNode<T> *_root_temp = _root_in_iter;

        while (_root_temp != NULL)
        {
          if (_node->_data < _root_temp->_data)
          {
            successor = _root_temp;
            _root_temp = _root_temp->_left;
          }
          else if (_node->_data >= _root_temp->_data)
          {
            if (_node == _root_temp)
            {
              break;
            }
            _root_temp = _root_temp->_right;
          }
        }
        _node = successor;
        return temp;
      }
    }
    iterator &operator--()
    {
      if (_node == NULL)
      {

        BSTreeNode<T> *_root_temp = _root_in_iter;
        while (_root_temp->_right != NULL)
        {
          _root_temp = _root_temp->_right;
        }
        _node = _root_temp;
        // iterator max(_root_temp,_root_in_iter);
        // return max;
        return *(this);
      }
      if (_node->_left != NULL)
      {
        BSTreeNode<T> *finding_max = _node->_left;
        while (finding_max->_right != NULL)
        {
          finding_max = finding_max->_right;
        }
        _node = finding_max;
        // iterator the_max(finding_max);
        // return the_max;
        return *(this);
      }
      else
      {
        BSTreeNode<T> *predecessor = NULL;
        BSTreeNode<T> *_root_temp = _root_in_iter;

        while (_root_temp != NULL)
        {
          if (_node->_data < _root_temp->_data)
          {
            if (_node == _root_temp)
            {
              break;
            }
            _root_temp = _root_temp->_left;
          }
          else if (_node->_data >= _root_temp->_data)
          {
            if (_node == _root_temp)
            {
              break;
            }
            predecessor = _root_temp;
            _root_temp = _root_temp->_right;
          }
        }
        _node = predecessor;
        // iterator pred(predecessor, _root_in_iter);
        // return pred;
        return *(this);
      }
    }
    iterator operator--(int)
    {
      iterator temp(_node, _root_in_iter);
      if (_node->_left != NULL)
      {
        BSTreeNode<T> *finding_max = _node->_left;
        while (finding_max->_right != NULL)
        {
          finding_max = finding_max->_right;
        }
        _node = finding_max;
        iterator the_max(finding_max);
        return temp;
      }
      else
      {
        BSTreeNode<T> *predecessor = NULL;
        BSTreeNode<T> *_root_temp = _root_in_iter;

        while (_root_temp != NULL)
        {
          if (_node->_data < _root_temp->_data)
          {
            if (_node == _root_temp)
            {
              break;
            }
            _root_temp = _root_temp->_left;
          }
          else if (_node->_data >= _root_temp->_data)
          {
            if (_node == _root_temp)
            {
              break;
            }
            predecessor = _root_temp;
            _root_temp = _root_temp->_right;
          }
        }
        _node = predecessor;
        iterator pred(predecessor, _root_in_iter);
        return temp;
      }
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
      return false;
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
      return false;
    }

  private:
    BSTreeNode<T> *_node;
    BSTreeNode<T> *_root_in_iter;
  };

  // TODO implement these functions
  iterator begin() const
  {
    if (_root == NULL)
    {
      return NULL;
    }
    BSTreeNode<T> *_root_temp = _root;
    while (_root_temp->_left != NULL)
    {
      _root_temp = _root_temp->_left;
    }
    iterator min(_root_temp, _root);
    // cout <<"root_temp's _data (begin())"<< _root_temp->_data << endl;
    return min;
  }
  iterator end() const
  {

    // BSTreeNode<T> *_the_end = new BSTreeNode<T>;
    // BSTreeNode<T> *_root_temp = _root;
    // while (_root_temp->_right != NULL)
    // {
    //   _root_temp = _root_temp->_right;
    // }
    // _root_temp->_right = _the_end;
    // iterator end_pos(_the_end, _root);

    // return end_pos;

    // cout <<"root_temp's _data(end())"<< _root_temp->_data << endl;
    // return max;
    // return end_pos;
    iterator temp(NULL, _root);
    return temp;
  }
  bool empty() const { return (_root == NULL); }
  size_t size() const
  {
    size_t counter = 0;
    iterator traverse = begin();
    while (traverse != end())
    {
      ++traverse;
      counter++;
    }
    return counter;
  }

  void insert(const T &x)
  {
    _root = insertion(x, _root);
  }
  BSTreeNode<T> *insertion(const T &x, BSTreeNode<T> *n)
  {
    if (n == NULL)
    {
      n = new BSTreeNode<T>(x, NULL, NULL);
    }
    else if (x >= n->_data)
    {
      n->_right = insertion(x, n->_right);
    }
    else if (x < n->_data)
    {
      n->_left = insertion(x, n->_left);
    }
    return n;
  }
  void pop_front()
  {
    iterator temp = begin();
    erase(temp);
  }
  void pop_back()
  {

    BSTreeNode<T> *_root_temp = _root;
    if(_root == NULL)
    {
      return;
    }
    while (_root_temp->_right != NULL)
    {
      _root_temp = _root_temp->_right;
    }

    iterator max(_root_temp, _root);
    // cout <<"root_temp's _data(end())"<< _root_temp->_data << endl;
    erase(max);
  }

  // return false if nothing to erase
  bool erase(iterator pos)
  {
    if (pos == NULL)
    {
      return false;
    }
    BSTreeNode<T> *_parent = finding_parent(pos._node);

    if (pos._node->_left == NULL || pos._node->_right == NULL)
    {
      if (_parent == NULL)
      {
        if (pos._node->_right != NULL)
          _root = pos._node->_right;
        else
          _root = pos._node->_left;
        _change_root = true;
        delete pos._node;
        return true;
      }
      if (*pos < _parent->_data)
      {
        if (pos._node->_right != NULL)
          _parent->_left = pos._node->_right;
        else
          _parent->_left = pos._node->_left;
        delete pos._node;
        return true;
      }
      else //>=
      {
        if (pos._node->_right != NULL)
          _parent->_right = pos._node->_right;
        else
          _parent->_right = pos._node->_left;
        delete pos._node;
        return true;
      }
    }
    else //left child and right child are all alive
    {
      BSTreeNode<T> *successor = finding_successor(pos._node);
      T store_data = successor->_data;

      BSTreeNode<T> *_parent_of_successor = finding_parent(successor);
      if (successor->_data >= _parent_of_successor->_data)
        _parent_of_successor->_right = successor->_right;
      else
        _parent_of_successor->_left = successor->_right;
      delete successor;
      pos._node->_data = store_data;
      return true;
      // iterator temp(successor, _root);
      // erase(temp);
    }
    return false;
  }
  bool erase(const T &x)
  {
    if (erase(find(x)))
    {
      return true;
    }
    return false;
  }

  iterator find(const T &x)
  {
    for (iterator li = begin(); li != end(); ++li)
    {
      if (x == *li)
      {
        return li;
      }
    }
    return end();
  }

  BSTreeNode<T> *finding_parent(BSTreeNode<T> *_target) const
  {
    if (_target == _root)
    {
      return NULL;
    }
    BSTreeNode<T> *_parent = _root;
    while (1)
    {
      if (_target->_data < _parent->_data)
      {
        if (_parent->_left == _target)
        {
          return _parent;
        }
        _parent = _parent->_left;
      }
      else if (_target->_data >= _parent->_data)
      {
        if (_parent->_right == _target)
        {
          return _parent;
        }
        _parent = _parent->_right;
      }
    }
  }
  BSTreeNode<T> *finding_successor(BSTreeNode<T> *_target) const
  {
    BSTreeNode<T> *success = _target->_right;
    while (success->_left != NULL)
    {
      success = success->_left;
    }
    return success;
  }
  void clear()
  {
    iterator temp = begin();
    while (temp != end())
    {
      // if (*temp==T(NULL))
      // {
      //   cout << "Bang" << endl;
      // }
      if(_change_root == true)
      {
        temp._root_in_iter = _root;
        _change_root = false;
      }
      // iterator store = temp;
      // temp++;
      erase(temp++);
    }
  } // delete all nodes except for the dummy node

  void sort() const { return; }

  void print() const
  {
    cout << "Not implement" << endl;
  }

private:
  BSTreeNode<T> *_root;
  bool _change_root = false;
};

#endif // BST_H
