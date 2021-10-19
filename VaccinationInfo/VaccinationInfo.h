#ifndef VACCINATIONINFO_H
#define VACCINATIONINFO_H
#include "../DataStructures/Date.h"
#include <iostream>
#include <string>
using namespace std;

class VaccinationInfo {
  bool isVaccinated;
  Date *date;
  string *virus;

public:
  VaccinationInfo *next;
  VaccinationInfo(bool isVaccinated, string *virus, Date *date = nullptr);
  ~VaccinationInfo();
  bool hasBeenVaccinated();
  void print();
  void update(Date *newDate);
  string getVirus();
  Date *getDate();
};

class VaccinationInfoList {
  VaccinationInfo *head;
  int numOfNodes;

public:
  VaccinationInfoList();
  ~VaccinationInfoList();
  void addVaccinationInfo(VaccinationInfo *vaccInfo);
  void print();
  int getNumOfVaccinations();
  VaccinationInfo *vaccinationInfoExists(string virus);
  VaccinationInfo *getVaccinationByIndex(int index);
};
#endif