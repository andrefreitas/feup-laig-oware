#include "Player.h"

void Player::setName(string name){
	this->name = name;
}

void Player::setType(string type){
	this->type = type;
}

void Player::setScore(int newScore){
	this->score = newScore;
}

void Player::setSeeds(vector<int> seeds){
	this->seeds = seeds;
}

string Player::getName(){
	return name;
}

string Player::getType(){
	return type;
}

int Player::getScore(){
	return score;
}

vector<int> Player::getSeeds(){
	return seeds;
}

int Player::getHoleSeeds(int hole){
	if(hole < 1 || hole >6)
		return -1;

	return seeds[hole-1];
}

Player::~Player(){}


Human::Human(string name, string type){
	setName(name);
	setType(type);
}

int Human::play(Socket *s1, int hole){
	string msg;
	string num;

	if(getHoleSeeds(hole) > 0){
		msg = "[playerChooses, ";
		num = hole+48;
		msg.append(num);
		msg.append("].\n");
		s1->writes(msg);

		return 0;
	}

	return 1;
}

Computer::Computer(string name, string type){
	setName(name);
	setType(type);
}

int Computer::play(Socket *s1, int hole){
	cout << "\nCOMPUTER\n";
	return 0;
}
