#ifndef BOARD_H_
#define BOARD_H_

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Board{
private:
	vector<string> board;
	vector<int> player1Seeds;
	vector<int> player2Seeds;
	int startHole;

public:
	Board();
	void loadBoard(vector<string> board);
	void setStartHole(int startHole);
	void update();
	void draw();
};


#endif /* BOARD_H_ */
