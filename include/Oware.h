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
	stack<vector<string> > statusStack;//playerTurn, boardStatus, player1Score, player2Score
	queue<vector<string> > demoModeQueue;
	queue<int> demoModeChooses;
	queue<vector<string> > movie;

public:
	Oware();
	void createGame(Player *player1, Player *player2, int dificulty);
	void createGame(string player1Name, string player1Type, string player2Name, string player2Type, int dificulty);
	string getRoules();
	Player* getPlayer1();
	Player* getPlayer2();
	Player* getPlayer(int playerTurn);
	queue<vector<string> > getDemoModeQueue();
	queue<int> getDemoModeChooses();
	Board* getBoard();
	int startServer();
	bool startGame(string player1, string player2);
	bool startGame(string player1, string player2, string playerTurn, string board, string scoreP1, string scoreP2);
	void endGame();

	/*
	 * return values:
	 * -1 continue
	 *  0 no seeds
	 *  1 endGame
	 */
	int readStatus();
	void update();

	void saveStatus(string playerTurn, string board, string player1Score, string player2Score);
	int getStatusStackSize();
	bool undoIsReadyToUse();
	void undo();
	void skipPlayer();
	vector<string> topStatus();
	string statusToPlayerTurn(vector<string> status);
	string satusToBoard(vector<string> status);
	string statusToPlayer1Score(vector<string> status);
	string statusToPlayer2Score(vector<string> status);

	queue<vector<string> > getMovie();
	vector<string> getMovieFrame(unsigned int frame);

	void setPlayerTurn(string playerTurn);
	string getPlayerTurn();
	void setPlayerChoose(int playerChoose);
	int getPlayerChoose();
	int getWinner();
	int getFinalPoints();
	int getMaxTime();
	void swapPlayerTurn();
	void play(int hole);
};


#endif /* OWARE_H_ */
