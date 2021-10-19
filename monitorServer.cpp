#include "Monitor/Monitor.h"
#include "PathBuffer/pathBuffer.h"
#include "Utils/Utils.h"
#include "fcntl.h"
#include "travelMonitor/travelMonitor.h"
#include <arpa/inet.h>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;
int main(int argc, char **argv) {
  //check arguments from travelMonitor
  checkArgsMonitor(argc, argv);
  int port = atoi(argv[0]);
  int numThreads = atoi(argv[1]);
  int socketBufferSize = atoi(argv[2]);
  int cyclicBufferSize = atoi(argv[3]);
  int sizeOfBloom = atoi(argv[4]);
  int numDirs = argc - 5;
  int monitorIndex = port - PORT;
  string dirs[numDirs];
  for (int i = 0; i < numDirs; i++) {
    string directory(argv[5 + i]);
    dirs[i] = directory;
  }
  Monitor *monitor = new Monitor(monitorIndex, sizeOfBloom, socketBufferSize, cyclicBufferSize, numDirs, dirs, port, numThreads);
  //Create Socket Structs
  struct sockaddr_in monitorServer, travelClient;
  int sock, newsock;
  struct sockaddr *monitorServerPtr = (struct sockaddr *)&monitorServer;
  struct sockaddr *travelClientPtr = (struct sockaddr *)&travelClient;
  struct hostent *monitorServerHost;
  socklen_t clientlen = sizeof(travelClientPtr);

  //Create Socket and Set Reusable Address
  sock = createSocket();

  monitor->setListeningSocket(sock);
  //Get MonitorServer Address(localhost)
  char hostname[1024];
  hostname[1023] = '\0';

  gethostname(hostname, sizeof(hostname));
  monitorServerHost = gethostbyname(hostname);
  monitorServer.sin_family = AF_INET;
  memcpy(&monitorServer.sin_addr, monitorServerHost->h_addr, monitorServerHost->h_length);
  monitorServer.sin_port = htons(port);

  //Create and fill path Buffer
  int numPaths = monitor->countAllPaths();
  string *paths = new string[numPaths];
  monitor->getAllPaths(paths);

  //Check if cyclicBufferSize is more than needed
  if (cyclicBufferSize > numPaths) {
    cyclicBufferSize = numPaths;
  }

  pathBuffer *buffer = new pathBuffer(cyclicBufferSize, paths, numPaths);

  //Create threads argsStruct
  threadArgs *thrArgs = (threadArgs *)malloc(sizeof(threadArgs));
  thrArgs->monitor = monitor;
  thrArgs->buffer = buffer;
  thrArgs->numPaths = numPaths;
  thrArgs->paths = paths;

  //Create Threads

  for (int i = 0; i < numThreads; i++) {
    pthread_t thr = createThread(thrArgs);
    monitor->addThreadID(i, thr);
  }
  fstream file;
  string line;
  int pathsIndex = 0;
  int pathsLeft = numPaths;
  do {
    buffer->placeNewPath(paths[pathsIndex]);
    pathsIndex++;
  } while (pathsIndex < numPaths);
  while (!buffer->terminateMainThread()) //wait for all threads to process files
    ;

  bindSocket(sock, monitorServerPtr);
  listenSocket(sock, 1);
  cout << "\033[1;33m -> Monitor Server " << monitorIndex << " listening for connections on port " << port << " \033[0m" << endl;
  newsock = acceptSocket(sock, travelClientPtr, clientlen);
  monitor->setSocket(newsock);

  //Create BloomSet
  BloomSet *bloomSet = monitor->createBloomSet();

  //Send BloomSet
  monitor->sendBloomSet();
  while (monitor->getStringFromTravelMonitor(sock, newsock) != "All Blooms Transfered")
    ;

  //process commands

  while (true) {
    int commandCode = monitor->getIntFromTravelMonitor(sock, newsock);

    switch (commandCode) {
    case 1: {
      int citizenID = monitor->getIntFromTravelMonitor(sock, newsock);
      string virus = monitor->getStringFromTravelMonitor(sock, newsock);
      if (monitor->isVaccinated(citizenID, virus)) {
        monitor->sendStringToTravelMonitor(sock, newsock, "YES");
        string date = monitor->getVaccinationDate(citizenID, virus);
        monitor->sendStringToTravelMonitor(sock, newsock, date);
      } else {
        monitor->sendStringToTravelMonitor(sock, newsock, "NO");
      }
      string RequestWasAccepted = monitor->getStringFromTravelMonitor(sock, newsock);
      RequestWasAccepted == "ACCEPTED" ? monitor->addAcceptedRequest() : monitor->addRejectedRequest();
      break;
    }
    case 3: {
      string countryName = monitor->getStringFromTravelMonitor(sock, newsock);
      DIR *countryDIR = opendir(countryName.c_str());
      struct dirent *country;
      country = readdir(countryDIR);
      Files *newFiles = new Files();

      int numOfNewFiles = 0;
      while (!strcmp(country->d_name, ".") || !strcmp(country->d_name, "..")) //ignore current and parent directory names
        country = readdir(countryDIR);

      while (country != NULL) {
        if (country->d_type == DT_REG) {
          string filename(country->d_name);
          File *file = monitor->checkFileExistance(filename);
          if (monitor->checkFileExistance(filename) == nullptr) {
            newFiles->addFile(filename);
            numOfNewFiles++;
          }
        }
        country = readdir(countryDIR);
      }
      rewinddir(countryDIR);
      if (numOfNewFiles > 0) { //new files have been added
        monitor->sendIntToTravelMonitor(sock, newsock, 1);
        string *files = new string[numOfNewFiles];
        newFiles->getAllFiles(files);
        for (int i = 0; i < numOfNewFiles; i++) {
          cout << "\033[1;32m Found new  file " << files[i] << "\033[0m " << endl;
          monitor->addFile(files[i]);
        }
        buffer->resetBuffer(numOfNewFiles);
        int pathsIndex = 0;
        do {
          buffer->placeNewPath(files[pathsIndex]);
          pathsIndex++;
        } while (pathsIndex < numOfNewFiles);
        while (!buffer->terminateMainThread()) //wait for all threads sto process files
          ;
        //Send BloomSet
        monitor->sendBloomSet();
        while (monitor->getStringFromTravelMonitor(sock, newsock) != "All Blooms Transfered")
          ;
        delete[] files;

      } else {
        //notify client not to expect updated bloom filters
        monitor->sendIntToTravelMonitor(sock, newsock, 0);
      }
      delete newFiles;
      closeDir(countryDIR);
      break;
    }
    case 4: {
      int citizenID = monitor->getIntFromTravelMonitor(sock, newsock);
      PersonNode *personNode = monitor->getPerson(citizenID);
      if (!personNode) {
        monitor->sendStringToTravelMonitor(sock, newsock, "NOTFOUND");
        break;
      }
      Person *person = personNode->getPerson();
      monitor->sendStringToTravelMonitor(sock, newsock, "FOUND");
      string name = person->getFirstName() + " " + person->getLastName();
      string country = person->getCountry();
      int age = person->getAge();
      VaccinationInfoList *vaccList = personNode->getVaccinationInfoList();
      int numOfVaccinations = vaccList->getNumOfVaccinations();

      monitor->sendStringToTravelMonitor(sock, newsock, to_string(citizenID) + " " + name + " " + country);
      monitor->sendStringToTravelMonitor(sock, newsock, "AGE " + to_string(age));
      monitor->sendIntToTravelMonitor(sock, newsock, numOfVaccinations);
      for (int i = 0; i < numOfVaccinations; i++) {
        VaccinationInfo *vaccInfo = vaccList->getVaccinationByIndex(i);
        string vaccination;
        if (vaccInfo->hasBeenVaccinated()) {
          vaccination = "\033[1;32m " + vaccInfo->getVirus() + " VACCINATED ON " + vaccInfo->getDate()->getDateAsString() + "  \033[0m";
        } else {
          vaccination = "\033[1;31m " + vaccInfo->getVirus() + " NOT YET VACCINATED  \033[0m";
        }
        monitor->sendStringToTravelMonitor(sock, newsock, vaccination);
      }

      break;
    }
    case 5: {
      ofstream log("logs/log_file." + to_string(getpid()));
      streambuf *coutstreamBuffer = cout.rdbuf(); //backup stream buffer
      streambuf *logBuffer = log.rdbuf();         //get stream buffer of log file
      cout.rdbuf(logBuffer);                      // redirect cout buffer to log
      monitor->getCountries()->print();
      cout << "TOTAL TRAVEL REQUESTS " << monitor->getTotalRequests() << endl;
      cout << "ACCEPTED " << monitor->getAcceptedRequests() << endl;
      cout << "REJECTED " << monitor->getRejectedRequests() << endl;

      cout.rdbuf(coutstreamBuffer); //redirect stream back to cout
      cout << "\033[1;32m  -> Monitor " << monitorIndex << " created log... \033[0m" << endl;

      buffer->terminateAllThreads();

      //Join Threads
      for (int i = 0; i < numThreads; i++) {
        joinThread(monitor->getThreadId(i));
        cout << "\033[1;33m  -> Joined Thread " << i + 1 << " \033[0m" << endl;
      }
      delete monitor;
      delete paths;
      delete buffer;
      log.close();
      close(sock);
      close(newsock);
      exit(EXIT_SUCCESS);
    }
    }
  }
}