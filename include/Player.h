#ifndef PLAYER_H_
#define PLAYER_H_

#include <iostream>
#include <string>
#include <vector>

#include "Socket.h"

using namespace std;

class Player{
private:
	string name;
	string type;
	int score;
	vector<int> seeds;

public:
	virtual int play(Socket *s1, int hole) = 0;
	void setName(string name);
	void setType(string type);
	void setScore(int newScore);
	void setSeeds(vector<int> seeds);
	string getName();
	string getType();
	int getScore();
	vector<int> getSeeds();
	int getHoleSeeds(int hole);
	virtual ~Player();
};

class Human: public Player{
public:
	Human(string name, string type);
	int play(Socket *s1, int hole);
};

class Computer: public Player{
public:
	Computer(string name, string type);
	int play(Socket *s1, int hole);
};


#endif /* PLAYER_H_ */
