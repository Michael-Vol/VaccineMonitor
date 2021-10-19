#include "monitorUtils.h"
#include "fstream"
#include <ctime>
#include <iostream>
#include <limits>
#include <sstream>
using namespace std;

Date *calculateDate(string dateArray) {
  //creates a Date Object from a date array
  stringstream iss(dateArray);
  string dateTokens[3];
  int i = 0;
  while (iss.good() && i < 3) {
    getline(iss, dateTokens[i], '-');
    ++i;
  }
  Date *date = new Date(stoi(dateTokens[0]), stoi(dateTokens[1]), stoi(dateTokens[2]));
  return date;
}

void clearStdin() {
  string temp;
  getline(cin, temp);
}

bool isDate(string dateArray) {
  stringstream iss(dateArray);
  string dateTokens[3];
  int i = 0;
  while (iss.good() && i < 3) {
    getline(iss, dateTokens[i], '-');
    ++i;
  }

  if (i < 3) {
    //array given is not a date
    return false;
  }
  return true;
}

Date *getCurrentDate() {
  //get current date
  time_t tt;
  struct tm *ti;
  time(&tt);
  ti = localtime(&tt);
  string dateStr = to_string(ti->tm_mday) + "-" + to_string(ti->tm_mon + 1) + "-" + to_string(ti->tm_year + 1900);
  return calculateDate(dateStr);
}

commandInfo *parseCommand() {
  int numArgs = 0;
  commandInfo *info = new commandInfo();
  info->numOfArguments = 0;
  info->commandCode = -1;
  string command, line, dummy;
  int commandCode;
  printCommandMenu();
  cin.seekg(0, ios::end);
  cin.clear();
  getline(cin, line);
  cout << endl;
  stringstream ssin(line);
  ssin >> command;

  if (command[0] != '/') {
    cout << "\033[1;31m  \nWrong Command Structure. Please use slash (/) before your command name.\033[0m" << endl;
    return info;
  }
  string *tokens = new string[5];

  while (ssin.good() && numArgs < 5) { //parse arguments
    ssin >> tokens[numArgs];
    numArgs++;
  }

  info->numOfArguments = numArgs;
  info->arguments = tokens;
  if (command == "/travelRequest" && numArgs == 5) {
    commandCode = 1;
  } else if (command == "/travelStats" && numArgs >= 3) {
    commandCode = 2;
  } else if (command == "/addVaccinationRecords" && numArgs == 1) {
    commandCode = 3;
  } else if (command == "/searchVaccinationStatus" && numArgs == 1) {
    commandCode = 4;
  } else if (command == "/exit") {
    commandCode = 5;
  }
  info->commandCode = commandCode;
  return info;
}
bool levelUp() {
  //selects randomly if skipList needs to also insert node to an upper level
  return (rand() % K) < (K * 0.9); //return value based on K and p constants
}

void printCommandMenu() {
  cout << endl;
  cout << "\033[1;34m  Available Commands: \033[0m" << endl;
  cout << "• /travelRequest citizenID date countryFrom countryTo virusName" << endl;
  cout << "• /travelStats virusName date1 date2 [country]" << endl;
  cout << "• /addVaccinationRecords country" << endl;
  cout << "• /searchVaccinationStatus citizenID" << endl;
  cout << "• /exit" << endl;
  cout << endl;

  cout << "➞ ";
}
