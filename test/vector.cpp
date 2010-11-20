#include <iostream>
#include "test.h"
#include "vector.h"

using namespace std;
using namespace Foundation;

void testVectorSize() {
  Vector<int> vector;
  assertEquals(0, vector.size());
  vector << 10;
  vector << 20;
  assertEquals(2, vector.size());
}

void testFirstAndLast() {
  Vector<int> vector;
  assertThrows(VectorAccessException<int>, vector.first());
  assertThrows(VectorAccessException<int>, vector.last());
  vector << 10;
  assertEquals(10, vector.last());
  assertEquals(10, vector.first());
  
  vector << 20;
  assertEquals(20, vector.last());
  assertEquals(10, vector.first());
  
  vector << 30;
  assertEquals(30, vector.last());
  assertEquals(10, vector.first());
}

void testGoodVectorAccess() {
  Vector<int> vector;
  vector << 10;
  vector << 20;
  assertEquals(10, vector[0]);
  assertEquals(20, vector[1]);
  vector[0] = 30;
  assertEquals(30, vector[0]);
}

void testBadVectorAccess() {
  Vector<int> vector;
  vector << 10;
  vector << 20;
  assertThrows(VectorAccessException<int>, vector[2]);
}

void testVectorGrowing() {
  Vector<int> vector;
  int max = 1000;
  for (int i = 0; i < max; ++i) {
    vector << i;
  }
  assertEquals(max, vector.size());
  for (int i = 0; i < max; ++i) {
    assertEquals(i, vector[i]);
  }
}

void testIndexOfElement() {
  Vector<int> vector;
  for (int i = 0; i < 100; ++i) {
    vector << i * i;
  }
  assertEquals(25, vector.index(625));
  assertEquals(-1, vector.index(9999));
  
  // test last index
  vector.clear();
  vector << 20 << 10 << 10 << 20 << 50;
  assertEquals(3, vector.lastIndex(20));
}

void testCopy() {
  // create original
  Vector<int> vector;
  for (int i = 0; i < 100; ++i) {
    vector << i;
  }
  
  Vector<int> copyVector = vector.slice(0);
  for (int i = 0; i < 100; ++i) {
    assertEquals(i, copyVector[i]);
    assertNotEquals(&vector[i], &copyVector[i]);
  }
}

void testSlicing() {
  // create original
  Vector<int> vector;
  for (int i = 0; i < 100; ++i) {
    vector << i;
  }
  
  // 1 on 1 copy
  Vector<int> slice1 = vector.slice(0);
  for (int i = 0; i < 100; ++i) {
    assertEquals(i, slice1[i]);
    assertNotEquals(&vector[i], &slice1[i]);
  }
  
  // half slice
  Vector<int> slice2 = vector.slice(50);
  for (int i = 0; i < 50; ++i) {
    assertEquals(i + 50, slice2[i]);
    assertNotEquals(&vector[i + 50], &slice2[i]);
  }
  
  // slice of 10 elements
  Vector<int> slice3 = vector.slice(50, 10);
  for (int i = 0; i < 10; ++i) {
    assertEquals(i + 50, slice3[i]);
    assertNotEquals(&vector[i + 50], &slice3[i]);
  }
  assertEquals(10, slice3.size());
}

int powerOfTwo(int value) {
  return value * value;
}

void testMapping() {
  Vector<int> vector;
  for (int i = 0; i < 100; ++i) {
    vector << i;
  }
  
  vector.map(powerOfTwo);
  
  for (int i = 0; i < 100; ++i) {
    assertEquals(i * i, vector[i]);
  }
}

void testReverse() {
  Vector<int> vector;
  
  // just one value
  vector << 1;
  vector.reverse();
  assertEquals(1, vector.first());
  
  // 3 values
  vector << 2;
  vector << 3;
  vector.reverse();
  assertEquals(3, vector.first());
  assertEquals(2, vector[1]);
  assertEquals(1, vector.last());
  
  // 100 values
  vector.clear();
  for (int i = 0; i < 100; ++i) {
    vector << i;
  }
  vector.reverse();
  for (int i = 99, j = 0; i >= 0; --i) {
    assertEquals(i, vector[j++]);
  }
}

int descOrder(const int &left, const int &right) {
  if (left == right) return 0;
  else if (left < right) return -1;
  else return 1;
}

void testSort() {
  int numbers[] = {
    1, 22, 4, 15, 69, 7, 88, 90, 0, 7
  };
  Vector<int> vector(numbers, 10);
  
  // default sorting
  vector.sort();
  assertEquals(0, vector[0]);
  assertEquals(1, vector[1]);
  assertEquals(4, vector[2]);
  assertEquals(7, vector[3]);
  assertEquals(7, vector[4]);
  assertEquals(15, vector[5]);
  assertEquals(22, vector[6]);
  assertEquals(69, vector[7]);
  assertEquals(88, vector[8]);
  assertEquals(90, vector[9]);
  
  // own sorting method using custom comperator
  vector.sort(descOrder);
  assertEquals(90, vector[9]);
  assertEquals(88, vector[8]);
  assertEquals(69, vector[7]);
  assertEquals(22, vector[6]);
  assertEquals(15, vector[5]);
  assertEquals(7, vector[4]);
  assertEquals(7, vector[3]);
  assertEquals(4, vector[2]);
  assertEquals(1, vector[1]);
  assertEquals(0, vector[0]);
}

int main (int argc, char * const argv[]) {
  Test::Suite suite("Vector", 20);
  suite << testVectorSize;
  suite << testFirstAndLast;
  suite << testGoodVectorAccess;
  suite << testBadVectorAccess;
  suite << testVectorGrowing;
  suite << testIndexOfElement;
  suite << testCopy;
  suite << testSlicing;
  suite << testMapping;
  suite << testReverse;
  suite << testSort;
  suite.run();
  return 0;
}
