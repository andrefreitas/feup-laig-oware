#include "Oware.h"

Oware::Oware(Player *player1, Player *player2){
	this->player1 = player1;
	this->player2 = player2;
}

string Oware::getRoules(){
	string rules;
	rules = "O jogo inicia com 24 sementes para cada jogador\n"
			"ficando 4 sementes em cada um dos 6 buracos\n"
			"Os jogadores jogam alternadamente...";
	return rules;
}

void Oware::startServer(Socket *s1){
	s1->opens();
}

bool Oware::startGame(Socket *s1, string player1, string player2){
	string msg;
	string computer;
	string str;

	str = "[beginGame,[";
	str.append(player1); str.append(",0],[");
	str.append(player2); str.append(",0],1,newBoard].\n");

	s1->writes(str.c_str());

	s1->reads(msg);

	return strncmp(msg.c_str(), "ack.", strlen("ack.")-1) == 0;
}

bool Oware::startGame(Socket *s1,  string player1, string player2, string playerTurn,
		              string board, string scoreP1, string scoreP2){
	string msg;
	string computer;
	string str;
	string num;
	string scorePlayer1, scorePlayer2;


	str = "[beginGame,[";
    str.append(player1); str.append(","); str.append(scoreP1); str.append("],[");
	str.append(player2); str.append(","); str.append(scoreP2); str.append("],");
	str.append(playerTurn); str.append(","); str.append(board);
	str.append("].\n");

	s1->writes(str.c_str());

	s1->reads(msg);

	return strncmp(msg.c_str(), "ack.", strlen("ack.")-1) == 0;
}

void Oware::endGame(Socket *s1){
	s1->writes("[endGame].");
}

int Oware::readStatus(Socket *s1){
	string msg;
	int num;

	while(s1->reads(msg)){
		cout << msg;
		if((signed)msg.find("gameStatus") != -1){
			this->gameStatus = msg;
			cout << msg;
			num = -1; break;
		}
		else if((signed)msg.find("noSeeds") != -1){
			cout << msg;
			num = 0; break;
		}
		else if((signed)msg.find("endGame") != -1){
			cout << msg;

			int pos = msg.find("victory");
			if(pos != -1){
				winner = msg.at(pos + 8) - 48;
				finalPoints = (msg.at(pos + 10) - 48) * 10 + (msg.at(pos + 11) - 48);
			}
			else{
				winner = 0;
				finalPoints = 24;
			}

			cout << winner << endl;
			cout << finalPoints << endl;

			num = 1; break;
		}
	}

	return num;
}

void Oware::updatePlayers(){
	char *str;

	str = &gameStatus[0];

	vector<int> v1;
	vector<int> v2;
	strtok(str, "[,]");

	int n = 0;
	while(n++ < 6)
		v1.push_back(atoi(strtok(NULL, "[,]")));
	player1->setSeeds(v1);

	n = 0;
	while(n++ < 6)
		v2.push_back(atoi(strtok(NULL, "[,]")));
	player2->setSeeds(v2);

	player1->setScore(atoi(strtok(NULL, "] ")));
	player2->setScore(atoi(strtok(NULL, " ")));
}

int Oware::getWinner(){
	return winner;
}

int Oware::getFinalPoints(){
	return finalPoints;
}

