/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>
// #define parent(a)
template <class Data>

class MinHeap
{
 public:
   MinHeap(size_t s = 0)
   {
      if (s != 0)
         _data.reserve(s);
   }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data &operator[](size_t i) const { return _data[i]; }
   Data &operator[](size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO:
   const Data &min() const { return _data[0]; }
   void insert(const Data &d)
   {
      _data.push_back(d);
      size_t count = _data.size() - 1;
      while (count != 0)
      {
         if (_data[count] < parent(count))
         {
            swap(parent(count), _data[count]);
            count = (count - 1) / 2;
         }
         else
            break;
      }
      // cout << "insert success to position: " << count << endl;
   }
   void delMin() { delData(0); }
   void delData(size_t i)
   {
      if(_data.empty())
      {
         return;
      }
      const size_t num_of_node = _data.size() - 1;
      swap(_data[i], _data[num_of_node]);
      _data.pop_back();
      if (num_of_node <= 1)
         return;
      if (num_of_node == 2)
      {
         if (!(_data[0] < _data[1]))
         {
            swap(_data[0], _data[1]);
         }
         return;
      }
      size_t count = i;
      while (count != 0)
      {
         if (_data[count] < parent(count))
         {
            swap(_data[count], parent(count));
            count = (count - 1) / 2;
         }
         else
         break;
      }
      count = i;
      while (num_of_node > count * 2 + 2)
      {

         if (right_child(count) < _data[count] && right_child(count) < left_child(count)) //right_child is the smallest
         {
            swap(right_child(count), _data[count]);
            count = 2 * count + 2; //the index of right_child
         }
         else if (left_child(count) < _data[count]) //left child is the smallest
         {
            swap(left_child(count), _data[count]);
            count = 2 * count + 1; //the index of left_child
         }
         // if (left_child(count) < _data[count] && left_child(count) < right_child(count)) //left_child is the smallest
         // {
         //    swap(left_child(count), _data[count]);
         //    count = 2 * count + 1; //the index of left_child
         // }
         // else if (right_child(count) < _data[count]) //right child is the smallest
         // {
         //    swap(right_child(count), _data[count]);
         //    count = 2 * count + 2; //the index of right_child
         // }
         else
            break;
      }
      if (num_of_node == 2 * count + 2 && left_child(count) < _data[count])
      {
         swap(left_child(count), _data[count]);
      }
      return;
   }
   inline Data &parent(size_t i)
   {
      return _data[(i - 1) / 2];
   }
   inline Data &left_child(size_t i)
   {
      return _data[2 * i + 1];
   }
   inline Data &right_child(size_t i)
   {
      return _data[2 * i + 2];
   }
   void filter_on(size_t i)
   {
   }

 private:
   // DO NOT add or change data members
   vector<Data> _data;
};

#endif // MY_MIN_HEAP_H
