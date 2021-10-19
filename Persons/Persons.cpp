#include "Persons.h"
#include <iostream>
#include <string>

Person::Person(int citizenID, string firstName, string lastName, string *country, int age) {
  this->citizenID = citizenID;
  this->firstName = firstName;
  this->lastName = lastName;
  this->age = age;
  this->country = country;
}
int Person::getCitizenID() {
  return citizenID;
}
string Person::getFirstName() {
  return firstName;
}
string Person::getLastName() {
  return lastName;
}
int Person::getAge() {
  return age;
}
string Person::getCountry() {
  return *country;
}
Person::~Person() {
}
void Person::print() {
  cout << citizenID << " " << firstName << " " << lastName << " " << *country << " " << age << endl;
}



PersonList::PersonList() {
  numOfPersons = 0;
  head = nullptr;
}
PersonNode *PersonList::createNode(Person *person, VaccinationInfo *vaccinationInfo, CountryNode *country) {
  //get to end of list
  PersonNode *currentNode;
  if ((currentNode = personExists(person->getCitizenID()))) {
    //only add vaccination info to vaccination info list
    currentNode->getVaccinationInfoList()->addVaccinationInfo(vaccinationInfo);
    //  currentNode->print();
    return currentNode;
  }
  currentNode = head;
  while (currentNode && currentNode->next)
    currentNode = currentNode->next;

  //create new node
  PersonNode *newNode;
  if (!head) {
    newNode = head = new PersonNode(person, vaccinationInfo);
  } else {
    newNode = currentNode->next = new PersonNode(person, vaccinationInfo);
  }

  //newNode->print();
  numOfPersons++;
  //add to population
  country->addToPopulation();
  return newNode;
}

PersonList::~PersonList() {
  PersonNode *current = head;
  PersonNode *next;
  while (current) {
    next = current->next;
    delete current;
    current = next;
  }
}
PersonNode *PersonList::personExists(int citizenID) {
  PersonNode *currrentNode = head;
  while (currrentNode) {
    if (currrentNode->getPerson()->getCitizenID() == citizenID) {
      return currrentNode;
    }
    currrentNode = currrentNode->next;
  }
  return nullptr;
}

void PersonList::print() {
  PersonNode *currrentNode = head;
  while (currrentNode) {
    currrentNode->print();
    currrentNode = currrentNode->next;
  }
}
PersonNode::PersonNode(Person *person, VaccinationInfo *vaccInfo) {
  this->person = person;
  this->vaccinationInfos = new VaccinationInfoList();
  vaccinationInfos->addVaccinationInfo(vaccInfo);
  this->next = nullptr;
}
PersonNode::~PersonNode() {
  delete person;
  delete vaccinationInfos;
}

void PersonNode::print() {
  person->print();
  //cout << "VACCINATIONS : " << endl;
  //vaccinationInfos->print();
  cout << endl;
}

Person *PersonNode::getPerson() {
  return person;
}
VaccinationInfoList *PersonNode::getVaccinationInfoList() {
  return vaccinationInfos;
}

VaccinationInfo *PersonNode::vaccinationInfoExists(string virus) {
  return getVaccinationInfoList()->vaccinationInfoExists(virus);
}

void PersonList::populationStatus(string virusName, Date *date1, Date *date2, string country, Countries *countries) {
  PersonNode *currentNode = head;
  VaccinationInfo *vaccInfo;
  int population = 0;
  int personsVaccinated = 0;
  if (country != "") {
    //if country has been provided:  argsList = country,virusName,date1,date2
    while (currentNode) {
      if (currentNode->getPerson()->getCountry() == country) {
        population++;
        vaccInfo = currentNode->vaccinationInfoExists(virusName);
        if (vaccInfo && vaccInfo->hasBeenVaccinated() && vaccInfo->getDate()->isBetweenLimits(date1, date2)) {
          //if person has been vaccinated for the given virus and date is between limits
          personsVaccinated++;
        }
      }
      currentNode = currentNode->next;
    }
    cout << country << " " << population << " " << fixed << setprecision(2) << ((float)personsVaccinated / (float)population) * 100 << "% " << endl;

  } else {
    //if country has not been provided:  argsList = virusName,date1,date2
    CountryNode *currentCountry = countries->getHead();
    while (currentCountry) {
      currentNode = head;
      personsVaccinated = 0;
      string countryName = *(currentCountry->getCountryPtr());
      int population = currentCountry->getPopulation();
      while (currentNode) {
        if (currentNode->getPerson()->getCountry() == countryName) {
          vaccInfo = currentNode->vaccinationInfoExists(virusName);
          if (vaccInfo && vaccInfo->hasBeenVaccinated() && vaccInfo->getDate()->isBetweenLimits(date1, date2)) {
            personsVaccinated++;
          }
        }
        currentNode = currentNode->next;
      }
      cout << countryName << " " << personsVaccinated << " " << fixed << setprecision(2) << ((float)personsVaccinated / (float)population) * 100 << "% " << endl;
      currentCountry = currentCountry->next;
    }
  }
}

void PersonList::populationStatusByAge(string virusName, Date *date1, Date *date2, string country, Countries *countries) {
  PersonNode *currentNode = head;
  VaccinationInfo *vaccInfo;
  Person *person;
  int currentAgeLimit = 0;
  if (country != "") {
    //if country has been provided : argsList =  country,virusName,date1,date2
    populationStatusByAgeCountrySpecific(virusName, date1, date2, country);
  } else {
    //if country has not been provided:  argsList = virusName,date1,date2
    CountryNode *currentCountry = countries->getHead();
    while (currentCountry) {
      country = *(currentCountry->getCountryPtr());
      populationStatusByAgeCountrySpecific(virusName, date1, date2, country);
      cout << endl;
      currentCountry = currentCountry->next;
    }
  }
}

void PersonList::populationStatusByAgeCountrySpecific(string virusName, Date *date1, Date *date2, string country) {
  PersonNode *currentNode = head;
  VaccinationInfo *vaccInfo;
  Person *person;
  int *populationPerAge = new int[4];
  int *vaccinationsPerAge = new int[4];
  int currentAgeLimit = 0;
  currentAgeLimit = 0;

  cout << country << endl;

  for (int i = 0; i < 4; i++) {
    currentAgeLimit += 20;
    populationPerAge[i] = 0;
    vaccinationsPerAge[i] = 0;
    currentNode = head;
    while (currentNode) {
      person = currentNode->getPerson();
      bool passedCountry = (person->getCountry() == country);
      bool passedAge = (person->getAge() < currentAgeLimit && person->getAge() >= (currentAgeLimit - 20)) || (person->getAge() > currentAgeLimit && currentAgeLimit == 80);
      if (passedCountry && passedAge) {
        populationPerAge[i]++;
        vaccInfo = currentNode->vaccinationInfoExists(virusName);
        if (vaccInfo && vaccInfo->hasBeenVaccinated() && vaccInfo->getDate()->isBetweenLimits(date1, date2)) {
          vaccinationsPerAge[i]++;
        }
      }
      currentNode = currentNode->next;
    }
    cout << currentAgeLimit - 20 << "-" << currentAgeLimit << " " << vaccinationsPerAge[i] << " " << fixed << setprecision(2) << ((float)vaccinationsPerAge[i] / (float)populationPerAge[i]) * 100 << "% " << endl;
  }
  delete[] populationPerAge;
  delete[] vaccinationsPerAge;
}
