#include "Countries.h"
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

Countries::Countries() {
  this->head = nullptr;
  this->numOfCountries = 0;
}

string *Countries::addCountry(string country) {
  string *CountryPtr = getCountryPtr(country);
  if (CountryPtr == nullptr) { //country doesn't exist in list
    CountryNode *currentNode = head;
    while (currentNode && currentNode->next)
      currentNode = currentNode->next;

    //create new node
    CountryNode *newNode;
    if (!head) {
      newNode = head = new CountryNode(country);
    } else {
      newNode = currentNode->next = new CountryNode(country);
    }
    numOfCountries++;
    return newNode->getCountryPtr();
  }
  return CountryPtr; //return ptr of country already in list
}

string *Countries::getCountryPtr(string country) {
  CountryNode *currentNode = head;
  while (currentNode) {
    string *countryPtr = currentNode->getCountryPtr();
    if (*countryPtr == country) {
      return countryPtr;
    }
    currentNode = currentNode->next;
  }
  return nullptr;
}
CountryNode *Countries::getHead() {
  return head;
}

CountryNode *Countries::getCountry(string country) {
  CountryNode *currentNode = head;
  while (currentNode) {
    if (*(currentNode->getCountryPtr()) == country) {
      return currentNode;
    }
    currentNode = currentNode->next;
  }
  return nullptr;
}
int Countries::getNumOfCountries() {
  return numOfCountries;
}
void Countries::print() {
  CountryNode *currentNode = head;
  while (currentNode) {
    cout << *(currentNode->getCountryPtr()) << endl;
    currentNode = currentNode->next;
  }
}
Countries::~Countries() {
  CountryNode *current = head;
  CountryNode *next;
  while (current) {
    next = current->next;
    delete current;
    current = next;
  }
}
string Countries::getCountryByIndex(int countryIndex) {
  CountryNode *current = head;
  int counter = 0;
  while (current) {
    if (countryIndex == counter) {
      return *(current->getCountryPtr());
    }
    current = current->next;
    counter++;
  }
  return "";
}

CountryNode::CountryNode(string country) {
  this->country = country;
  this->population = 0;
  this->next = nullptr;
}
CountryNode::~CountryNode() {
}
string *CountryNode::getCountryPtr() {
  return &country;
}
int CountryNode::getPopulation() {
  return population;
}

void CountryNode::addToPopulation() {
  population++;
}
