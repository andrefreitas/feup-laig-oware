#include "Oware.h"

string itos(int i){
	stringstream out;
	out << i;
	return out.str();
}

Oware::Oware(){
	board = new Board();
	s1 = new Socket("127.0.0.1",6300);

	winner = 1;
	finalPoints = 1;
	maxTime = 1;
	playerChoose = 1;
	seed=new LSFSphere(5.0,12,6);
	seed->init(0);
}

void Oware::createGame(Player *player1, Player *player2, int dificulty){
	if(player1->getType() == "human")
		this->player1 = new Human(player1->getName(), player1->getType());
	else
		this->player1 = new Computer(player1->getName(), player1->getType());

	if(player2->getType() == "human")
		this->player2 = new Human(player2->getName(), player2->getType());
	else
		this->player2 = new Computer(player2->getName(), player2->getType());

	switch(dificulty){
	case 1: maxTime = 30; break;
	case 2: maxTime = 20; break;
	case 3: maxTime = 10; break;
	}

	this->player1->setScore(0);
	this->player2->setScore(0);
}

void Oware::createGame(string player1Name, string player1Type, string player2Name, string player2Type, int dificulty){
	if(player1Type != "human"){
		if(dificulty == 1)
			this->player1 = new Computer(player1Name, "bot1");
		else
			this->player1 = new Computer(player1Name, "bot2");
	}
	else
		this->player1 = new Human(player1Name, "human");

	if(player2Type != "human"){
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

void Oware::loadDemoMode(){
	queue<string> msg;
	FILE * file;
	int num;

	if((file = fopen("demoMode.txt", "r")) == NULL)
		exit(EXIT_FAILURE);

	char line[100];
	char *read;
	while(!feof(file)){
		read = fgets(line, 100, file);
		if(read)
			msg.push((string)line);
	}

	fclose(file);

	for(;!msg.empty(); msg.pop()){
		cout << msg.front();
		string tmp = msg.front();
		num = decodeMSG(tmp);
		if(num == 1)
			break;
		else if(num == 0)
			swapPlayerTurn();
		else if(num == -1){
			update();
			swapPlayerTurn();
		}
	}
}

string Oware::getRoules(){
	string rules;
	rules = "\n"
			"1. O jogo inicia com 24 sementes para cada jogador\n\n"
			"2. Os jogadores jogam alternadamente\n\n"
			"3. O jogador escolhe um buraco e semeia as sementes\n"
			"   que lá estão pelos outros buracos no sentido oposto\n"
			"   ao movimento dos ponteiros do relógio - 1 semente\n"
			"   para cada buraco\n\n"
			"4. Se a última semente semeada da jogada calhar num\n"
			"   buraco adversário e este ficar com 2 ou 3 sementes\n"
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

Player* Oware::getPlayer(int playerTurn){
	if(playerTurn == 1)
		return player1;
	else
		return player2;
}

queue<int> Oware::getDemoModeChooses(){
	return demoModeChooses;
}

Board* Oware::getBoard(){
	return board;
}

int Oware::startServer(){
	int success = s1->opens();

	return success;
}

bool Oware::startGame(string player1, string player2){
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

bool Oware::startGame(string player1, string player2, string playerTurn, string board, string scoreP1, string scoreP2){
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

void Oware::endGame(){
	s1->writes("[endGame].\n");
}

int Oware::readStatus(){
	string msg;
	int num;

	s1->reads(msg);
	num = decodeMSG(msg);

	return num;
}

int Oware::decodeMSG(string msg){
	if((signed)msg.find("endGame") != -1){
		if((signed)msg.find("Chooses") != -1){
			this->playerChoose = msg.at(msg.find("Chooses") + 8) - 48;
			if(player1->getType() != "human" && player2->getType() != "human")
				this->demoModeChooses.push(this->playerChoose);
		}
		cout << msg;
		int pos = msg.find("victory");
		if(pos != -1){
			winner = msg.at(pos + 8) - 48;
			finalPoints = (msg.at(pos + 10) - 48) * 10 + (msg.at(pos + 11) - 48);

			if(player1->getType() != "human" && player2->getType() != "human"){
				demoModeWinner = winner;
				demoModeFinalPoints = finalPoints;
			}
		}
		else{
			winner = 0;
			finalPoints = 24;

			if(player1->getType() != "human" && player2->getType() != "human"){
				demoModeWinner = winner;
				demoModeFinalPoints = finalPoints;
			}
		}

		cout << winner << endl;
		cout << finalPoints << endl;

		return 0;
	}
	else if((signed)msg.find("noSeeds.") != -1){
		cout << msg;
		if(msg.size() > 20){
			this->nextStatus = new string(msg.substr(10));
			this->nextStatusActive = true;
			cout << "nextStatus: " << *nextStatus << endl;
		}
		return 1;
	}
	else if((signed)msg.find("gameStatus") != -1){
		cout << msg;
		this->gameStatus = new string(msg);
		if(msg.size() > 99){
			this->nextStatus = new string(msg.substr(99, msg.size()-99));
			this->nextStatusActive = true;
			cout << "nextStatus: " << *nextStatus << endl;
		}
		if((signed)msg.find("Chooses") != -1){
			this->playerChoose = msg.at(msg.find("Chooses") + 8) - 48;
			cout <<  this->playerChoose << endl;
		}
		return 2;
	}
	else if((signed)msg.find("Chooses") != -1){
		this->playerChoose = msg.at(14) - 48;
		if(player1->getType() != "human" && player2->getType() != "human")
			this->demoModeChooses.push(this->playerChoose);
		cout << msg;
		cout << this->playerChoose << endl;
		return 3;
	}

	return -1;
}

void Oware::update(){
	char *str;
	string board;
	string num, p1, p2;

	str = (char*)gameStatus->c_str();

	vector<int> v1;
	vector<int> v2;
	strtok(str, "[,]");

	int n = 0;
	board = "[[";
	while(n < 6){
		v1.push_back(atoi(strtok(NULL, "[,]")));
		board.append(itos(v1.back()));
		if(v1.size() < 6) board.append(",");
		n++;
	}
	player1->setSeeds(v1);

	n = 0;
	board.append("],[");
	while(n < 6){
		v2.push_back(atoi(strtok(NULL, "[,]")));
		board.append(itos(v2.back()));
		if(v2.size() < 6) board.append(",");
		n++;
	}
	board.append("]]");
	player2->setSeeds(v2);

	player1->setScore(atoi(strtok(NULL, "] ")));
	player2->setScore(atoi(strtok(NULL, " ")));

	saveStatus(playerTurn, board, itos(player1->getScore()), itos(player2->getScore()));
	movie.push(status.top());
}

void Oware::saveStatus(string playerTurn, string board, string player1Score, string player2Score){
	vector<string> vec;
	vec.push_back(playerTurn);
	vec.push_back(board);
	vec.push_back(player1Score);
	vec.push_back(player2Score);

	if(player1->getType() != "human" && player2->getType() != "human"){
		demoModeStatus.push(vec);
		demoModePlayer1Seeds.push(player1->getSeeds());
		demoModePlayer2Seeds.push(player2->getSeeds());
	}

	status.push(vec);
}

int Oware::getStatusSize(){
	return status.size();
}

void Oware::setGameStatus(string *status){
	this->gameStatus = status;
}

bool Oware::isNextStatusActive(){
	return this->nextStatusActive;
}

void Oware::clearNextStatus(){
	this->nextStatusActive = false;
}

string *Oware::getGameStatus(){
	return this->gameStatus;
}

string *Oware::getNextStatus(){
	return this->nextStatus;
}

bool Oware::undoIsReadyToUse(){
	if((player1->getType() != "human" || player2->getType() != "human") && status.size() > 2)
		return true;
	else if((player1->getType() == "human" && player2->getType() == "human") && status.size() > 1)
		return true;

	return false;
}

void Oware::undo(){
	if((player1->getType() != "human" || player2->getType() != "human") && status.size() > 2){
		status.pop(); cout << status.size() << endl;
		status.pop(); cout << status.size() << endl;
		status.pop(); cout << status.size() << endl;
	}
	else if((player1->getType() == "human" && player2->getType() == "human") && status.size() > 1){
		status.pop();
		status.pop();
	}

	endGame();

	if(!status.empty()){
		movie.push(status.top());

		startGame(player1->getType(), player2->getType(), status.top().at(0),
				status.top().at(1), status.top().at(2), status.top().at(3));
	}
	else{
		vector<string> vec;
		vec.push_back("0");
		vec.push_back("[[0,0,0,0,0,0],[0,0,0,0,0,0]]");
		vec.push_back("0");
		vec.push_back("0");

		movie.push(vec);

		startGame(player1->getType(), player2->getType());
	}
}

void Oware::skipPlayer(){
	if(!status.empty()){
		vector<string> statusTemp = status.top();

		this->swapPlayerTurn();
		endGame();

		status.pop();

		//movie
		vector<string> vec;
		vec.push_back(this->playerTurn);
		vec.push_back(statusTemp.at(1));
		vec.push_back(statusTemp.at(2));
		vec.push_back(statusTemp.at(3));

		movie.push(vec);

		startGame(player1->getType(), player2->getType(), this->playerTurn,
				  statusTemp.at(1), statusTemp.at(2), statusTemp.at(3));
	}
}
stack<vector<string> > Oware::getStatus(){
	return status;
}

vector<string> Oware::topStatus(){
	if(!status.empty())
		return status.top();

	vector<string> vec;
	return vec;
}

queue<vector<string> > Oware::getDemoModeStatus(){
	return demoModeStatus;
}

string Oware::statusToPlayerTurn(vector<string> status){
	return status.at(0);
}

string Oware::satusToBoard(vector<string> status){
	return status.at(1);
}

string Oware::statusToPlayer1Score(vector<string> status){
	return status.at(2);
}

string Oware::statusToPlayer2Score(vector<string> status){
	return status.at(3);
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

int Oware::getDemoModeWinner(){
	return demoModeWinner;
}

int Oware::getDemoModeFinalPoints(){
	return demoModeFinalPoints;
}

queue<vector<int> > Oware::getdemoModePlayerSeeds(int playerNum){
	if(playerNum == 1)
		return demoModePlayer1Seeds;
	else
		return demoModePlayer2Seeds;
}

int Oware::getFinalPoints(){
	return finalPoints;
}

int Oware::getMaxTime(){
	return maxTime;
}

void Oware::swapPlayerTurn(){
	if(this->getPlayerTurn() == "1")
		this->setPlayerTurn("2");
	else
		this->setPlayerTurn("1");
}

void Oware::play(int hole){
	if(hole >= 0 && hole <= 5)
		player1->play(s1, 6-hole);
	else if(hole >= 6 && hole <= 11)
		player2->play(s1, hole-5);
}

void Oware::drawHoleSeeds(int seeds, int x, int y, int z){
	int dz = 0;
	float kx = 0, ky = 0;

	for(int j=0; j<seeds; j++){
		// Draw a Seed
		glPushMatrix();
			if((j%3) == 0 && j > 0) {
				dz = !dz;
				kx = j*2.2;
			}
			if((j%6) == 0) ky++;

		    glTranslated(x+2.2*j-kx, ky, 4.0*dz);
			glScalef(0.15, 0.15, 0.35);
			seed->draw();
		glPopMatrix();
	}
}

void Oware::drawSeeds(vector<int> seeds, int playerScore, int x, int y, int z){
	float kx=0;
	float ky=0;
	float kz=0;

	glPushMatrix();
	glTranslated(-10,6,27);

	int displacement[6]={0,13,27,40,53,66};

	glTranslated(0, 0, z);

	// For each hole of line 1
	for (int i = 0; i < 6; i++)
		drawHoleSeeds(seeds[i], displacement[i], 0, 0);


	glTranslated(1, 0, 5);

	glTranslated(x, 0, 0);
	int signal;
	if(x == 0) signal = -1;
	else signal = 1;
	// Player collected seeds
	kx = 0; ky = 0; kz = 0;
	for(int i = 0; i < playerScore; i++){
		if((i%3) == 0 && i > 0) {
			kz += 4;
			kx = i*2.2;
		}

		if((i%18)==0 && i>0){
			kz -= 4*6;
			ky++;
		}

		glPushMatrix();
		glTranslated(-14+2.2*i-kx,ky,-kz*signal+(signal == -1 ? -7 : 0));
		glScalef(0.15, 0.15, 0.35);
		seed->draw();
		glPopMatrix();

	}

	glPopMatrix();
}

//void Oware::drawSeeds(){
//
//	int numseeds=28;
//	int dx,dy,dz=0;
//	float kx=0;
//	float ky=0;
//	float kz=0;
//	float zsignal=-1;
//	int displacement[6]={0,13,27,40,53,66};
//
//	// Get score and seeds
//	int p1score=player1->getScore();
//	int p2score=player2->getScore();
//	vector<int> line1=player1->getSeeds();
//	vector<int> line2=player2->getSeeds();
//
//	// Line 1
//	glPushMatrix();
//	glTranslated(-10,6,27);
//	for (int i=0; i<6; i++){
//		dz=0;
//		kx=0;
//		ky=0;
//		numseeds=line1[i];
//		for(int j=0; j<numseeds; j++){
//			// Draw a Seed
//			glPushMatrix();
//
//				if((j%3)==0 && j>0) {
//					dz=!dz;
//					kx=j*2.2;
//				}
//
//				if((j%6)==0) ky++;
//
//			    glTranslated(displacement[i]+2.2*j-kx,ky,4.0*dz);
//				glScalef(0.15, 0.15, 0.35);
//				seed->draw();
//			glPopMatrix();
//		}
//	}
//
//	// Line 2
//	glTranslated(0,0,14);
//
//	for (int i=0; i<6; i++){
//		dz=0;
//		kx=0;
//		ky=0;
//		numseeds=line2[i];
//		for(int j=0; j<numseeds; j++){
//			// Draw a Seed
//			glPushMatrix();
//
//				if((j%3)==0 && j>0) {
//					dz=!dz;
//					kx=j*2.2;
//				}
//
//				if((j%6)==0) ky++;
//			    glTranslated(displacement[i]+2.2*j-kx,ky,4.0*dz);
//				glScalef(0.15, 0.15, 0.35);
//				seed->draw();
//			glPopMatrix();
//		}
//	}
//
//
//	//Draw players scores
//
//	glTranslated(1,0,5);
//
//	// Player 2
//	dz=0; kx=0; ky=0; kz=0; zsignal=-1;
//	for(int i=0; i<p1score;i++){
//		if((i%3)==0 && i>0) {
//			kz+=4;
//			kx=i*2.2;
//
//		}
//
//		if((i%18)==0 && i>0){
//			kz-=4*6;
//			ky++;
//		}
//
//		glPushMatrix();
//		glTranslated(-14+2.2*i-kx,ky,-kz);
//		glScalef(0.15, 0.15, 0.35);
//		seed->draw();
//		glPopMatrix();
//
//	}
//
//
//	glTranslated(94,0,0);
//
//	// Player 1
//	dz=0; kx=0; ky=0; kz=0; zsignal=-1;
//	for(int i=0; i<p2score;i++){
//		if((i%3)==0 && i>0) {
//			kz+=4;
//			kx=i*2.2;
//		}
//
//		if((i%18)==0 && i>0){
//			kz-=4*6;
//			ky++;
//		}
//
//		glPushMatrix();
//		glTranslated(-14+2.2*i-kx,ky,-kz);
//		glScalef(0.15, 0.15, 0.35);
//		seed->draw();
//		glPopMatrix();
//
//	}
//
//
//	glPopMatrix();
//}
