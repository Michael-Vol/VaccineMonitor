#include "Utils/Utils.h"
#include "travelMonitor/travelMonitor.h"
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

using namespace std;

int main(int argc, char **argv) {

  //Get and check arguments

  DIR *inputDIR = checkArgs(argc, argv);
  int numMonitors = atoi(argv[2]);
  int socketBufferSize = atoi(argv[4]);
  int cyclicBufferSize = atoi(argv[6]);
  int sizeOfBloom = atoi(argv[8]);
  string input_dir(argv[10]);
  int numThreads = atoi(argv[12]);

  //Create Travel Monitor
  travelMonitor *trMonitor = new travelMonitor(numMonitors, countCountries(inputDIR), socketBufferSize, cyclicBufferSize, sizeOfBloom, numThreads, input_dir, inputDIR);

  // Distribute countries to monitors
  struct dirent **countriesDir;
  int count = scandir(input_dir.c_str(), &countriesDir, NULL, alphasort);

  if ((count - 2) < numMonitors) {
    numMonitors = count - 2;
    trMonitor->setNumMonitors(numMonitors);
  }
  trMonitor->setCountriesDir(countriesDir);

  //Create Monitor Servers

  if (trMonitor->getNumCountries() / trMonitor->getNumMonitors() == 0) {
    trMonitor->setNumMonitors(trMonitor->getNumCountries() / trMonitor->getNumMonitors());
  }
  for (int i = 0; i < trMonitor->getNumMonitors(); i++) {
    int numDirs = trMonitor->numDirsPerMonitor(i);
    string *countries = new string[numDirs];
    trMonitor->distributeDirectories(i, countries);
    trMonitor->forkMonitorServer(i, PORT + i, numDirs, countries);
    delete[] countries;
  }

  for (int i = 0; i < numMonitors; i++) {
    //Create Socket Structs
    int sock;
    struct sockaddr_in monitorServer;
    struct sockaddr *monitorServerPtr = (struct sockaddr *)&monitorServer;
    struct hostent *monitorServerHost;

    //Create Socket
    sock = createSocket();
    trMonitor->setSocket(i, sock);

    //Get MonitorServer Address (localhost)
    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);
    monitorServerHost = gethostbyname(hostname);

    //Initialize monitorServer
    monitorServer.sin_family = AF_INET;
    memcpy(&monitorServer.sin_addr, monitorServerHost->h_addr, monitorServerHost->h_length);
    monitorServer.sin_port = htons(PORT + i);

    //Connect to socket
    connectSocket(sock, monitorServerPtr);
    cout << "\033[1;34m  \n-> Client connected to Monitor Server \033[0m" << endl;

    trMonitor->getBloomFilters(sock);
    trMonitor->sendStringToMonitor(sock, "All Blooms Transfered");
    cout << "\033[1;32m -> Monitor " << i + 1 << " transfered Bloom Filters \033[0m " << endl;
  }

  trMonitor->processCommand();
}
