#ifndef REQUESTS_H
#define REQUESTS_H

class RequestNode;
class Requests;

#include "../Countries/Countries.h"
#include "../DataStructures/BloomFilter.h"
#include "../DataStructures/BloomSet.h"
#include "../DataStructures/Date.h"
#include "../DataStructures/Files.h"
#include "../DataStructures/SkipList.h"
#include "../Persons/Persons.h"
#include "../Utils/Utils.h"
#include "../Utils/monitorUtils.h"
#include "../Viruses/Viruses.h"
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

class RequestNode {
  int citizenID;
  Date *date;
  string countryFrom;
  string countryTo;
  string virus;
  bool wasAccepted;

public:
  RequestNode *next;
  RequestNode(int citizenID, Date *date, string countryFrom, string countryTo, string virus, bool wasAccepted);
  ~RequestNode();
  int getCitizenID();
  Date *getDate();
  string getCountryFrom();
  string getCountryTo();
  string getVirus();
  bool getWasAccepted();
};

class Requests {
  RequestNode *head;
  int totalRequests;
  int acceptedRequests;
  int rejectedRequests;

public:
  Requests();
  ~Requests();
  RequestNode *addRequestNode(int citizenID, Date *date, string countryFrom, string countryTo, string virus, bool wasAccepted);
  void searchRequests(Date *date1, Date *date2, string country = "");
  int getTotalRequests();
  int getAcceptedRequests();
  int getRejectedRequests();
  void addAcceptedRequest();
  void addRejectedRequest();
};
#endif