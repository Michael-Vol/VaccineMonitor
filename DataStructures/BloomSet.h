#ifndef BLOOMSET_H
#define BLOOMSET_H
#include "BloomFilter.h"
#include <iostream>
#include <string>

using namespace std;

class BloomNode {
  BloomFilter *filter;
  string virus;

public:
  BloomNode *next;
  BloomNode(BloomFilter *filter, string virus);
  ~BloomNode();
  string getVirusName();
  BloomFilter *getFilter();
  uint64_t calculateSize();
  void mergeFilters(BloomFilter *filter);
};

class BloomSet {
  BloomNode *set;

public:
  BloomSet();
  ~BloomSet();
  BloomNode *addBloomNode(BloomFilter *filter, string virus);
  uint64_t calculateSize();
  BloomNode *getNode(string virus);
  int getNumOfNodes();
  uint64_t getNodeSize(int nodeIndex);
  string getVirusName(int nodeIndex);
  BloomNode *getNode(int nodeIndex);
  bool checkVaccination(string virus, int citizenID);
};
#endif