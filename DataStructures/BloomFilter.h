#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H
#include "../Utils/Hash.h"
#include <iostream>
using namespace std;

class BloomFilter {
  int *filter;
  int filterSize;

public:
  BloomFilter(int filterSize);
  ~BloomFilter();
  void fillFilter(int *fillArray);
  void insert(int citizenID);
  bool check(int citizenID);
  void merge(BloomFilter *filter);
  int *getFilter();
  void setBit(unsigned long bitPos);
  int findBitValue(unsigned long bitPos);
  uint64_t getFilterSize();
};
#endif