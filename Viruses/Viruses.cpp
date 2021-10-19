#include "Viruses.h"
#include "../Utils/Hash.h"
#include <iostream>
#include <string>

using namespace std;

Viruses::Viruses() {
  this->head = nullptr;
  this->numOfViruses = 0;
}

string *Viruses::addVirus(string virus, int filterSize, int citizenID) {
  string *virusPtr = getVirusPtr(virus);
  BloomFilter *bloomFilter;
  if (virusPtr == nullptr) { //virus doesn't exist in list
    VirusNode *currentNode = head;
    while (currentNode && currentNode->next)
      currentNode = currentNode->next;

    //create new node
    VirusNode *newNode;

    if (!head) {
      newNode = head = new VirusNode(virus, filterSize);
    } else {
      newNode = currentNode->next = new VirusNode(virus, filterSize);
    }
    numOfViruses++;
    virusPtr = newNode->getVirusPtr();
  }

  return virusPtr; //return ptr of virus already in list
}
void Viruses::print() {
  VirusNode *currentNode = head;
  cout << "OK" << endl;
  while (currentNode) {
    currentNode->print();
    currentNode = currentNode->next;
  }
}
string *Viruses::getVirusPtr(string virus) {
  VirusNode *currentNode = head;
  while (currentNode) {
    string *virusPtr = currentNode->getVirusPtr();
    if (*virusPtr == virus) {
      return virusPtr;
    }
    currentNode = currentNode->next;
  }
  return nullptr;
}
VirusNode *Viruses::getVirusNode(string virus) {
  VirusNode *currentNode = head;
  while (currentNode) {
    string *virusPtr = currentNode->getVirusPtr();
    if (*virusPtr == virus) {
      return currentNode;
    }
    currentNode = currentNode->next;
  }
  return nullptr;
}

BloomFilter *Viruses::getBloomFilterPtr(string virus) {
  VirusNode *currentNode = head;
  while (currentNode) {
    string *virusPtr = currentNode->getVirusPtr();
    if (*virusPtr == virus) {
      return currentNode->getBloomFilter();
    }
    currentNode = currentNode->next;
  }
  return nullptr;
}
SkipList *Viruses::getVaccinatedSkipList(string virus) {
  VirusNode *currentNode = head;
  while (currentNode) {
    string *virusPtr = currentNode->getVirusPtr();
    if (*virusPtr == virus) {
      return currentNode->getVaccinatedSkipList();
    }
    currentNode = currentNode->next;
  }
  return nullptr;
}
SkipList *Viruses::getNotVaccinatedSkipList(string virus) {
  VirusNode *currentNode = head;
  while (currentNode) {
    string *virusPtr = currentNode->getVirusPtr();
    if (*virusPtr == virus) {
      return currentNode->getNotVaccinatedSkipList();
    }
    currentNode = currentNode->next;
  }
  return nullptr;
}
void Viruses::checkAllSkipLists(int citizenID, PersonNode *personNode) {
  VirusNode *currentNode = head;
  while (currentNode) {
    cout << *currentNode->getVirusPtr() << " ";
    if (currentNode->getSkipListStatus(citizenID)) {
      cout << "YES";
      personNode->vaccinationInfoExists(*(currentNode->getVirusPtr()))->getDate()->print();
    } else {
      cout << "NO" << endl;
    }
    currentNode = currentNode->next;
  }
}

Viruses::~Viruses() {
  VirusNode *current = head;
  VirusNode *next;
  while (current) {
    next = current->next;
    delete current;
    current = next;
  }
}
VirusNode::VirusNode(string virus, int filterSize) {
  this->virus = virus;
  this->next = nullptr;
  bloomFilter = new BloomFilter(filterSize);
  vaccinatedSkipList = new SkipList();
  notVaccinatedSkipList = new SkipList();
}
VirusNode::~VirusNode() {
  delete bloomFilter;
  delete vaccinatedSkipList;
  delete notVaccinatedSkipList;
}
void VirusNode::print() {
  if (next) {
    cout << virus << " -> ";
  } else {
    cout << virus << endl;
  }
}
BloomFilter *VirusNode::getBloomFilter() {
  return bloomFilter;
}
SkipList *VirusNode::getVaccinatedSkipList() {
  return vaccinatedSkipList;
};
SkipList *VirusNode::getNotVaccinatedSkipList() {
  return notVaccinatedSkipList;
};
void VirusNode::insertToBloomFilter(int citizenID) {
  bloomFilter->insert(citizenID);
}
bool VirusNode::getBloomFilterStatus(int citizenID) {
  if (bloomFilter->check(citizenID)) {
    return true;
  }
  return false;
}

void VirusNode::insertToVaccinatedSkipList(Person *person) {
  vaccinatedSkipList->insertNode(person);
}
void VirusNode::insertToNotVaccinatedSkipList(Person *person) {
  notVaccinatedSkipList->insertNode(person);
}
bool VirusNode::getSkipListStatus(int citizenID) {
  return vaccinatedSkipList->check(citizenID);
}

string *VirusNode::getVirusPtr() {
  return &virus;
}

void Viruses::createBloomSet(BloomSet *bloomSet) {
  VirusNode *current = head;
  int count = 0;
  while (current) {
    bloomSet->addBloomNode(current->getBloomFilter(), *(current->getVirusPtr()));
    current = current->next;
    count++;
  }
}