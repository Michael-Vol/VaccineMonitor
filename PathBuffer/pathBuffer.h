#ifndef PATHBUFFER_H
#define PATHBUFFER_H
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
class pathBuffer {
  string *paths;
  int start;
  int end;
  int count;
  int maxSize;
  int pathsProcessed;
  int totalPaths;
  bool finished;
  pthread_mutex_t mtx;
  pthread_cond_t notEmpty;
  pthread_cond_t notFull;

public:
  pathBuffer(int maxSize, string *paths, int totalPaths);
  ~pathBuffer();
  int placeNewPath(string path);
  string getPath();
  int getPathsProcessed();
  void completedPath();
  bool terminateMainThread();
  void terminateAllThreads();
  void resetBuffer(int totalPaths);
  bool checkIfFinished();
};

#endif