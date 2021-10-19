#include "BloomSet.h"

using namespace std;

BloomSet::BloomSet() {
  set = nullptr;
}

BloomSet::~BloomSet() {
  BloomNode *current = set;
  BloomNode *next;
  while (current) {
    next = current->next;
    delete current;
    current = next;
  }
}

BloomNode *BloomSet::addBloomNode(BloomFilter *filter, string virus) {
  BloomNode *current = set;
  BloomNode *node;
  while (current && current->next) {
    current = current->next;
  }
  if (set == nullptr) {
    //set  head of list
    node = set = new BloomNode(filter, virus);
  } else {
    //set last item
    node = current->next = new BloomNode(filter, virus);
  }
  return node;
}

BloomNode *BloomSet::getNode(string virus) {
  BloomNode *current = set;
  while (current) {
    if (current->getVirusName() == virus) {
      return current;
    }
    current = current->next;
  }
  return nullptr;
}
uint64_t BloomSet::calculateSize() {
  int numOfNodes = 0;
  BloomNode *current = set;
  uint64_t totalSize = 0;
  while (current) {
    totalSize += current->calculateSize();
    current = current->next;
  }
  return totalSize;
}
int BloomSet::getNumOfNodes() {
  BloomNode *current = set;
  int nodes = 0;
  while (current) {
    nodes++;
    current = current->next;
  }
  return nodes;
}

uint64_t BloomSet::getNodeSize(int nodeIndex) {
  BloomNode *current = set;
  int count = 0;
  while (current) {
    if (count == nodeIndex) {
      return current->calculateSize();
    }
    current = current->next;
    count++;
  }
  return -1;
}

BloomNode *BloomSet::getNode(int nodeIndex) {
  BloomNode *current = set;
  int count = 0;
  while (current) {
    if (count == nodeIndex) {
      return current;
    }
    current = current->next;
    count++;
  }
  return nullptr;
}

string BloomSet::getVirusName(int nodeIndex) {
  BloomNode *current = set;
  int count = 0;
  while (current) {
    if (count == nodeIndex) {
      return current->getVirusName();
    }
    current = current->next;
    count++;
  }
  return "";
}
BloomNode::BloomNode(BloomFilter *filter, string virus) {
  this->filter = filter;
  this->virus = virus;
  next = nullptr;
}
BloomNode::~BloomNode() {
  delete filter;
}

void BloomNode::mergeFilters(BloomFilter *filter) {
  this->filter->merge(filter);
}
string BloomNode::getVirusName() {
  return virus;
}

BloomFilter *BloomNode::getFilter() {
  return filter;
}

uint64_t BloomNode::calculateSize() {
  return filter->getFilterSize() + virus.size();
}

bool BloomSet::checkVaccination(string virus, int citizenID) {
  BloomNode *current = set;
  while (current) {
    if (current->getVirusName() == virus) {
      return current->getFilter()->check(citizenID);
    }
    current = current->next;
  }
  return false;
}
