#include "Utils.h"

using namespace std;

pthread_mutex_t lineParserMtx = PTHREAD_MUTEX_INITIALIZER;
DIR *checkArgs(int argc, char **argv) {
  // check number of arguments
  if (argc < 12) {
    cerr << "[CHECKARGS] Wrong number of arguments provided" << endl;
    printUsage();
    exit(EX_USAGE);
  }

  //check argument flags
  if (strcmp(argv[1], "-m") || strcmp(argv[3], "-b") || strcmp(argv[5], "-c") || strcmp(argv[7], "-s") || strcmp(argv[9], "-i") || strcmp(argv[11], "-t")) {
    cerr << "[CHECKARGS] False arguments" << endl;
    printErrno();
    printUsage();
    exit(EX_USAGE);
  }

  //try to open input_dir

  DIR *input_dir = openDir(argv[10]);
  return input_dir;
}

DIR *openDir(char *path) {
  DIR *input_dir = opendir(path);
  if (ENOENT == errno) {
    cerr << "[CHECKARGS] Input Directory doesn't exist" << endl;
    printErrno();
    exit(errno);
  } else if (input_dir == NULL) {
    cerr << "[CHECKARGS] Couldn't open input_dir" << endl;
    printErrno();
    exit(errno);
  }
  return input_dir;
}

int closeDir(DIR *dir) {
  if (closedir(dir) == -1) {
    cerr << "[CLOSEDIR] Couldn't close input directory" << endl;
    printErrno();
    exit(errno);
  }
  return 0;
}
void printErrno() {
  cout << "Errno: " << errno << " | " << strerror(errno) << endl;
}
void printError(string text, int socketToClose, int secondarySocket) {
  text += "\n";
  perror(text.c_str());
  close(socketToClose);
  if (secondarySocket > -1) {
    close(secondarySocket);
  }
  exit(EXIT_FAILURE);
}

void printUsage() {
  cout << endl
       << "Usage : ./travelMonitor [args]" << endl;
  cout << "-m numMonitors : number of Monitor Servers to create" << endl;
  cout << "-b socketBufferSize : buffer size of socket in bytes" << endl;
  cout << "-c cyclicBufferSize : cyclic buffer size of Monitor Servers in entries" << endl;
  cout << "-s sizeOfBloom: size of Bloom Filter in bytes" << endl;
  cout << "-i input_dir: input directory for monitors" << endl;
  cout << "-t numThreads: number of threads for each Monitor Server" << endl;
}

void checkArgsMonitor(int argc, char **argv) {
  if (argc < 6) {
    cerr << "Wrong number of arguments provided to Monitor" << endl;
    exit(EXIT_FAILURE);
  }
}

int getBitPortion(int source, int low, int high) {
  unsigned r = 0;
  for (unsigned i = low; i <= high; i++)
    source |= 1 << i;

  return source;
}

int countCountries(DIR *inputDIR) {
  struct dirent *counter;
  int numCountries = 0;
  while ((counter = readdir(inputDIR)) != NULL) {
    if (!strcmp(counter->d_name, ".") || !strcmp(counter->d_name, "..")) { //ignore current and parent directory names
      continue;
    }
    if (counter->d_type == DT_DIR) { /* If the entry is a regular file */
      numCountries++;
    }
  }
  rewinddir(inputDIR);
  return numCountries;
}

void getCountriesInDir(DIR *directory, string *countries) {
  struct dirent *counter;
  int numCountries = 0;
  while ((counter = readdir(directory)) != NULL) {
    if (counter->d_type == DT_REG) { /* If the entry is a regular file */
      string country(counter->d_name);
      countries[numCountries] = country;
      numCountries++;
    }
  }
  rewinddir(directory);
}

int countCountriesInDir(DIR *directory) {
  struct dirent *counter;
  int numCountries = 0;
  while ((counter = readdir(directory)) != NULL) {
    if (counter->d_type == DT_REG) { /* If the entry is a regular file */
      numCountries++;
    }
  }
  rewinddir(directory);
  return numCountries;
}

void printCountries(DIR *inputDIR) {
  struct dirent *printer;
  printer = readdir(inputDIR);
  while (!strcmp(printer->d_name, ".") || !strcmp(printer->d_name, "..")) { //ignore current and parent directory names
    printer = readdir(inputDIR);
  }
  while ((printer = readdir(inputDIR)) != NULL) {
    if (printer->d_type == DT_DIR) { /* If the entry is a regular file */
      cout << printer->d_name << endl;
    }
  }
  rewinddir(inputDIR);
}

//Sockets

int createSocket() {
  int sock;
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("Couldn't create socket from monitorServer");
    close(sock);
    exit(EXIT_FAILURE);
  }
  int enabler = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enabler, sizeof(enabler)) < 0) {
    perror("Couldn't set Socket Options");
    exit(EXIT_FAILURE);
  }
  return sock;
}
void bindSocket(int sock, struct sockaddr *addrPtr) {
  if (bind(sock, addrPtr, sizeof(*addrPtr)) < 0) {
    perror("Couldn't bind socket to Monitor Server");
    close(sock);
    exit(EXIT_FAILURE);
  }
}
void listenSocket(int sock, int numOfConnections) {
  if (listen(sock, 1) < 0) {
    cerr << "-> Monitor Server couldn't listen for connections on port" << endl;
    exit(EXIT_FAILURE);
  }
}

int acceptSocket(int sock, struct sockaddr *clientPtr, socklen_t clientlen) {
  int newsock;
  if ((newsock = accept(sock, clientPtr, &clientlen)) < 0) {
    perror("Monitor Server couldn't accept request");
    close(newsock);
    close(sock);
    exit(EXIT_FAILURE);
  }
  return newsock;
}

void connectSocket(int sock, struct sockaddr *addrPtr) {
  int connected;
  do {
    if (((connected = connect(sock, addrPtr, sizeof(*addrPtr))) < 0) && errno != ECONNREFUSED) {
      printError("Couldn't connect to Monitor Server", sock);
    }
  } while (connected < 0);
}

//Threads
void printThreadError(string text, int err) {
  cerr << text << " : " << strerror(err) << endl;
  exit(EXIT_FAILURE);
}
pthread_t createThread(threadArgs *thrArgs) {
  pthread_t thread;
  int err;
  if ((err = pthread_create(&thread, NULL, threadExecuter, (void *)thrArgs))) {
    printThreadError("Couldn't create thread", err);
  }
  return thread;
}

int joinThread(pthread_t thread) {
  int err;
  void *status;
  if ((err = pthread_join(thread, (void **)&status))) {
    printThreadError("Couldn't join thread", err);
  }
  return err;
}

int detachThread() {
  int err;
  if ((err = pthread_detach(pthread_self()))) {
    printThreadError("Couldn't detach thread", err);
  }
  return err;
}

void *threadExecuter(void *args) {
  threadArgs *thrArgs = (threadArgs *)args;
  pathBuffer *buffer = thrArgs->buffer;
  Monitor *monitor = thrArgs->monitor;
  int numPaths = thrArgs->numPaths;
  string *paths = thrArgs->paths;
  int pathsProcessed;

  fstream file;
  string line;

  while (!buffer->checkIfFinished()) {
    string path = buffer->getPath();
    if (path == "Done") //Finished processing all paths
      break;

    file.open(path);
    while (getline(file, line)) {
      pthread_mutex_lock(&lineParserMtx);
      monitor->parseLine(line);
      pthread_mutex_unlock(&lineParserMtx);
    }
    buffer->completedPath();
    file.close();
  }
  return NULL;
};
