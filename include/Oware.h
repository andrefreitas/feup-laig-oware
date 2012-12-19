#ifndef OWARE_H_
#define OWARE_H_

#include "Socket.h"
#include "Player.h"

class Oware{
private:
	Player *player1;
	Player *player2;
	int winner;
	int finalPoints;
	//Board board;
	//Scenario scenario;
	string gameStatus;

public:
	Oware(Player *player1, Player *player2);
	string getRoules();
	void startServer(Socket *s1);
	bool startGame(Socket *s1, string player1, string player2);
	bool startGame(Socket *s1, string player1, string player2, string playerTurn,
                   string board, string scoreP1, string scoreP2);
	void endGame(Socket *s1);

	/*
	 * return values:
	 * -1 continue
	 *  0 no seeds
	 *  1 endGame
	 */
	int readStatus(Socket *s1);
	void updatePlayers();

	int getWinner();
	int getFinalPoints();
};


#endif /* OWARE_H_ */
