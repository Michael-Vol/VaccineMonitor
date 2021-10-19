#ifndef TRAVELMONITOR_H
#define TRAVELMONITOR_H

#define PORT 7777
#define MAXCOUNTRYLENGTH 70
#include "../DataStructures/BloomSet.h"
#include "../Requests/Requests.h"
#include "../Utils/Utils.h"
#include "../Utils/monitorUtils.h"
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/dir.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

class travelMonitor {
  int numMonitors;
  int socketBufferSize;
  int cyclicBufferSize;
  int sizeOfBloom;
  int numThreads;
  string input_dir;
  DIR *inputDIR;
  struct dirent **countriesDir;
  int *monitorPIDs;
  int *sockets;
  BloomSet *bloomSet;
  commandInfo *info;
  int numCountries;
  Requests *requests;

public:
  travelMonitor(int numMonitors, int numCountries, int socketBufferSize, int cyclicBufferSize, int sizeOfBloom, int numThreads, string input_dir, DIR *inputDIR);
  ~travelMonitor();
  int getNumMonitors();
  int getNumCountries();
  int getSocketBufferSize();
  int getCyclicBufferSize();
  int getSizeOfBloom();
  int getnumThreads();
  string getInputDir();
  DIR *getInputDIR();
  int *getMonitorPIDs();
  int getSocket(int monitorIndex);
  void setSocket(int monitorIndex, int socket);
  int forkMonitorServer(int monitorIndex, int port, int numCountries, string *paths);
  void setNumMonitors(int numMonitors);
  int numDirsPerMonitor(int monitorIndex);
  void distributeDirectories(int monitorIndex, string *countries);
  void sendStringToMonitor(int socket, string text);
  string getStringFromMonitor(int socket);
  void sendIntToMonitor(int socket, int number);
  int getIntFromMonitor(int socket);
  void setCountriesDir(struct dirent **countriesDir);
  void getBloomFilters(int socket);
  void processCommand();
  RequestNode *addRequestNode(int citizenID, Date *date, string countryFrom, string countryTo, string virus, bool wasAccepted);
  int getMonitorIDByCountry(string country);
  int getTotalRequests();
  int getAcceptedRequests();
  int getRejectedRequests();
  void addAcceptedRequest();
  void addRejectedRequest();
  void searchRequests(Date *date1, Date *date2, string country = "");
};

#endif