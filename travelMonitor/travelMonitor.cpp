#include "travelMonitor.h"

using namespace std;

travelMonitor::travelMonitor(int numMonitors, int numCountries, int socketBufferSize, int cyclicBufferSize, int sizeOfBloom, int numThreads, string input_dir, DIR *inputDIR) {
  this->numMonitors = numMonitors;
  this->numCountries = numCountries;
  this->socketBufferSize = socketBufferSize;
  this->cyclicBufferSize = cyclicBufferSize;
  this->sizeOfBloom = sizeOfBloom;
  this->numThreads = numThreads;
  string endSlash = input_dir[input_dir.length() - 1] != '/' ? "/" : "";
  this->input_dir = input_dir + endSlash;
  this->inputDIR = inputDIR;
  this->bloomSet = new BloomSet();
  this->info = nullptr;
  this->monitorPIDs = new int[numMonitors];
  this->sockets = new int[numMonitors];
  this->requests = new Requests();
}

travelMonitor::~travelMonitor() {
  delete[] monitorPIDs;
  delete[] sockets;
  delete bloomSet;
  delete requests;
  delete info;
  closeDir(inputDIR);
}

int travelMonitor::getNumCountries() {
  return numCountries;
}

int travelMonitor::getNumMonitors() {
  return numMonitors;
}

int travelMonitor::getSocketBufferSize() {
  return socketBufferSize;
}
int travelMonitor::getCyclicBufferSize() {
  return cyclicBufferSize;
}

int travelMonitor::getSizeOfBloom() {
  return sizeOfBloom;
}
int travelMonitor::getnumThreads() {
  return numThreads;
}

string travelMonitor::getInputDir() {
  return input_dir;
}

DIR *travelMonitor::getInputDIR() {
  return inputDIR;
}

int *travelMonitor::getMonitorPIDs() {
  return monitorPIDs;
}
int travelMonitor::getSocket(int monitorIndex) {
  return sockets[monitorIndex];
}
void travelMonitor::setSocket(int monitorIndex, int socket) {
  sockets[monitorIndex] = socket;
}

int travelMonitor::forkMonitorServer(int monitorIndex, int port, int numCountries, string *paths) {
  pid_t ppid, monitorpid;
  ppid = getpid();

  if ((monitorpid = fork()) < 0) { //try to fork new Monitor Server
    perror("Couldn't fork Monitor Server");
    exit(EXIT_FAILURE);
  }

  if (monitorpid == 0) {

    //Generate args array for each Monitor
    char **args = new char *[6 + numCountries];
    for (int i = 0; i < 5 + numCountries; i++) {
      args[i] = new char[1024];
      args[i][1023] = '\0';
    }
    sprintf(args[0], "%d", port);
    sprintf(args[1], "%d", numThreads);
    sprintf(args[2], "%d", socketBufferSize);
    sprintf(args[3], "%d", cyclicBufferSize);
    sprintf(args[4], "%d", sizeOfBloom);

    for (int i = 0; i < numCountries; i++) {
      args[5 + i] = (char *)paths[i].c_str();
    }
    args[5 + numCountries] = NULL;

    //Create Monitor
    execvp("./monitorServer", (char **)args);
    exit(EXIT_SUCCESS);
  } else {
    monitorPIDs[monitorIndex] = monitorpid;
  }
  return monitorpid;
}

void travelMonitor::setNumMonitors(int numMonitors) {
  this->numMonitors = numMonitors;
}
void travelMonitor::setCountriesDir(struct dirent **countriesDir) {
  this->countriesDir = countriesDir;
}
int travelMonitor::numDirsPerMonitor(int monitorIndex) {
  struct dirent *country;

  int RRIndex = 0;
  int numDirs = 0;

  for (int i = 0; i < numCountries + 2; i++) {
    if (!strcmp(countriesDir[i]->d_name, ".") || !strcmp(countriesDir[i]->d_name, "..")) { //ignore current and parent directory names
      continue;
    }
    if (RRIndex % numMonitors == 0) {
      RRIndex = 0;
    }
    if (RRIndex == monitorIndex) {
      country = countriesDir[i];
      if (country != NULL && country->d_type == DT_DIR) {
        numDirs++;
      }
    }
    RRIndex++;
  }
  return numDirs;
}

RequestNode *travelMonitor::addRequestNode(int citizenID, Date *date, string countryFrom, string countryTo, string virus, bool wasAccepted) {
  return requests->addRequestNode(citizenID, date, countryFrom, countryTo, virus, wasAccepted);
}

int travelMonitor::getTotalRequests() {
  return requests->getTotalRequests();
}
int travelMonitor::getAcceptedRequests() {
  return requests->getAcceptedRequests();
}
int travelMonitor::getRejectedRequests() {
  return requests->getRejectedRequests();
}
void travelMonitor::addAcceptedRequest() {
  requests->addAcceptedRequest();
}
void travelMonitor::addRejectedRequest() {
  requests->addRejectedRequest();
}
void travelMonitor::searchRequests(Date *date1, Date *date2, string country) {
  requests->searchRequests(date1, date2, country);
}
int travelMonitor::getMonitorIDByCountry(string country) {
  int RRIndex = 0;
  for (int i = 0; i < numCountries + 2; i++) {
    if (!strcmp(countriesDir[i]->d_name, ".") || !strcmp(countriesDir[i]->d_name, "..")) { //ignore current and parent directory names
      continue;
    }
    if (RRIndex % numMonitors == 0) {
      RRIndex = 0;
    }
    if (countriesDir[i]->d_name == country) {
      return RRIndex;
    }
    RRIndex++;
  }
  return -1;
}
void travelMonitor::distributeDirectories(int monitorIndex, string *countries) {
  struct dirent *country;

  int RRIndex = 0;
  int numDirs = numDirsPerMonitor(monitorIndex);

  int dirIndex = 0;
  for (int i = 0; i < numCountries + 2; i++) {
    if (!strcmp(countriesDir[i]->d_name, ".") || !strcmp(countriesDir[i]->d_name, "..")) { //ignore current and parent directory names
      continue;
    }
    if (RRIndex % numMonitors == 0) {
      RRIndex = 0;
    }
    if (RRIndex == monitorIndex) {
      country = countriesDir[i];
      if (country != NULL && country->d_type == DT_DIR) {
        string countryName(country->d_name);

        countries[dirIndex] = input_dir + countryName;
        dirIndex++;
      }
    }
    RRIndex++;
  }
}

void travelMonitor::sendStringToMonitor(int socket, string text) {
  const char *textStr = text.c_str();
  int sizeOfText = strlen(textStr);
  if (write(socket, &sizeOfText, sizeof(int)) < 0) {
    printError("TravelMonitor couldn't write size of text to socket", socket);
  }
  int bufferIndex = 0;
  for (int i = 0; i <= sizeOfText / socketBufferSize; i++) {
    int transferSize = (sizeOfText > socketBufferSize ? socketBufferSize : sizeOfText);
    if (write(socket, &textStr[bufferIndex], transferSize) < 0) {
      printError("TravelMonitor couldn't write text to socket", socket);
    }
    bufferIndex += socketBufferSize;
  }
}

string travelMonitor::getStringFromMonitor(int socket) {
  int textSize = getIntFromMonitor(socket);
  //cout << "Text Size : " << textSize << endl;
  string text;
  for (int i = 0; i <= textSize / socketBufferSize; i++) {
    int transferSize = (textSize > socketBufferSize ? socketBufferSize : textSize);
    int bytesRead;
    char next[transferSize];
    do {
      if ((bytesRead = read(socket, &next, transferSize)) < 0) {
        printError("TravelMonitor couldn't read text from socket", socket);
      }
      next[transferSize] = '\0';
    } while (bytesRead < transferSize);
    text += next;
  }
  sendStringToMonitor(socket, "Done");

  return text;
}

void travelMonitor::sendIntToMonitor(int socket, int number) {
  if (write(socket, &number, sizeof(int)) < 0) {
    printError("TravelMonitor couldn't write to socket", socket);
  }
}
int travelMonitor::getIntFromMonitor(int socket) {
  int number;
  int bytesRead;
  do {
    if ((bytesRead = read(socket, &number, sizeof(int))) < 0) {
      printError("TravelMonitor couldn't read from socket", socket);
    }
  } while (bytesRead < sizeof(int));
  return number;
}

void travelMonitor::getBloomFilters(int socket) {
  BloomNode *node;
  int numOfBloomNodes;
  uint64_t bloomNodeSize;

  numOfBloomNodes = getIntFromMonitor(socket);
  for (int i = 0; i < numOfBloomNodes; i++) {

    //get VirusName
    string virusName = getStringFromMonitor(socket);

    //get Bloom Filter

    int filterSize = sizeOfBloom / sizeof(int);
    int *filter = new int[filterSize];
    int bufferIndex = 0;

    

    for (int i = 0; i < filterSize / socketBufferSize; i++) {
      int transferSize = (filterSize > socketBufferSize ? socketBufferSize : filterSize);
      int transferWidth = (bufferIndex + socketBufferSize / sizeof(int)) - bufferIndex;

      for (int j = bufferIndex; j < bufferIndex + socketBufferSize / sizeof(int); j++) {
        filter[j] = getIntFromMonitor(socket);
      }
      bufferIndex += transferWidth;
    }

    int transferSize = filterSize % socketBufferSize;
    for (int j = bufferIndex; j < bufferIndex + transferSize / sizeof(int); j++) {
      filter[j] = getIntFromMonitor(socket);
    }

    cout << "Transfered : " << virusName << endl;

    BloomFilter *bloomFilter = new BloomFilter(sizeOfBloom);
    bloomFilter->fillFilter(filter);

    if ((node = bloomSet->getNode(virusName))) {
      node->mergeFilters(bloomFilter); //node already exists , merge filters
      delete bloomFilter;
    } else {
      bloomSet->addBloomNode(bloomFilter, virusName);
    }
  }
  cout << endl;
}

void travelMonitor::processCommand() {
  do {
    if (info) {
      delete info;
    }
    info = parseCommand();

    if (info->numOfArguments == 0 && info->commandCode < 5) {
      continue;
    }

    switch (info->commandCode) {
    case 1: {
      int citizenID = stoi(info->arguments[0]);
      if (!isDate(info->arguments[1])) {
        cout << "Date is not valid." << endl;
        continue;
      }
      Date *date = calculateDate(info->arguments[1]);
      string countryFrom = info->arguments[2];
      string countryTo = info->arguments[3];
      string virusName = info->arguments[4];
      cout << " -> Checking Bloom Filter for Vaccination Record ..." << endl;
      if (!(bloomSet->checkVaccination(virusName, citizenID))) {
        cout << "\033[1;31m REQUEST REJECTED – YOU ARE NOT VACCINATED \033[0m" << endl;
        addRequestNode(citizenID, date, countryFrom, countryTo, virusName, false);
        continue;
      } else { //Ask monitor for vaccination status
        cout << " -> Requesting Vaccination Info from Monitor Server..." << endl;
        int monitorID = getMonitorIDByCountry(countryFrom);
        int sock = getSocket(monitorID);
        sendIntToMonitor(sock, info->commandCode);
        sendIntToMonitor(sock, citizenID);
        sendStringToMonitor(sock, virusName);
        string isVaccinated = getStringFromMonitor(sock);

        int monthsPassed;
        bool canTravel = false;
        if (isVaccinated == "YES") {
          Date *vaccinationDate = calculateDate(getStringFromMonitor(sock));
          monthsPassed = date->monthsHavePassed(vaccinationDate);
          if (monthsPassed <= 180 && monthsPassed > 0) {
            cout << "\033[1;32m \n REQUEST ACCEPTED – HAPPY TRAVELS \033[0m" << endl;
            sendStringToMonitor(sock, "ACCEPTED");
            canTravel = true;
          } else if (monthsPassed > 0) {
            cout << "\033[1;31m \nREQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE \033[0m" << endl;
            sendStringToMonitor(sock, "REJECTED");
          } else {
            cout << "\033[1;31m \n REQUEST REJECTED – YOU ARE NOT VACCINATED \033[0m" << endl;
            sendStringToMonitor(sock, "REJECTED");
          }
        } else {
          cout << "\033[1;31m \n REQUEST REJECTED – YOU ARE NOT VACCINATED \033[0m" << endl;
          sendStringToMonitor(sock, "REJECTED");
        }
        addRequestNode(citizenID, date, countryFrom, countryTo, virusName, isVaccinated == "YES" && canTravel);
      }
      break;
    }
    case 2: {
      string virusName = info->arguments[0];
      if (!isDate(info->arguments[1]) || !isDate(info->arguments[2])) {
        cout << "Date is not valid." << endl;
        continue;
      }
      Date *date1 = calculateDate(info->arguments[1]);
      Date *date2 = calculateDate(info->arguments[2]);
      string countryName = "";
      if (info->numOfArguments == 4) {
        countryName = info->arguments[3];
      }
      searchRequests(date1, date2, countryName);

      break;
    }
    case 3: {
      string country = info->arguments[0];
      int monitorID = getMonitorIDByCountry(country);
      int sock = getSocket(monitorID);
      sendIntToMonitor(sock, info->commandCode);
      sendStringToMonitor(sock, getInputDir() + country);
      cout << "Sent Directory to Monitor Server to check for new files..." << endl;
      int expectNewFiles = getIntFromMonitor(sock);
      if (!expectNewFiles) {
        cout << "\033[1;31m  No new files found. \033[0m " << endl;

      } else {
        getBloomFilters(sock);
        sendStringToMonitor(sock, "All Blooms Transfered");
        cout << "\033[1;32m -> Monitor " << monitorID + 1 << " transfered Bloom Filters \033[0m " << endl;
      }
      break;
    }
    case 4: {
      int citizenID = stoi(info->arguments[0]);
      for (int i = 0; i < numMonitors; i++) {
        int sock = getSocket(i);
        sendIntToMonitor(sock, info->commandCode);

        sendIntToMonitor(sock, citizenID);
        string personFound = getStringFromMonitor(sock);
        if (personFound == "FOUND") {
          string basicInfo = getStringFromMonitor(sock);
          string ageInfo = getStringFromMonitor(sock);
          int numOfVaccinations = getIntFromMonitor(sock);

          cout << basicInfo << endl;
          cout << ageInfo << endl;
          for (int k = 0; k < numOfVaccinations; k++) {
            string vaccinationInfo = getStringFromMonitor(sock);
            cout << vaccinationInfo << endl;
          }
        }
      }
      break;
    }
    case 5: {
      for (int i = 0; i < numMonitors; i++) {
        int sock = getSocket(i);
        sendIntToMonitor(sock, info->commandCode);
      }
      string streamName = "logs/log_file." + to_string(getpid());
      ofstream log(streamName);
      streambuf *coutstreamBuffer = cout.rdbuf(); //backup stream buffer
      streambuf *logBuffer = log.rdbuf();         //get stream buffer of log file
      cout.rdbuf(logBuffer);                      // redirect cout buffer to log
      printCountries(inputDIR);
      cout << "TOTAL TRAVEL REQUESTS " << getTotalRequests() << endl;
      cout << "ACCEPTED " << getAcceptedRequests() << endl;
      cout << "REJECTED " << getRejectedRequests() << endl;
      cout.rdbuf(coutstreamBuffer); //redirect stream back to cout
      log.close();
      cout << "\033[1;32m \n->Travel Monitor Created Log...\033[0m" << endl;
      int status;
      wait(&status); //wait for monitorServers to Exit

      for (int i = 0; i < numMonitors; i++) {
        close(getSocket(i));
        cout << "\033[1;31m ->Closing Socket " << i << "\033[0m" << endl;
      }
      cout << "Exiting ..." << endl;
      delete this;
      exit(EXIT_SUCCESS);
    }
    }
  } while (true);
}