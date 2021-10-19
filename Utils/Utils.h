#ifndef UTILS_H
#define UTILS_H
#define FIFOSPERMONITOR 2
#include "../Countries/Countries.h"
#include "../Monitor/Monitor.h"
#include "../PathBuffer/pathBuffer.h"
#include "../travelMonitor/travelMonitor.h"
#include <cstring>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <string>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sysexits.h>
#include <unistd.h>

class Monitor;
typedef struct threadArgs {
  Monitor *monitor;
  int numPaths;
  string *paths;
  pathBuffer *buffer;
} threadArgs;

void printUsage();
DIR *checkArgs(int argc, char **argv);
void checkArgsMonitor(int argc, char **argv);
int getBitPortion(int source, int low, int high);
void printErrno();
int closeDir(DIR *dir);
DIR *openDir(char *path);
int countCountries(DIR *inputDIR);
void printCountries(DIR *inputDIR);

//Sockets
int createSocket();
void bindSocket(int sock, struct sockaddr *addrPtr);
void listenSocket(int sock, int numOfConnections);
int acceptSocket(int sock, struct sockaddr *clientPtr, socklen_t clientlen);
void connectSocket(int sock, struct sockaddr *addrPtr);
void printError(string text, int socketToClose, int secondarySocket = -1);
void printThreadError(string text, int err);
void *threadExecuter(void *nl);
pthread_t createThread(threadArgs *args);
int joinThread(pthread_t thread);
int detachThread();
int countCountriesInDir(DIR *directory);
void getCountriesInDir(DIR *directory, string *countries);

#endif