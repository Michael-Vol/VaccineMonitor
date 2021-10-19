UNAME := $(shell uname)
UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
CC = clang++
endif

ifeq ($(UNAME), Linux)
CC = g++
endif

TROBJS = Utils.o Requests.o travelMonitorClient.o BloomSet.o Files.o BloomFilter.o Hash.o Countries.o travelMonitor.o Monitor.o Date.o Viruses.o Persons.o SkipList.o VaccinationInfo.o monitorUtils.o  pathBuffer.o
MONITOROBJS = Utils.o  Requests.o travelMonitor.o Countries.o  BloomFilter.o  Files.o SkipList.o Persons.o VaccinationInfo.o Hash.o Monitor.o monitorUtils.o Date.o Viruses.o BloomSet.o  pathBuffer.o

CFLAGS = -g -c -std="c++11"
OUT = travelMonitorClient

$(OUT): clean $(TROBJS) $(MONITOROBJS)
	$(CC) -g -std="c++11"  $(MONITOROBJS) -o monitorServer monitorServer.cpp -lpthread
	$(CC) -g -std="c++11"  $(TROBJS) -o $(OUT) -lpthread


travelMonitorClient.o:
		$(CC) $(CFLAGS) travelMonitorClient.cpp
Utils.o:
		$(CC) $(CFLAGS) Utils/Utils.cpp
monitorUtils.o:
		$(CC) $(CFLAGS) Utils/monitorUtils.cpp
Countries.o:
		$(CC) $(CFLAGS) Countries/Countries.cpp
Persons.o:
		$(CC) $(CFLAGS) Persons/Persons.cpp
BloomFilter.o:
		$(CC) $(CFLAGS) DataStructures/BloomFilter.cpp
BloomSet.o:
		$(CC) $(CFLAGS) DataStructures/BloomSet.cpp
SkipList.o:
		$(CC) $(CFLAGS) DataStructures/SkipList.cpp
Hash.o:
		$(CC) $(CFLAGS) Utils/Hash.cpp
VaccinationInfo.o:
		$(CC) $(CFLAGS) VaccinationInfo/VaccinationInfo.cpp
Viruses.o:
		$(CC) $(CFLAGS) Viruses/Viruses.cpp	
travelMonitor.o:
		$(CC) $(CFLAGS) travelMonitor/travelMonitor.cpp	
Monitor.o:
		$(CC) $(CFLAGS) Monitor/Monitor.cpp
Date.o:
		$(CC) $(CFLAGS) DataStructures/Date.cpp
Files.o:
		$(CC) $(CFLAGS) DataStructures/Files.cpp
Requests.o:
			$(CC) $(CFLAGS) Requests/Requests.cpp
pathBuffer.o:
			$(CC) $(CFLAGS) PathBuffer/pathBuffer.cpp
.PHONY: clean 

clean:
	rm -rf travelMonitorClient monitorServer $(TROBJS)  $(MONITOROBJS) logs/*
clean2:
	rm -rf logs/*