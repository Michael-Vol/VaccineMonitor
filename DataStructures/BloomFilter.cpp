#include "BloomFilter.h"
#include <iostream>

using namespace std;

BloomFilter::BloomFilter(int filterSize) {
  filterSize = filterSize / sizeof(int);
  this->filter = new int[filterSize];
  for (int i = 0; i < filterSize; i++) {
    filter[i] = 0;
  }

  this->filterSize = filterSize;
}

BloomFilter::~BloomFilter() {
  delete[] filter;
}

int *BloomFilter::getFilter() {
  return filter;
}
void BloomFilter::setBit(unsigned long bitPos) {
  int index = bitPos / 32;    //get index of filter Array
  int position = bitPos % 32; //get bitPosition in the array element at index

  // get flag (bit = 1 ) at position
  unsigned int flag = 1;
  flag = flag << position;
  filter[index] = filter[index] | flag; //set bit to 1
}

int BloomFilter::findBitValue(unsigned long bitPos) {
  int index = bitPos / 32;    //get index of filter Array
  int position = bitPos % 32; //get bitPosition in the array element at index

  // get flag (bit = 1 ) at position
  unsigned int flag = 1;
  flag = flag << position;

  if (filter[index] & flag) { //test bit position
    return 1;                 // bit is one
  }
  return 0; //bit is zero
}

void BloomFilter::insert(int citizenID) {
  //convert citizenID
  string citizenIDString = to_string(citizenID);
  unsigned char *ctrCitizenID = (unsigned char *)citizenIDString.c_str();

  for (int i = 0; i < HASHING_TIMES; i++) {
    //hash citizenID
    unsigned long bloomBit = hash_i(ctrCitizenID, i) % (8 * filterSize);
    //insert virus to bloomfilter (set bit)
    setBit(bloomBit);
  }
  //delete ctrCitizenID;
}

bool BloomFilter::check(int citizenID) {
  //convert citizenID
  string citizenIDString = to_string(citizenID);
  unsigned char *ctrCitizenID = (unsigned char *)citizenIDString.c_str();

  for (int i = 0; i < HASHING_TIMES; i++) {
    //hash citizenID
    unsigned long bloomBit = hash_i(ctrCitizenID, i) % (8 * filterSize);
    if (!findBitValue(bloomBit)) {
      return false;
    }
  }
  return true;
}
void BloomFilter::merge(BloomFilter *filter) {
  for (int i = 0; i < filterSize; i++) {
    this->filter[i] = this->filter[i] | filter->filter[i];
  }
}

uint64_t BloomFilter::getFilterSize() {
  return filterSize * sizeof(int);
}

void BloomFilter::fillFilter(int *intArray) {
  filter = intArray;
}