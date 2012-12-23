#ifndef OWARE_H_
#define OWARE_H_

#include <stack>
#include <queue>
#include <list>

#include "Socket.h"
#include "Player.h"
#include "Board.h"

#include <sstream>

string itos(int i);

class Oware{
private:
	Player *player1;
	Player *player2;
	Socket *s1;
	int winner;
	int finalPoints;
	int maxTime;
	Board *board;
	string playerTurn;
	string gameStatus;
	int playerChoose;
	stack<vector<string> > status;//playerTurn, boardStatus, player1Score, player2Score
	queue<vector<string> > movie;

	//exclusive demoMode
	int demoModeWinner;
	int demoModeFinalPoints;
	queue<vector<string> > demoModeStatus;
	queue<int> demoModeChooses;
	queue<vector<int> > demoModePlayer1Seeds;
	queue<vector<int> > demoModePlayer2Seeds;

public:
	Oware();
	void createGame(Player *player1, Player *player2, int dificulty);
	void createGame(string player1Name, string player1Type, string player2Name, string player2Type, int dificulty);
	void saveStatus(string playerTurn, string board, string player1Score, string player2Score);
	void setPlayerTurn(string playerTurn);
	void setPlayerChoose(int playerChoose);
	void play(int hole);
	void endGame();
	void update();
	void undo();
	void skipPlayer();
	void swapPlayerTurn();

	int startServer();
	int readStatus();
	int getStatusSize();
	int getPlayerChoose();
	int getWinner();
	int getFinalPoints();
	int getMaxTime();

	bool startGame(string player1, string player2);
	bool startGame(string player1, string player2, string playerTurn, string board, string scoreP1, string scoreP2);
	bool undoIsReadyToUse();

	Player* getPlayer1();
	Player* getPlayer2();
	Player* getPlayer(int playerTurn);

	Board* getBoard();

	string getRoules();
	string statusToPlayerTurn(vector<string> status);
	string satusToBoard(vector<string> status);
	string statusToPlayer1Score(vector<string> status);
	string statusToPlayer2Score(vector<string> status);
	string getPlayerTurn();

	stack<vector<string> > getStatus();

	vector<string> getMovieFrame(unsigned int frame);
	vector<string> topStatus();

	queue<vector<string> > getMovie();

	//exclusive demoMode
	queue<vector<string> > getDemoModeStatus();
	queue<int> getDemoModeChooses();
	int getDemoModeWinner();
	int getDemoModeFinalPoints();

};


#endif /* OWARE_H_ */
