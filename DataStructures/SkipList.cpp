#include "SkipList.h"
#include <iostream>
#include <string>

using namespace std;

SkipList::SkipList(int maxLevel) {
  levels = new SkipListLevel *[maxLevel];
  for (int i = 0; i < maxLevel; i++) {
    levels[i] = new SkipListLevel(i);
  }

  currentLevels = 0;
  levels[0] = new SkipListLevel(0);
  this->maxLevel = maxLevel;
}

SkipList::~SkipList() {
  for (int i = 0; i < maxLevel; i++) {
    delete levels[i];
  }
  delete[] levels;
}

void SkipList::insertNode(Person *person) {
  levels[0]->insertNode(person, maxLevel, levels, nullptr);
}
void SkipList::print() {
  int i = 0;
  SkipListLevel *current = levels[i];
  while (current->getHead()) {
    current->print();
    current = levels[++i];
  }
}
void SkipList::printAll() {
  levels[0]->print();
}

bool SkipList::check(int citizenID) {
  int upperLevelNum = maxLevel;
  SkipListLevel *upperLevel = levels[maxLevel - 1];
  while (upperLevel->getHead() == nullptr) {
    if(upperLevelNum == 0) break;
    upperLevel = levels[--upperLevelNum];
  }
  return upperLevel->check(citizenID, levels);
}
void SkipList::remove(int citizenID) {
  SkipListLevel *currLevel;
  for (int i = 0; i < maxLevel; i++) {
    currLevel = levels[i];
    if (currLevel->getHead()) {
      currLevel->remove(citizenID);
    }
  }
}

SkipListLevel::SkipListLevel(int level) {
  this->head = nullptr;
  this->numOfNodes = 0;
  this->level = level;
}

SkipListLevel::~SkipListLevel() {
  SkipListNode *current = head;
  SkipListNode *next;
  while (current) {
    next = current->next;
    delete current;
    current = next;
  }
}

bool SkipListLevel::check(int citizenID, SkipListLevel **levels, SkipListNode *startNode) {
  if(level == 0 && head == nullptr) return false; //empty skip list,virus doesn't exist
  if (level >= 0) {
    //get startnode
    SkipListNode *current = startNode;
    if (!startNode) {
      current = head; //if startnode doesn't exist, start from head of list
    }

    while (current && current->next) {
      if (current->getCitizenID() == citizenID) {
        return true; //exists in skiplist
      }
      if (current->next->getCitizenID() > citizenID) { //if next citizenID is bigger than key get to lower level
        if (level > 0) {
          return levels[level - 1]->check(citizenID, levels, current->down);
        }
      }
      current = current->next;
    }
    if (current->getCitizenID() == citizenID) { //check if last node is same as key
      return true;                              //exists in skiplist
    }
    //reached end of list get to lower level
    if (level > 0) {
      return levels[level - 1]->check(citizenID, levels, current->down);
    }  
    return false; // virus doesn't exist
  }
  return false;
}

void SkipListLevel::insertNode(Person *person, int maxLevel, SkipListLevel **levels, SkipListNode *downNode) {
  if (level < maxLevel - 1) {
    numOfNodes++;
    int citizenID = person->getCitizenID();
    SkipListNode *newNode = nullptr;
    SkipListNode *oldNext;
    SkipListNode *current = head;
    while (current && current->next) {
      if (current == head && head->getCitizenID() > citizenID) {
        oldNext = head;
        head = new SkipListNode(person);
        newNode = head;
        newNode->next = oldNext;
        if (downNode) {
          newNode->down = downNode;
        }
        if (levelUp() && level < maxLevel - 1) {
          //insert node to upper level
          levels[level + 1]->insertNode(person, maxLevel, levels, newNode);
        }
        return;
      } else if (current->next->getCitizenID() > citizenID) {
        oldNext = current->next;
        current->next = new SkipListNode(person);
        newNode = current->next;
        newNode->next = oldNext;
        if (downNode) {
          newNode->down = downNode;
        }
        if (levelUp() && level < maxLevel - 1) {
          //insert node to upper level
          levels[level + 1]->insertNode(person, maxLevel, levels, newNode);
        }
        return;
      }
      current = current->next;
    }
    if (!head) {
      //no node exists, create first node
      head = new SkipListNode(person);
      newNode = head;
      newNode->next = nullptr;
    } else if (!(head->next) && head->getCitizenID() > citizenID) {
      //only one node exists and new nodes needs to be the head
      SkipListNode *oldHead = head;
      head = new SkipListNode(person);
      newNode = head;
      head->next = oldHead;
    } else {
      newNode = current->next = new SkipListNode(person);
      newNode->next = nullptr;
    }
    if (downNode) {
      newNode->down = downNode;
    }
    if (levelUp() && level < maxLevel - 1) {
      //insert node to upper level
      levels[level + 1]->insertNode(person, maxLevel, levels, newNode);
    }
    return;
  }
}
void SkipListLevel::print() {
  cout << "Level " << level << endl;
  SkipListNode *current = head;
  while (current) {
    current->print();
    current = current->next;
  }
}
SkipListNode::SkipListNode(Person *person) {
  citizenID = person->getCitizenID();
  this->person = person;
}
SkipListNode *SkipListLevel::getHead() {
  return head;
}
SkipListNode::~SkipListNode() {
  
}
void SkipListNode::print() {
  person->print();
  if (!next) {
    cout << endl;
  }
}
int SkipListNode::getCitizenID() {
  return citizenID;
}

void SkipListLevel::remove(int citizenID) {
  SkipListNode *current = head;
  SkipListNode *toDelete;
  SkipListNode *next = nullptr;
  while (current && current->next) {
    if (current->getCitizenID() == citizenID) {
      toDelete = current;
      next = current->next;
      delete toDelete;
      head = next;
      return;
    } else if (current->next->getCitizenID() == citizenID) {
      toDelete = current->next;
      next = toDelete->next;
      delete toDelete;
      current->next = next;
      return;
    }
    current = current->next;
  }
  if (current && current->getCitizenID() == citizenID) {
    delete current;
    head = nullptr;
  }
}