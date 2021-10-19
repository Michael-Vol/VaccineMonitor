#include "Files.h"

using namespace std;

Files::Files() {
  head = nullptr;
}

Files::~Files() {
  File *current = head;
  File *next;
  while (current) {
    next = current->next;
    delete current;
    current = next;
  }
}

File *Files::addFile(string fileName) {
  File *currentFile = head;
  File *file;
  while (currentFile && currentFile->next) {
    currentFile = currentFile->next;
  }
  if (head == nullptr) {
    file = head = new File(fileName);
  } else {
    file = currentFile->next = new File(fileName);
  }
  return file;
}

File *Files::checkExistance(string fileName) {
  File *currentFile = head;
  while (currentFile) {
    if (currentFile->getFileName() == fileName) {
      return currentFile;
    }
    currentFile = currentFile->next;
  }
  return nullptr;
}
void Files::printFiles() {
  File *currentFile = head;
  while (currentFile) {
    currentFile->print();
    currentFile = currentFile->next;
  }
}
File::File(string fileName) {
  this->fileName = fileName;
  this->next = nullptr;
}
File::~File() {
}
string File::getFileName() {
  return fileName;
}
void File::print() {
  cout << fileName << endl;
}

void Files::getAllFiles(string *files) {
  int i = 0;
  File *currentFile = head;
  while (currentFile) {
    files[i++] = currentFile->getFileName();
    currentFile = currentFile->next;
  }
}