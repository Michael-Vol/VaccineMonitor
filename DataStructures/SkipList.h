#ifndef SKIPLIST_H
#define SKIPLIST_H
#define K 10000
#define MAXLEVEL 9
#include "../Persons/Persons.h"
#include "../Utils/monitorUtils.h"
#include <iostream>
#include <string>
using namespace std;

class SkipListNode {
  int citizenID;
  Person *person;

public:
  SkipListNode *next;
  SkipListNode *down;
  SkipListNode(Person *person);
  ~SkipListNode();
  int getCitizenID();
  void print();
};
class SkipListLevel {
  SkipListNode *head;
  int numOfNodes;
  int level;

public:
  SkipListLevel(int level);
  void insertNode(Person *person, int maxLevel, SkipListLevel **levels, SkipListNode *downNode = nullptr);
  ~SkipListLevel();
  SkipListNode *getHead();
  bool check(int citizenID, SkipListLevel **levels, SkipListNode *startNode = nullptr);
  void remove(int citizenID);
  void print();
};
class SkipList {
  SkipListLevel **levels;
  int currentLevels;
  int maxLevel;

public:
  SkipList(int maxLevel = MAXLEVEL);
  void insertNode(Person *person);
  bool check(int citizenID);
  void remove(int citizenID);
  ~SkipList();
  void print();
  void printAll();
};
#endif