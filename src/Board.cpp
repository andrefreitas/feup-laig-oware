#include "Board.h"

Board::Board(){
	startHole = 1;
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

bool Board::isLoaded(){
	return loaded;
}

/*
void Board::draw(){
	for(int i = 1; i <= 6; i++)
		draw_aux(player1Seeds, i, 22);

	for(int i = 1; i <= 6; i++)
		draw_aux( player2Seeds, i, 30);
}

void Board::draw_aux(vector<int> playerSeeds, int i, int z){
	LSFscene::drawNumber(playerSeeds.at(i-1), 3.5, 2, z, 0.5);
//	string numbers;
//	stack<LSFappearance*> appearancesStack6;
//	appearancesStack6.push(app);
//	//Timer
//	if(playerSeeds.at(i-1) < 10){
//		numbers = playerSeeds.at(i-1);
//		glPushMatrix();
//		glTranslated(3.5, 2, z);
//		LSFrender::render(nodes,numbers,appearances,appearancesStack6,animations,LSFscene::timeSeconds);
//		glPopMatrix();
//	}
//	else{
//		numbers = playerSeeds.at(i-1)/10;
//		glPushMatrix();
//		glTranslated(3, 2, z);
//		LSFrender::render(nodes,numbers,appearances,appearancesStack6,animations,LSFscene::timeSeconds);
//		glPopMatrix();
//		numbers = playerSeeds.at(i-1)%10;
//		glPushMatrix();
//		glTranslated(4, 2, z);
//		LSFrender::render(nodes,numbers,appearances,appearancesStack6,animations,LSFscene::timeSeconds);
//		glPopMatrix();
//	}
}

*/
