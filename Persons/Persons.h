#ifndef PERSONS_H
#define PERSONS_H
#include "../Countries/Countries.h"
#include "../VaccinationInfo/VaccinationInfo.h"
#include <iostream>
#include <string>
#include <iomanip>

using namespace std;


class Person {
  int citizenID;
  string firstName;
  string lastName;
  int age;
  string *country;

public:
  Person(int citizenID, string firstName, string lastName, string *country, int age);
  ~Person();
  int getCitizenID();
  string getFirstName();
  string getLastName();
  int getAge();
  string getCountry();
  void print();
};

class PersonNode {
  Person *person;
  VaccinationInfoList *vaccinationInfos;

public:
  PersonNode *next;
  PersonNode(Person *person, VaccinationInfo *vaccInfo);
  Person *getPerson();
  VaccinationInfo *vaccinationInfoExists(string virus);
  VaccinationInfoList *getVaccinationInfoList();
  ~PersonNode();
  void print();
};

class PersonList {
  PersonNode *head;
  int numOfPersons;

public:
  PersonList();
  ~PersonList();
  void print();
  PersonNode *createNode(Person *person, VaccinationInfo *vaccinationInfo, CountryNode *country);
  PersonNode *personExists(int citizenID);
  void populationStatus(string virusName, Date *date1, Date *date2, string country = NULL, Countries *countries = nullptr);
  void populationStatusByAge(string virusName, Date *date1, Date *date2, string country = NULL, Countries *countries = nullptr);
  void populationStatusByAgeCountrySpecific(string virusName, Date *date1, Date *date2, string country);
};



#endif