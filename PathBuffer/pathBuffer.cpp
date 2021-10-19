#include "pathBuffer.h"

using namespace std;

pathBuffer::pathBuffer(int maxSize, string *paths, int totalPaths) {
  this->maxSize = maxSize;
  this->paths = new string[maxSize];
  this->totalPaths = totalPaths;
  start = 0;
  end = -1;
  count = 0;
  pathsProcessed = 0;
  finished = false;

  pthread_mutex_init(&mtx, 0);
  pthread_cond_init(&notEmpty, 0);
  pthread_cond_init(&notFull, 0);
}

pathBuffer::~pathBuffer() {
  pthread_cond_destroy(&notEmpty);
  pthread_cond_destroy(&notFull);
  delete[] paths;
}

int pathBuffer::placeNewPath(string path) {
  int pathsCompleted;
  pthread_mutex_lock(&mtx); //Lock mutex to access buffer

  while (count >= maxSize) {
    pthread_cond_wait(&notFull, &mtx);
  }

  end = (end + 1) % maxSize;
  paths[end] = path;
  count++;
  pathsCompleted = pathsProcessed;
  pthread_mutex_unlock(&mtx); // Placing completed, unlock mutex
  pthread_cond_signal(&notEmpty);
  return pathsCompleted;
}

string pathBuffer::getPath() {
  string path;
  pthread_mutex_lock(&mtx);             // Lock mutex to access buffer
  while (count <= 0) {                  //buffer is empty
    pthread_cond_wait(&notEmpty, &mtx); //wait for get

    if (finished) {
      pthread_mutex_unlock(&mtx);     // Get Completed, unlock mutex
      pthread_cond_signal(&notEmpty); //Signal other threads that all paths are processed
      return "Done";
    }
  }
  path = paths[start];           //Get first item of buffer
  start = (start + 1) % maxSize; //Update Current Start Of Buffer
  count--;
  pthread_mutex_unlock(&mtx); // Get Completed, unlock mutex
  pthread_cond_signal(&notFull);
  return path;
}

void pathBuffer::completedPath() {
  pthread_mutex_lock(&mtx); // Lock mutex to access buffer
  pathsProcessed++;
  pthread_mutex_unlock(&mtx); //Set completed, unlock mutex;
}

int pathBuffer::getPathsProcessed() {

  int completePaths;
  pthread_mutex_lock(&mtx); // Lock mutex to access buffer
  completePaths = pathsProcessed;
  pthread_mutex_unlock(&mtx); // Get Completed, unlock mutex
  return completePaths;
}

bool pathBuffer::terminateMainThread() {
  bool isDone;
  pthread_mutex_lock(&mtx); // Lock mutex to access buffer
  isDone = pathsProcessed == totalPaths;
  pthread_mutex_unlock(&mtx); // Terminate Completed, unlock mutex
  return isDone;
}

void pathBuffer::terminateAllThreads() {
  pthread_mutex_lock(&mtx); // Lock mutex to access buffer
  finished = true;
  pthread_cond_signal(&notEmpty);
  pthread_mutex_unlock(&mtx); // Terminate Completed, unlock mutex
}

bool pathBuffer::checkIfFinished() {
  bool isFinished;
  pthread_mutex_lock(&mtx); // Lock mutex to access buffer
  isFinished = finished;
  if (isFinished)
    pthread_cond_signal(&notEmpty);
  pthread_mutex_unlock(&mtx); // Check Completed, unlock mutex
  return isFinished;
}

void pathBuffer::resetBuffer(int totalPaths) {
  pthread_mutex_lock(&mtx); // Lock mutex to access buffer
  this->totalPaths = totalPaths;
  pathsProcessed = 0;
  start = 0;
  end = -1;
  count = 0;
  pthread_mutex_unlock(&mtx); // Reset Completed, unlock mutex
}
