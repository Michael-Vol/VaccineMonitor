#include "Date.h"
#include <iostream>
#include <string>
using namespace std;

Date::Date(int day, int month, int year) {
  this->day = day;
  this->month = month;
  this->year = year;
}
Date::~Date() {
}
int Date::getDay() {
  return day;
}
int Date::getMonth() {
  return month;
}
int Date::getYear() {
  return year;
}
void Date::print() {
  cout << " " << day << "-" << month << "-" << year << endl;
}
bool Date::isBetweenLimits(Date *date1, Date *date2) {
  string date1Str = to_string(date1->year) + '-' + to_string(date1->month) + '-' + to_string(date1->day);
  string date2Str = to_string(date2->year) + '-' + to_string(date2->month) + '-' + to_string(date2->day);
  string currentStr = to_string(year) + '-' + to_string(month) + '-' + to_string(day);
  return (currentStr >= date1Str) && (currentStr <= date2Str);
}

int Date::monthsHavePassed(Date *vaccinationDate) {

  int yearsDifference = (year - vaccinationDate->year) * 12 * 30;
  int monthsDifference = (month - vaccinationDate->month) * 30;
  int daysDifference = (day - vaccinationDate->day);
  int totalDifference = yearsDifference + monthsDifference + daysDifference;
  cout << totalDifference << " days since latest vaccination..." << endl;
  return totalDifference;
}

string Date::getDateAsString() {
  return to_string(day) + "-" + to_string(month) + "-" + to_string(year);
}
