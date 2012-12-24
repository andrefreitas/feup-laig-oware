#ifndef BOARD_H_
#define BOARD_H_

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Board{
private:
	vector<int> player1Seeds;
	vector<int> player2Seeds;
	int playerTurn;
	int startHole;
	int currentHole;
	int holeSeeds;
	bool loaded;
	bool startHoleCleared;

public:
	Board();
	void loadBoard(vector<int> player1Seeds, vector<int> player2Seeds, int playerTurn, int startHole);
	bool update();
	vector<int> getPlayerSeeds(int playerNum);
	bool isLoaded();
//	void draw();
//	void draw_aux(vector<int> playerSeeds, int i, int z);
};


#endif /* BOARD_H_ */
