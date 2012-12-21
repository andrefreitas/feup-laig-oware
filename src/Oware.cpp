#include "Oware.h"

string itos(int i){
	stringstream out;
	out << i;
	return out.str();
}

Oware::Oware(Player *player1, Player *player2, int dificulty){
	board = new Board();
	this->player1 = player1;
	this->player2 = player2;

	switch(dificulty){
	case 1: maxTime = 30; break;
	case 2: maxTime = 20; break;
	case 3: maxTime = 10; break;
	}

	this->player1->setScore(0);
	this->player2->setScore(0);
}

Oware::Oware(string player1Name, string player1Type, string player2Name, string player2Type, int dificulty){
	board = new Board();

	if(player1Type == "computer"){
		if(dificulty == 1)
			this->player1 = new Computer(player1Name, "bot1");
		else
			this->player1 = new Computer(player1Name, "bot2");
	}
	else
		this->player1 = new Human(player1Name, "human");

	if(player2Type == "computer"){
		if(dificulty == 1)
			this->player2 = new Computer(player2Name, "bot1");
		else
			this->player2 = new Computer(player2Name, "bot2");
	}
	else
		this->player2 = new Human(player2Name, "human");

	switch(dificulty){
	case 1: maxTime = 30; break;
	case 2: maxTime = 20; break;
	case 3: maxTime = 10; break;
	}

	this->player1->setScore(0);
	this->player2->setScore(0);
}

string Oware::getRoules(){
	string rules;
	rules = "\n"
			"1. O jogo inicia com 24 sementes para cada jogador\n\n"
			"2. Os jogadores jogam alternadamente\n\n"
			"3. O jogador escolhe um buraco e semeia as sementes\n"
			"   que l� est�o pelos outros buracos no sentido oposto\n"
			"   ao movimento dos ponteiros do rel�gio - 1 semente\n"
			"   para cada buraco\n\n"
			"4. Se a �ltima semente semeada da jogada calhar num\n"
			"   buraco advers�rio e este ficar com 2 ou 3 sementes\n"
			"   o jogador ganha as sementes de todos os buracos\n"
			"   consecutivos que ficaram com 2 ou 3 sementes a\n"
			"   partir desse\n\n"
			"5. Ganha o primeiro a obter 25 ou mais pontos\n\n";
	return rules;
}

Player* Oware::getPlayer1(){
	return player1;
}

Player* Oware::getPlayer2(){
	return player2;
}

Board* Oware::getBoard(){
	return board;
}

void Oware::startServer(Socket *s1){
	s1->opens();
}

bool Oware::startGame(Socket *s1, string player1, string player2){
	string msg;
	string computer;
	string str;

	this->playerTurn = "1";

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

	this->playerTurn = playerTurn;

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
		if((signed)msg.find("endGame") != -1){
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
		else if((signed)msg.find("noSeeds") != -1){
			cout << msg;
			num = 0; break;
		}
		else if((signed)msg.find("gameStatus") != -1){
			this->gameStatus = msg;
			cout << msg;
			num = -1; break;
		}
		else if((signed)msg.find("Chooses") != -1){
			this->playerChoose = msg.at(14) - 48;
			cout << msg;
		}
	}

	return num;
}

void Oware::update(){
	char *str;
	string board;
	string num, p1, p2;

	str = &gameStatus[0];

	vector<int> v1;
	vector<int> v2;
	strtok(str, "[,]");

	int n = 0;
	board = "[[";
	while(n++ < 6){
		v1.push_back(atoi(strtok(NULL, "[,]")));
		board.append(itos(v1.back()));
		if(v1.size() < 6) board.append(",");
	}
	player1->setSeeds(v1);

	n = 0;
	board.append("],[");
	while(n++ < 6){
		v2.push_back(atoi(strtok(NULL, "[,]")));
		board.append(itos(v2.back()));
		if(v2.size() < 6) board.append(",");
	}
	board.append("]]");
	player2->setSeeds(v2);

	player1->setScore(atoi(strtok(NULL, "] ")));
	player2->setScore(atoi(strtok(NULL, " ")));

	saveStatus(playerTurn, board, itos(player1->getScore()), itos(player2->getScore()));
	movie.push(statusStack.top());
}

void Oware::saveStatus(string playerTurn, string board, string player1Score, string player2Score){
	vector<string> vec;
	vec.push_back(playerTurn);
	vec.push_back(board);
	vec.push_back(player1Score);
	vec.push_back(player2Score);

	statusStack.push(vec);
}

int Oware::getStatusStackSize(){
	return statusStack.size();
}

bool Oware::undoIsReadyToUse(){
	if((player1->getType() != "human" || player2->getType() != "human") && statusStack.size() > 2)
		return true;
	else if((player1->getType() == "human" && player2->getType() == "human") && statusStack.size() > 1)
		return true;

	return false;
}

void Oware::undo(Socket *s1){
	if((player1->getType() != "human" || player2->getType() != "human") && statusStack.size() > 2){
		statusStack.pop(); cout << statusStack.size() << endl;
		statusStack.pop(); cout << statusStack.size() << endl;
		statusStack.pop(); cout << statusStack.size() << endl;
	}
	else if((player1->getType() == "human" && player2->getType() == "human") && statusStack.size() > 1){
		statusStack.pop();
		statusStack.pop();
	}

	endGame(s1);

	if(!statusStack.empty()){
		movie.push(statusStack.top());

		startGame(s1, player1->getType(), player2->getType(), statusStack.top()[0],
				statusStack.top()[1], statusStack.top()[2], statusStack.top()[3]);
	}
	else{
		vector<string> vec;
		vec.push_back("0");
		vec.push_back("[[0,0,0,0,0,0],[0,0,0,0,0,0]]");
		vec.push_back("0");
		vec.push_back("0");

		movie.push(vec);

		startGame(s1, player1->getType(), player2->getType());
	}
}

void Oware::skipPlayer(Socket *s1){
	vector<string> tempStatus;
	string playerTurn;

	tempStatus = statusStack.top();

	statusStack.pop();

	endGame(s1);
	if(tempStatus[0] == "1")
		playerTurn = "2";
	else
		playerTurn = "1";

	//movie
	vector<string> vec;
	vec.push_back(playerTurn);
	vec.push_back(tempStatus[1]);
	vec.push_back(tempStatus[2]);
	vec.push_back(tempStatus[3]);

	movie.push(vec);

	startGame(s1, player1->getType(), player2->getType(), playerTurn, tempStatus[1], tempStatus[2], tempStatus[3]);
}

vector<string> Oware::topStatus(){
	if(!statusStack.empty())
		return statusStack.top();

	vector<string> vec;
	return vec;
}

string Oware::statusToPlayerTurn(vector<string> status){
	return status[0];
}

string Oware::satusToBoard(vector<string> status){
	return status[1];
}

string Oware::statusToPlayer1Score(vector<string> status){
	return status[2];
}

string Oware::statusToPlayer2Score(vector<string> status){
	return status[3];
}

queue<vector<string> > Oware::getMovie(){
	return movie;
}

vector<string> Oware::getMovieFrame(unsigned int frame){
	queue<vector<string> > tempMovie(movie);

	if(frame <= 1)
		return tempMovie.front();
	else if(frame >= tempMovie.size())
		return tempMovie.back();

	for(unsigned int i = 1; i < frame; i++)
		tempMovie.pop();

	return tempMovie.front();
}

void Oware::setPlayerTurn(string playerTurn){
	this->playerTurn = playerTurn;
}

string Oware::getPlayerTurn(){
	return playerTurn;
}

void Oware::setPlayerChoose(int playerChoose){
	this->playerChoose = playerChoose;
}

int Oware::getPlayerChoose(){
	return playerChoose;
}

int Oware::getWinner(){
	return winner;
}

int Oware::getFinalPoints(){
	return finalPoints;
}

int Oware::getMaxTime(){
	return maxTime;
}

