/*
 *  vector.h
 *  foundation-cpp
 *
 *  Created by Vincent Landgraf on 16.11.10.
 *  Copyright 2010 Vincent Landgraf. All rights reserved.
 *
 */
#ifndef FOUNDATION_VECTOR
#define FOUNDATION_VECTOR

#include <sstream>

namespace Foundation {
  template <typename Item>
  inline void swap(Item &left, Item &right) {
    Item tmp = left;
    left = right;
    right = tmp;
  }
  
  template <typename Item>
  int defaultCompare(const Item &left, const Item &right) {
    if (left == right) return 0;
    else if (left < right) return -1;
    else return 1;
  }
  
  template <typename Item, typename Index = int>
  class Vector;
  
  template <typename Item, typename Index = int>
  class VectorAccessException : public std::exception {
  private:
    
    char * msg;
    
  public:
    
    VectorAccessException(Vector<Item, Index> *vector, Index index)
    :std::exception()
    {
      std::ostringstream error;
      
      // construct the message using the error string stream
      error << "You tried to access the vector("
            << vector->size() << ") at index " << index;
      if (vector->isEmpty()) {
        // error because of empty vector
        error << " but it is empty!";
      } else {
        // error because of bad access
        Index from = -(vector->size()) + 1;
        error << " but it is only being allowed between "
              << from << " and " << vector->size() << "!";
      }
      
      // store the message in the char * msg
      std::string msgString = error.str();
      this->msg = new char[msgString.size()];
      memcpy(this->msg, msgString.c_str(), msgString.size());
    }
    
    virtual const char* what() const throw() {
      return this->msg;
    };
  };
  
  template <typename Item, typename Index>
  class Vector {
    typedef Item (*mappingFunction)(const Item);
    typedef int (*compareFunction)(const Item &left, const Item &right);
    
  private:
    
    Item * elements;
    Index maxSize;
    Index elementsSize;
    
  public:
    
    Vector(Index size = 10)
    :elements(NULL), maxSize(size)
    {
      this->clear();
    }
    
    Vector(Item * array, int size)
    :elements(NULL), maxSize(size)
    {
      this->clear();
      memcpy(this->elements, array, sizeof(Item) * this->maxSize);
      this->elementsSize = size;
    }
    
    ~Vector() {
      free(this->elements);
    }
    
    /**
     * returns the size of the vector
     */
    Index size() {
      return this->elementsSize;
    }
    
    /**
     * returns true if the vector is empty
     */
    bool isEmpty() {
      return this->elementsSize == 0;
    }
    
    /**
     * returns the last element of the list
     * @throws VectorAccessException if the vector is empty
     */
    Item &last() {
      return this->elements[this->indexFor(-1)];
    }
    
    /*
     * returns the first element of the list
     * throws: VectorAccessException if the vector is empty
     */
    Item &first() {
      return this->elements[this->indexFor(0)];
    }
    
    Vector<Item, Index> &operator<<(const Item &item) {
      if (this->elementsSize >= this->maxSize) {
        this->resizeTo(this->maxSize * 2);
      }
      this->at(this->elementsSize++) = item;
      
      return *(this);
    }
    
    Item &at(const Index index) {
      return this->elements[this->indexFor(index)];
    }
    
    /*
     * searches for the item in the vector and returns the first occurance
     * if nothing was found -1 will be returned
     */
    Index index(const Item item) {
      for (Index i = 0; i < this->elementsSize; ++i) {
        if (this->elements[i] == item) {
          return i;
        }
      }
      return -1;
    }
    
    /*
     * searches for the item in the vector and returns the last occurance
     * if nothing was found -1 will be returned
     */
    Index lastIndex(const Item item) {
      Index found = -1;
      for (Index i = 0; i < this->elementsSize; ++i) {
        if (this->elements[i] == item) {
          found = i;
        }
      }
      return found;
    }
    
    /*
     * returns a copy of the vector
     */
    Vector<Item, Index> copy() {
      return slice(0);
    }
    
    /*
     * returns a slice from the starting of the vector to the end
     */
    Vector<Item, Index> slice(const int start) {
      return slice(start, this->elementsSize - start);
    }
    
    /**
     * returns a slice from the start index with a max count of entrys of 
     * size
     */
    Vector<Item, Index> slice(const Index start, const Index size) {
      Vector<Item, Index> newSlice(size);
      newSlice.copyFrom(this, start, size);
      return newSlice;
    }
    
    /**
     * map all values of the vector using the passed function
     */
    void map(mappingFunction fn) {
      for (Index i = 0; i < this->elementsSize; ++i) {
        this->elements[i] = fn(this->elements[i]);
      }
    }
    
    /**
     * reverse all elements in this vector
     */
    void reverse() {
      for (Index i = 0, j = this->elementsSize - 1; i < this->elementsSize / 2; ++i) {
        swap(this->elements[j--], this->elements[i]);
      }
    }
    
    /**
     * sort this array using the default comperator
     */
    void sort(compareFunction fn = defaultCompare) {
      this->quicksort(0, this->elementsSize - 1, fn);
    }
    
    /**
     * allow lvalue asssignment using the square bracket operator
     *  
     * @param
     * @returns 
     */
    Item &operator[](const Index index) {
      return at(index);
    }
    
    // return index
    Item operator[](const Index index) const {
      return *at(index);
    }
    
    /**
     * removes the element at the passed index
     */
    Item deleteAt(Index index) {
      index = this->indexFor(index);
      // save value
      Item item = this->elements[index];
      // move whole array over
      memmove(&this->elements[index], &this->elements[index + 1],
              (size_t)(sizeof(Item) * (this->elementsSize - index - 1)));
      this->elementsSize--;
      return item;
    }
    
    /**
     * return string representation of vector
     */
    std::string inspect() {
      std::ostringstream details;
      details << "<Foundation::Vector#" << this << " size:" << this->size()
              << " values:" << this->toString() << ">";
      return details.str();
    }
    
    /**
     * returns the string representation of the vector
     */
    std::string toString() {
      std::ostringstream values;
      values << "{";
      for (Index i = 0; i < this->elementsSize; ++i) {
        values << this->elements[i];
        if (i != this->elementsSize - 1) {
          values << ", ";
        }
      }
      values << "}";
      return values.str();
    }
    
    
    /**
     * removes all elements from the vector by freeing the old memory
     * and allocating new
     */
    void clear() {
      // free old data
      if (this->elements != NULL) free(this->elements);
      
      // reset usage
      this->elementsSize = 0;
      
      // we create a array with pointers to move around
      this->elements = (Item *)malloc(this->bytes());
    }
    
  protected:
    
    void quicksort(Index left, Index right, compareFunction fn) {
      if (left < right) {
        Index partition = quicksortPartition(left, right, fn);
        quicksort(left, partition - 1, fn);
        quicksort(partition + 1, right, fn);
      }
    }
    
    Index quicksortPartition(Index left, Index right, compareFunction fn) {
      Index i = left;
      Index j = right - 1;
      Item pivot = this->elements[right];
      
      do {
        while (fn(this->elements[i], pivot) <= 0 && i < right) i++;
        while (fn(this->elements[j], pivot) >= 0 && j > left) j--;
        if (i < j) {
          swap(this->elements[i], this->elements[j]);
        }
      } while (i < j);
      
      if (fn(this->elements[i], pivot) > 0) {
        swap(this->elements[i], this->elements[right]);
      }
        
      return i;
    }
    
    void copyFrom(Vector<Item, Index> *vector, const Index start, const Index size) {
      Index begin = vector->indexFor(start);
      Index end = vector->indexFor(start + size - 1);
      // check if the end and the start are in correct order
      if (end < begin) {
        throw VectorAccessException<Item, Index>(vector, end);
      } else {
        memcpy(this->elements, &(vector->at(begin)), sizeof(Item) * size);
        this->elementsSize = size;
      }
    }
    
    /**
     *
     */
    inline Index indexFor(Index index) {
      // translate negative to positive(negatives are starting from the tail of
      // the list) so given a size of 2 and index of -1 means 1 (the last member
      // of the list)
      if (index < 0) index = this->elementsSize + index;
      
      // check if all constraints are met
      if (index < 0 || index >= this->elementsSize)
        throw VectorAccessException<Item>(this, index);
      
      return index;
    }
    
    /**
     * returns the the size in bytes that is needed for this vector
     */
    inline size_t bytes() {
      return (size_t)this->maxSize * sizeof(Item);
    }
    
    /**
     * resize the array to the given size
     * @param size: the size of the new array
     */
    void resizeTo(const Index size) {
      this->maxSize = size;
      this->elements = (Item *)realloc(this->elements, this->bytes());
    }
  };
};

#endif
