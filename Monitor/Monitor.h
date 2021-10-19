#ifndef MONITOR_H
#define MONITOR_H

#include "../Countries/Countries.h"
#include "../DataStructures/BloomSet.h"
#include "../DataStructures/Files.h"
#include "../Persons/Persons.h"
#include "../Utils/Utils.h"
#include "../Viruses/Viruses.h"
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;
class Monitor {
  int monitorID;
  PersonList *personsList;
  Viruses *viruses;
  Countries *countries;
  BloomSet *bloomset;
  Files *files;
  int bloomSize;
  string *dirs;
  int num;
  int numDirs;
  int socketBufferSize;
  int cyclicBufferSize;
  int port;
  int numThreads;
  int socket;
  int listeningSocket;
  pthread_t *threadIDs;
  int totalRequests;
  int acceptedRequests;
  int rejectedRequests;

public:
  Monitor(int monitorID, int bloomFilterSize, int socketBufferSize, int cyclicBufferSize, int numCountries, string *dirs, int port, int numThreads);
  ~Monitor();
  void addCountry(string country);
  void parseLine(string line);
  uint64_t calculateSetSize();
  Countries *getCountries();
  uint64_t getBloomNodeSize(int nodeIndex);
  int getNumOfBloomNodes();
  string getVirusName(int nodeIndex);
  BloomSet *createBloomSet();
  BloomNode *getBloomNode(int nodeIndex);
  File *addFile(string fileName);
  void printFiles();
  string getCountryByIndex(int countryIndex);
  void addDir(string countryName);
  void setListeningSocket(int socket);
  void setSocket(int socket);
  void addThreadID(int threadIndex, pthread_t threadID);
  pthread_t getThreadId(int threadIndex);
  int countAllPaths();
  void getAllPaths(string *paths);
  string *getDirs();
  int getNumDirs();
  File *checkFileExistance(string file);
  void sendBloomSet();
  bool isVaccinated(int citizenID, string virus);
  PersonNode *getPerson(int citzenID);
  string getVaccinationDate(int citizenID, string virus);
  void sendStringToTravelMonitor(int listeningSocket, int socket, string text);
  string getStringFromTravelMonitor(int listeningSocket, int socket);
  void sendIntToTravelMonitor(int listeningSocket, int socket, int number);
  int getIntFromTravelMonitor(int listeningSocket, int socket);
  int getTotalRequests();
  int getAcceptedRequests();
  int getRejectedRequests();
  void addAcceptedRequest();
  void addRejectedRequest();
};

#endif