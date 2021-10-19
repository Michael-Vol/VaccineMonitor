#include "Requests.h"
using namespace std;

RequestNode::RequestNode(int citizenID, Date *date, string countryFrom, string countryTo, string virus, bool wasAccepted) {
  this->citizenID = citizenID;
  this->date = date;
  this->countryFrom = countryFrom;
  this->countryTo = countryTo;
  this->virus = virus;
  this->wasAccepted = wasAccepted;
  this->next = nullptr;
}

RequestNode::~RequestNode() {
  delete date;
}
bool RequestNode::getWasAccepted() {
  return wasAccepted;
}
int RequestNode::getCitizenID() {
  return citizenID;
}
Date *RequestNode::getDate() {
  return date;
}

string RequestNode::getCountryFrom() {
  return countryFrom;
}

string RequestNode::getCountryTo() {
  return countryTo;
}
string RequestNode::getVirus() {
  return virus;
}

Requests::Requests() {
  head = nullptr;
  totalRequests = acceptedRequests = rejectedRequests = 0;
}
Requests::~Requests() {
}
RequestNode *Requests::addRequestNode(int citizenID, Date *date, string countryFrom, string countryTo, string virus, bool wasAccepted) {
  RequestNode *current = head;
  RequestNode *newNode;
  while (current && current->next) {
    current = current->next;
  }
  if (head == nullptr) {
    newNode = head = new RequestNode(citizenID, date, countryFrom, countryTo, virus, wasAccepted);
  } else {
    newNode = current->next = new RequestNode(citizenID, date, countryFrom, countryTo, virus, wasAccepted);
  }
  wasAccepted ? acceptedRequests++ : rejectedRequests++;
  totalRequests++;
  return newNode;
}
int Requests::getTotalRequests() {
  return totalRequests;
}
int Requests::getAcceptedRequests() {
  return acceptedRequests;
}
int Requests::getRejectedRequests() {
  return rejectedRequests;
}
void Requests::addAcceptedRequest() {
  acceptedRequests++;
  totalRequests++;
}
void Requests::addRejectedRequest() {
  rejectedRequests++;
  totalRequests++;
}
void Requests::searchRequests(Date *date1, Date *date2, string country) {
  int totalRequests = 0;
  int acceptedRequests = 0;
  int rejectedRequests = 0;
  RequestNode *current = head;
  Date *currentDate;

  while (current) {
    currentDate = current->getDate();
    if (currentDate->isBetweenLimits(date1, date2)) {
      if (country == "" || current->getCountryTo() == country) {
        current->getWasAccepted() ? acceptedRequests++ : rejectedRequests++;
        totalRequests++;
      }
    }
    current = current->next;
  }
  cout << "\033[1;34m TOTAL REQUESTS " << totalRequests << " \033[0m" << endl;
  cout << "\033[1;32m ACCEPTED " << acceptedRequests << " \033[0m" << endl;
  cout << "\033[1;31m REJECTED " << rejectedRequests << " \033[0m" << endl;
}
