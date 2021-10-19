#include "Monitor.h"

using namespace std;

Monitor::Monitor(int monitorID, int bloomFilterSize, int socketBufferSize, int cyclicBufferSize, int numPaths, string *dirs, int port, int numThreads) {
  this->monitorID = monitorID;
  this->bloomSize = bloomFilterSize;
  this->socketBufferSize = socketBufferSize;
  this->cyclicBufferSize = cyclicBufferSize;
  this->numDirs = numPaths;
  this->port = port;
  this->dirs = dirs;
  this->numThreads = numThreads;
  //Initialize Data Structures
  personsList = new PersonList();
  viruses = new Viruses();
  countries = new Countries();
  bloomset = new BloomSet();
  files = new Files();
  this->threadIDs = new pthread_t[numThreads];
  totalRequests = acceptedRequests = rejectedRequests = 0;
}

Monitor::~Monitor() {
  delete personsList;
  delete viruses;
  delete countries;
  delete bloomset;
  delete files;
  delete[] threadIDs;
}

void Monitor::addCountry(string country) {
  countries->addCountry(country);
}

void Monitor::addDir(string countryName) {
  dirs[numDirs] = countryName;
  numDirs++;
}
void Monitor::setListeningSocket(int socket) {
  this->listeningSocket = socket;
}
void Monitor::setSocket(int socket) {
  this->socket = socket;
}
void Monitor::addThreadID(int threadIndex, pthread_t threadID) {
  threadIDs[threadIndex] = threadID;
}
pthread_t Monitor::getThreadId(int threadIndex) {
  return threadIDs[threadIndex];
}
string *Monitor::getDirs() {
  return dirs;
}

int Monitor::getNumDirs() {
  return numDirs;
}

File *Monitor::checkFileExistance(string file) {
  return files->checkExistance(file);
}

File *Monitor::addFile(string fileName) {
  return files->addFile(fileName);
}

void Monitor::printFiles() {
  files->printFiles();
}

PersonNode *Monitor::getPerson(int citizenID) {
  return personsList->personExists(citizenID);
}
int Monitor::getTotalRequests() {
  return totalRequests;
}
int Monitor::getAcceptedRequests() {
  return acceptedRequests;
}
int Monitor::getRejectedRequests() {
  return rejectedRequests;
}
void Monitor::addAcceptedRequest() {
  acceptedRequests++;
  totalRequests++;
}

void Monitor::addRejectedRequest() {
  rejectedRequests++;
  totalRequests++;
}

string Monitor::getCountryByIndex(int countryIndex) {
  return countries->getCountryByIndex(countryIndex);
}

BloomSet *Monitor::createBloomSet() {
  viruses->createBloomSet(bloomset);
  return bloomset;
}

uint64_t Monitor::calculateSetSize() {
  return bloomset->calculateSize();
}
int Monitor::getNumOfBloomNodes() {
  return bloomset->getNumOfNodes();
}

uint64_t Monitor::getBloomNodeSize(int nodeIndex) {
  return bloomset->getNodeSize(nodeIndex);
}

string Monitor::getVirusName(int nodeIndex) {
  return bloomset->getVirusName(nodeIndex);
}

BloomNode *Monitor::getBloomNode(int nodeIndex) {
  return bloomset->getNode(nodeIndex);
}

Countries *Monitor::getCountries() {
  return countries;
}

bool Monitor::isVaccinated(int citizenID, string virus) {
  VirusNode *node;
  if ((node = viruses->getVirusNode(virus))) {
    return node->getSkipListStatus(citizenID);
  }
  return false;
}

string Monitor::getVaccinationDate(int citizenID, string virus) {
  PersonNode *node;
  string date = "";
  VaccinationInfo *info;
  if ((node = personsList->personExists(citizenID))) {
    if ((info = node->vaccinationInfoExists(virus))) {
      date = to_string(info->getDate()->getDay()) + "-" + to_string(info->getDate()->getMonth()) + "-" + to_string(info->getDate()->getYear());
    }
  }

  return date;
}

void Monitor::sendStringToTravelMonitor(int listeningSocket, int socket, string text) {
  const char *textStr = text.c_str();
  int bufferIndex = 0;
  int sizeOfText = strlen(textStr);
  sendIntToTravelMonitor(listeningSocket, socket, sizeOfText);
  for (int i = 0; i <= sizeOfText / socketBufferSize; i++) {
    int transferSize = (sizeOfText > socketBufferSize ? socketBufferSize : sizeOfText);
    if (write(socket, &textStr[bufferIndex], transferSize) < 0) {
      printError("Monitor couldn't write text to socket", socket, listeningSocket);
    }
    bufferIndex += socketBufferSize;
  }
  while (getStringFromTravelMonitor(listeningSocket, socket) != "Done")
    ;
}
string Monitor::getStringFromTravelMonitor(int listeningSocket, int socket) {
  int textSize;
  if (read(socket, &textSize, sizeof(int)) < 0) {
    printError("Monitor couldn't read size of text from socket", socket, listeningSocket);
  }
  string text;
  for (int i = 0; i <= textSize / socketBufferSize; i++) {
    int transferSize = (textSize > socketBufferSize ? socketBufferSize : textSize);
    char next[transferSize];
    if (read(socket, &next, transferSize) < 0) {
      printError("Monitor couldn't read text from socket", socket, listeningSocket);
    }
    next[transferSize] = '\0';
    text += next;
  }

  return text;
}
void Monitor::sendIntToTravelMonitor(int listeningSocket, int socket, int number) {
  int bytesSent;
  if ((bytesSent = write(socket, &number, sizeof(int)) < 0)) {
    printError("Monitor couldn't write to socket", socket, listeningSocket);
  }
}
int Monitor::getIntFromTravelMonitor(int listeningSocket, int socket) {
  int number;
  if (read(socket, &number, sizeof(int)) < 0) {
    printError("Monitor couldn't read from socket", socket, listeningSocket);
  }
  return number;
}

int Monitor::countAllPaths() {
  int numAllPaths = 0;
  for (int i = 0; i < numDirs; i++) {
    numAllPaths += countCountriesInDir(openDir((char *)(dirs[i]).c_str()));
  }
  return numAllPaths;
}

void Monitor::getAllPaths(string *paths) {
  int currentIndex = 0;
  for (int i = 0; i < numDirs; i++) {
    DIR *currentDir = openDir((char *)(dirs[i]).c_str());
    int numCountriesInDir = countCountriesInDir(currentDir);
    string *countries = new string[numCountriesInDir];
    getCountriesInDir(currentDir, countries);
    for (int j = 0; j < numCountriesInDir; j++) {
      paths[currentIndex + j] = dirs[i] + "/" + countries[j];
      files->addFile(countries[j]);
    }
    currentIndex += numCountriesInDir;
  }
}

void Monitor::parseLine(string line) {
  bool isVaccinated;
  // Parse Line
  stringstream ssin(line);
  string tokens[8]; // citizenID,firstName,lastName,country,age,virusName,isVaccinated,date(only
                    // if isVaccinated is true)
  int i = 0;
  while (ssin.good() && i < 8) {
    ssin >> tokens[i];
    i++;
  }
  // Determine if person is vaccinated
  Date *date = nullptr;
  if (i == 7) {
    isVaccinated = false;
  } else {
    isVaccinated = true;
    date = calculateDate(tokens[7]);
  }

  Person *person;
  PersonNode *existanceNode = personsList->personExists(stoi(tokens[0]));
  VaccinationInfo *vaccinationInfo = nullptr;
  if (existanceNode) { // if person already exists
    vaccinationInfo = existanceNode->vaccinationInfoExists(tokens[5]);
    if (vaccinationInfo) { // if vaccination info about same virus exist
      cout << "\033[1;31m  ERROR IN RECORD: \033[0m" << endl;
      existanceNode->print();
      return; // ignore record, read next line
    }
  }

  string *countryPtr = countries->addCountry(
      tokens[3]); // get pointer to country name (if country doesn't exist,
                  // also create country)
  if (existanceNode == nullptr) {
    person = new Person(stoi(tokens[0]), tokens[1], tokens[2], countryPtr, stoi(tokens[4])); // create Person
  } else {
    person = existanceNode->getPerson(); // get person that already exxists
  }
  string *virusPtr =
      viruses->addVirus(tokens[5], bloomSize, stoi(tokens[0])); // get pointer to virus name

  if (!vaccinationInfo) {
    vaccinationInfo = new VaccinationInfo(isVaccinated, virusPtr, date);
  }

  // Add person to list of persons
  personsList->createNode(person, vaccinationInfo,
                          countries->getCountry(*countryPtr));

  if (isVaccinated) {
    viruses->getBloomFilterPtr(*virusPtr)->insert(stoi(tokens[0])); // Insert to Bloom Filter
    viruses->getVaccinatedSkipList(*virusPtr)->insertNode(person);  // Insert to Vaccinated Skip List
  } else {
    viruses->getNotVaccinatedSkipList(*virusPtr)->insertNode(person); // Insert to Not Vaccinated Skip List
  }
}

void Monitor::sendBloomSet() {
  int numOfBloomNodes = getNumOfBloomNodes();
  sendIntToTravelMonitor(listeningSocket, socket, numOfBloomNodes);

  for (int i = 0; i < numOfBloomNodes; i++) {
    //Send virusNameSize
    BloomNode *node = getBloomNode(i);
    string virusNameStr = node->getVirusName();

    sendStringToTravelMonitor(listeningSocket, socket, virusNameStr);

    //    Send Bloom Filter
    BloomFilter *bloomFilter = node->getFilter();
    int *filter = bloomFilter->getFilter();
    int filterSize = bloomSize / sizeof(int);

    int bufferIndex = 0;

    for (int i = 0; i < filterSize / socketBufferSize; i++) {
      // cout << "Monitor is at " << i << " Of " << filterSize / socketBufferSize << " of " << virusNameStr << endl;
      int transferSize = (filterSize > socketBufferSize ? socketBufferSize : filterSize);
      int transferWidth = (bufferIndex + socketBufferSize / sizeof(int)) - bufferIndex;

      for (int j = bufferIndex; j < bufferIndex + socketBufferSize / sizeof(int); j++) {
        sendIntToTravelMonitor(listeningSocket, socket, filter[j]);
      }
      bufferIndex += transferWidth;
    }
    int transferSize = filterSize % socketBufferSize;
    for (int j = bufferIndex; j < bufferIndex + transferSize / sizeof(int); j++) {
      sendIntToTravelMonitor(listeningSocket, socket, filter[j]);
    }
  }
}
