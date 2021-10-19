#include "VaccinationInfo.h"
#include <iostream>
#include <string>

using namespace std;

VaccinationInfo::VaccinationInfo(bool isVaccinated, string *virus, Date *date) {
  this->isVaccinated = isVaccinated;
  this->virus = virus;
  if (isVaccinated) {
    this->date = date;
  } else {
    this->date = nullptr;
  }
  next = nullptr;
}

VaccinationInfo::~VaccinationInfo() {
  if (date) {
    delete date;
  }
}
void VaccinationInfo::update(Date *newDate) {
  isVaccinated = true;
  date = newDate;
}

void VaccinationInfo::print() {
  if (isVaccinated) {
    cout << *virus << " YES ";
    date->print();
  }
  cout << *virus << " NO" << endl;
}

string VaccinationInfo::getVirus() {
  return *virus;
}
bool VaccinationInfo::hasBeenVaccinated() {
  return isVaccinated;
}

Date *VaccinationInfo::getDate() {
  return date;
}

VaccinationInfoList::VaccinationInfoList() {

  head = nullptr;
  numOfNodes = 0;
}

VaccinationInfoList::~VaccinationInfoList() {
  VaccinationInfo *current = head;
  VaccinationInfo *next;
  while (current) {
    next = current->next;
    delete current;
    current = next;
  }
}
void VaccinationInfoList::print() {
  VaccinationInfo *current = head;
  while (current) {
    current->print();
    current = current->next;
  }
}
int VaccinationInfoList::getNumOfVaccinations() {
  return numOfNodes;
}
void VaccinationInfoList::addVaccinationInfo(VaccinationInfo *vaccInfo) {
  VaccinationInfo *current = head;
  while (current && current->next) {
    current = current->next;
  }
  if (!head) {
    current = head = vaccInfo;
  } else {
    current->next = vaccInfo;
  }
  numOfNodes++;
}

VaccinationInfo *VaccinationInfoList::vaccinationInfoExists(string virus) {
  VaccinationInfo *current = head;
  while (current) {
    if (current->getVirus() == virus) {
      return current;
    }
    current = current->next;
  }
  return nullptr;
}
VaccinationInfo *VaccinationInfoList::getVaccinationByIndex(int index) {
  VaccinationInfo *current = head;
  int counter = 0;
  while (current) {
    if (counter == index) {
      return current;
    }
    current = current->next;
    counter++;
  }
  return nullptr;
}
