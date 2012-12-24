#include "Timer.h"

Timer::Timer(){
	started = false;
}

void Timer::startCountDown(int countTime){
	this->countTime = countTime;
	time(&startTime);
	time(&endTime);

	srand ( time(NULL) );

	started = true;
}

void Timer::stopCountDown(){
	started = false;
}

int Timer::getCountDown(){
	int t;
	time(&endTime);

	t = countTime - (int)(endTime - startTime);

	if(t <= 0){
		started = false;
		t = 0;
	}

	return t;
}

int Timer::getHour(){
    timeinfo = localtime ( &rawtime );

    return timeinfo->tm_hour;
}

int Timer::getMinute(){
    timeinfo = localtime ( &rawtime );

    return timeinfo->tm_min;
}

int Timer::getSecond(){
    timeinfo = localtime ( &rawtime );

    return timeinfo->tm_sec;
}

bool Timer::isStarted(){
	return started;
}

void Timer::wait(int seconds){
#ifdef __linux__
	sleep(seconds);
#else
	Sleep(seconds*1000);
#endif
}

