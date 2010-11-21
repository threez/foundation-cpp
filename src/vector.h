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
    /// this is the prototype for every mapping function accepred by this vector
    typedef Item (*mappingFunction)(const Item);
    
    /// this is the prototype for every compare function accepted by this vector
    typedef int (*compareFunction)(const Item &left, const Item &right);
    
  private:
    
    /// the array that holds the elements of the vector
    Item * elements;
    
    /// the index that keeps track of the max size of the vector
    Index maxSize;
    
    /**
     * the index that keeps track of the current size of the vector
     * the real fill state. 
     */
    Index elementsSize;
    
  public:
    
    /**
     * initlalize the vector with a new max size
     * @param size the first initial max size for the vector
     */
    Vector(Index size = 10)
    :elements(NULL), maxSize(size)
    {
      this->clear();
    }
    
    /**
     * initialize an array using a plain old array and size of array. the
     * generated vector will have exactly the size of the passed array.
     * @param array the array to copy in
     * @param size the size of the passed array
     */
    Vector(Item * array, int size)
    :elements(NULL), maxSize(size)
    {
      this->clear();
      memcpy(this->elements, array, sizeof(Item) * this->maxSize);
      this->elementsSize = size;
    }
    
    /**
     * deletes the internal data structure that holds the data
     */
    ~Vector() {
      free(this->elements);
    }
    
    /**
     * returns the size of the vector
     * @return number of elements
     */
    Index size() const {
      return this->elementsSize;
    }
    
    /**
     * returns true if the vector is empty
     */
    bool isEmpty() const {
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
     * @throws: VectorAccessException if the vector is empty
     */
    Item &first() {
      return this->elements[this->indexFor(0)];
    }
    
    /**
     * adds an item to the vector and returns self to allow chaining.
     * @param item the item to add to the vector
     * @return self (the current vector) to enable chaining of <<
     */
    Vector<Item, Index> &operator<<(const Item &item) {
      if (this->elementsSize >= this->maxSize) {
        this->resizeTo(this->maxSize * 2);
      }
      this->at(this->elementsSize++) = item;
      
      return *(this);
    }
    
    /**
     * returns a reference to the element a the passed index.
     * @param index the index to get the item from. The index may be negative 
     *              and will be converted in Nth item before the end.
     */
    Item &at(const Index index) {
      return this->elements[this->indexFor(index)];
    }
    
    /*
     * searches for the item in the vector and returns the first occurance
     * if nothing was found -1 will be returned
     * @param item the item to search for
     * @return -1 for nothing, otherwiese a positive index
     */
    Index index(const Item item) const {
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
     * @param item the item to search for
     * @return -1 for nothing, otherwiese a positive index
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
     * returns a full copy of the vector
     */
    Vector<Item, Index> copy() {
      return slice(0);
    }
    
    /*
     * returns a slice from the starting of the vector to the end
     * @param start the start index from where to slice from. If you want to
     *              slice from the beginning just pass 0. The index may be
     *              negative and will be converted in Nth item before the end.
     * @return a new vector that contains all slice items
     */
    Vector<Item, Index> slice(const int start) {
      return slice(start, this->elementsSize - start);
    }
    
    /**
     * returns a slice from the start index with a max count of entrys of 
     * size
     * @param start the start index from where to slice from. If you want to
     *              slice from the beginning just pass 0. The index may be
     *              negative and will be converted in Nth item before the end.
     * @param size the number of elements that should be in the new vector
     * @return a new vector that contains all slice items
     */
    Vector<Item, Index> slice(const Index start, const Index size) {
      Vector<Item, Index> newSlice(size);
      newSlice.copyFrom(this, start, size);
      return newSlice;
    }
    
    /**
     * map all values of the vector using the passed function
     * @param fn the function that will be used to manipulate the current vector 
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
     * sort this array using the defaultComperator. This sort using quicksort
     * with no worst case O(N^2) protection.
     * @param fn the function to use, to compare the elements while sorting
     */
    void sort(compareFunction fn = defaultCompare) {
      this->quicksort(0, this->elementsSize - 1, fn);
    }
    
    /**
     * searches for the element at the passed index. The index can be either
     * positive or negative. Negative values translate to the Nth value before
     * the end of the vector.
     * Throws an exception of type VectorAccessException<Item> when a incorrect
     * index was passed.
     * @return the element reference that was found at the given index
     */
    Item &operator[](const Index index) {
      return this->at(index);
    }
    
    /*
     * searches for the element at the passed index. The index can be either
     * positive or negative. Negative values translate to the Nth value before
     * the end of the vector.
     * Throws an exception of type VectorAccessException<Item> when a incorrect
     * index was passed.
     * @return the element that was found at the given index
     */
    Item operator[](const Index index) const {
      return *this->at(index);
    }
    
    /*
     * removes all items from the vector that are equal to item. This
     * will traverse the whole Vector in O(N). 
     * @param the element that will be used for searching
     * @return the count of deleted items, 0 when no item was removed 
     */
    Index remove(const Item &item) {
      Index removed = 0;
      for (Index i = 0; i < this->elementsSize; ++i) {
        if (this->elements[i] == item) {
          this->removeAt(i);
          removed++;
        }
      }
      return removed;
    }
    
    /**
     * removes the element at the passed index
     * @param index the index of the element to remove. This can be a positive
     *              or negative index. Negative means Nth item before end.
     * @return the element that was removed
     */
    Item removeAt(Index index) {
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
     * returns a string representation of vector
     */
    std::string inspect() const {
      std::ostringstream details;
      details << "<Foundation::Vector#" << this << " size:" << this->size()
              << " values:" << this->toString() << ">";
      return details.str();
    }
    
    /**
     * returns a string representation of the values of the vector
     */
    std::string toString() const {
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
     * and allocating new one. The vector will therefor use the old max
     * size. So by clearing the vector no space will be freed.
     * It's just the data that will be discarded.
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
    
    /**
     * implements a quicksort on the vector. The comparison can be overwritten
     * using a compare function fn.
     * @param left the start of the partition, on first call 0
     * @param right the end of the partition, on start usually (size - 1)
     * @param fn the function that will be used to compare
     */
    void quicksort(Index left, Index right, compareFunction fn) {
      if (left < right) {
        Index partition = quicksortPartition(left, right, fn);
        quicksort(left, partition - 1, fn);
        quicksort(partition + 1, right, fn);
      }
    }
    
    /**
     * returns the index to partition by 
     * WARNING: has no protection against the worst case O(N^2) behaviour
     * of quicksort. Maybe i implement introsort which protects you from this
     * and keep up the O(NlogN= behaviour of the algorithm.
     * @param left the start of the partition
     * @param right the end of the partition
     * @param fn the function that will be used to compare
     */
    Index quicksortPartition(Index left, Index right, compareFunction fn) {
      Index i = left;
      Index j = right - 1;
      Item pivot = this->elements[right];  // stupid pivot search
      
      do {
        while (fn(this->elements[i], pivot) <= 0 && i < right) i++;
        while (fn(this->elements[j], pivot) >= 0 && j > left) j--;
        if (i < j) swap(this->elements[i], this->elements[j]);
      } while (i < j);
      
      if (fn(this->elements[i], pivot) > 0) {
        swap(this->elements[i], this->elements[right]);
      }
        
      return i;
    }
    
    /**
     * copys a partition of, or the whole other vector. The boundrys will be
     * checked using normal access checking.
     * @param vector the vector to copy from
     * @param start the start index (0 to copy from the beginning or higher).
     *              This can also be a negaive number like -10 which results in
     *              the 10th element befor the end of the vector.
     * @param size the number of elements to copy. This may not exceed the
     *             number of elements that are in the vector.
     */
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
     * verifies and calculates the correct index to access the vector elements.
     * It will translate negative numbers to a count from the end of the list.
     * So when one tries to access -1 the result is the last element of the
     * vetor.
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
    inline size_t bytes() const {
      return (size_t)this->maxSize * sizeof(Item);
    }
    
    /**
     * resize the array to the given size
     * @param size the size of the new array (number of elements, not bytes)
     */
    void resizeTo(const Index size) {
      this->maxSize = size;
      this->elements = (Item *)realloc(this->elements, this->bytes());
    }
  };
};

#endif
