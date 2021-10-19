#ifndef VIRUSES_H
#define VIRUSES_H
#include "../DataStructures/BloomFilter.h"
#include "../DataStructures/BloomSet.h"
#include "../DataStructures/SkipList.h"
#include "../Persons/Persons.h"
#include "iostream"
#include "string"

using namespace std;

class VirusNode {
  string virus;
  BloomFilter *bloomFilter;
  SkipList *vaccinatedSkipList;
  SkipList *notVaccinatedSkipList;

public:
  VirusNode *next;
  VirusNode(string virus, int filterSize);
  ~VirusNode();
  string *getVirusPtr();
  void insertToBloomFilter(int citizenID);
  bool getBloomFilterStatus(int citizenID);
  void insertToVaccinatedSkipList(Person *person);
  void insertToNotVaccinatedSkipList(Person *person);
  bool getSkipListStatus(int citizenID);
  BloomFilter *getBloomFilter();
  SkipList *getVaccinatedSkipList();
  SkipList *getNotVaccinatedSkipList();
  void print();
};
class Viruses {
  VirusNode *head;
  int numOfViruses;

public:
  Viruses();
  ~Viruses();
  string *addVirus(string virus, int filterSize, int citizenID);
  void checkAllSkipLists(int citizenID, PersonNode *personNode);
  string *getVirusPtr(string virus);
  VirusNode *getVirusNode(string virus);
  BloomFilter *getBloomFilterPtr(string virus);
  void createBloomSet(BloomSet *bloomSet);
  SkipList *getVaccinatedSkipList(string virus);
  SkipList *getNotVaccinatedSkipList(string virus);
  void print();
};

#endif