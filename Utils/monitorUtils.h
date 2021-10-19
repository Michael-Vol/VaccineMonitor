#ifndef MONITORUTILS_H
#define MONITORUTILS_H
#include "../DataStructures/Date.h"
#include "../DataStructures/SkipList.h"
#include <string>
using namespace std;

typedef struct commandInfo {
  string *arguments;
  int numOfArguments;
  int commandCode;
  /* 
   1 - /travelRequest citizenID date countryFrom countryTo virusName
  2 - /travelStats virusName date1 date2 [country]
  3 - /addVaccinationRecords country
  4 - /searchVaccinationStatus citizenID
  5 - /exit
  */
} commandInfo;
Date *calculateDate(string dateArray);
Date *getCurrentDate();
bool isDate(string dateArray);
void printCommandMenu();
bool levelUp();
commandInfo *parseCommand();
void clearStdin();

#endif