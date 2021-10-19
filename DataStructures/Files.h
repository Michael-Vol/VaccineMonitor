#ifndef FILES_H
#define FILES_H
#include <iostream>
#include <string>

using namespace std;

class File {
  string fileName;

public:
  File *next;
  File(string fileName);
  string getFileName();
  ~File();
  void print();
};

class Files {
  File *head;

public:
  Files();
  ~Files();
  File *addFile(string fileName);
  File *checkExistance(string fileName);
  void printFiles();
  void getAllFiles(string *files);
};

#endif