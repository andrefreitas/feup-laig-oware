#include "Board.h"

Board::Board(){
	currentHole = startHole = 1;
	loaded = false;
}

void Board::loadBoard(vector<int> player1Seeds, vector<int> player2Seeds, int playerTurn, int startHole){
	this->player1Seeds = player1Seeds;
	this->player2Seeds = player2Seeds;
	this->playerTurn = playerTurn;
	this->startHole = startHole;

	if(playerTurn == 1)
		holeSeeds = player1Seeds.at(startHole-1);
	else if(playerTurn == 2)
		holeSeeds = player2Seeds.at(startHole-1);

	currentHole = startHole;
	loaded = true;
	startHoleCleared = false;
	seedsMoving = false;
}

bool Board::update(){
	if(!startHoleCleared){
		if(playerTurn == 1)
			player1Seeds.at(startHole-1) = 0;
		else if(playerTurn == 2)
			player2Seeds.at(startHole-1) = 0;

		startHoleCleared = true;
		return true;
	}
	else if(holeSeeds > 0){
		if(!seedsMoving){
			holeSeeds--;

			if(playerTurn == 1){
				currentHole--;
				if(currentHole == 0){
					playerTurn = 2;
					currentHole = 1;
				}
			}
			else if(playerTurn == 2){
				currentHole++;
				if(currentHole == 7){
					playerTurn = 1;
					currentHole = 6;
				}
			}

			if(playerTurn == 1){
				player1Seeds.at(currentHole-1)++;
			}
			else if(playerTurn == 2){
				player2Seeds.at(currentHole-1)++;
			}
		}
		return true;
	}
	else{
		loaded = false;
		return false;
	}
}

vector<int> Board::getPlayerSeeds(int playerNum){
	if(playerNum == 1)
		return player1Seeds;
	else
		return player2Seeds;
}

int Board::getHoleseeds(){
	return holeSeeds;
}

bool Board::isLoaded(){
	return loaded;
}

bool Board::isStartHoleCleared(){
	return startHoleCleared;
}

int Board::getStartHole(){
	if(playerTurn == 1)
		return 6 - startHole;
	else
		return 5 + startHole;
}

int Board::getCurrentHole(){
	if(playerTurn == 1)
		return 6 - currentHole;
	else
		return 5 + currentHole;
}

void Board::setSeedsMoving(bool moving){
	this->seedsMoving = moving;
}
