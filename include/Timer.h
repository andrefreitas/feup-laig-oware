#ifndef TIMER_H_
#define TIMER_H_

#include <iostream>
#include <cstdio>
#include <ctime>

#ifdef __linux__
#include <unistd.h>
#include <stdlib.h>
#else
#include <windows.h>
#endif

using namespace std;

class Timer{
private:
	time_t startTime;
	time_t countTime;
	time_t endTime;
	time_t rawtime;
	struct tm * timeinfo;
	bool started;

public:
	Timer();
	void startCountDown(int startTime);
	void stopCountDown();
	int getCountDown();
	int getHour();
	int getMinute();
	int getSecond();
	bool isStarted();
	void wait(int seconds);
};


#endif /* TIMER_H_ */
