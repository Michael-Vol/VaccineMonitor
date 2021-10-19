#ifndef DATE_H
#define DATE_H
#include <string>

using namespace std;
class Date {
  int day;
  int month;
  int year;

public:
  Date(int day, int month, int year);
  ~Date();
  bool isBetweenLimits(Date *date1, Date *date2);
  void print();
  int getDay();
  int getMonth();
  int getYear();
  int monthsHavePassed(Date *vaccinationDate);
  string getDateAsString();
};

#endif