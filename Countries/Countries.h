#ifndef COUNTRIES_H
#define COUNTRIES_H
#define NUM_OF_COUNTRIES 197

#include <iostream>
#include <string>

using namespace std;

class CountryNode {
  string country;
  int population;

public:
  CountryNode *next;
  CountryNode(string country);
  ~CountryNode();
  string *getCountryPtr();
  int getPopulation();
  void addToPopulation();
};

class Countries {
  CountryNode *head;
  int numOfCountries;

public:
  Countries();
  ~Countries();
  CountryNode *getHead();
  void print();
  string *getCountryPtr(string country);
  CountryNode *getCountry(string country);
  string *addCountry(string country);
  string getCountryByIndex(int countryIndex);
  int getNumOfCountries();
};

#endif