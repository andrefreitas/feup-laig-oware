#include <iostream>
#include <exception>
#include "CGFapplication.h"
#include "LSFscene.h"
#include "LSFobjects.h"
#include "LSFinterface.h"

using std::cout;
using std::exception;
using namespace std;

//comentar esta parte para testar jogo na consola
int main(int argc, char* argv[]) {

	CGFapplication app = CGFapplication();

	LSFscene *scene = new LSFscene(argv);
	LSFinterface *interface = new LSFinterface(scene);

	try {
		app.init(&argc, argv);

		app.setScene(scene);
		app.setInterface(interface);

		app.run();
	}
	catch(GLexception& ex) {
		cout << "Erro: " << ex.what();
		return -1;
	}
	catch(exception& ex) {
		cout << "Erro inesperado: " << ex.what();
		return -1;
	}
	return 0;
}


//teste de jogo na consola
//#include "Player.h"
//#include "Socket.h"
//#include "Oware.h"
//
//int main(){
//	Player *player1;
//	Player *player2;
//	Socket *s1;
//	Oware *game;
//
//	player1 = new Computer("ABC", "bot2");
//	player2 = new Human("Paulo", "human");
//	s1 = new Socket("127.0.0.1",6300);
//
//	game = new Oware(player1, player2);
//
//	cout << game->getRoules() << endl;
//
//	game->startServer(s1);
//	game->startGame(s1, player1->getType(), player2->getType());
//	//game->startGame(s1, player1->getType(), player2->getType(), "1", "[[1,2,3,4,5,6],[1,15,1,1,1,1]]", "0", "6");
//	//game->startGame(s1, player1->getType(), player2->getType(), "1", "[[0,0,0,0,0,0],[0,0,0,0,0,0]]", "24", "24");
//
//	int status = 0;
//	while(true){
//		status = game->readStatus(s1);
//		if(status == 1)
//			break;
//
//		game->update();
//
//		//testes da stack
//		cout << endl;
//		cout << "statusStack size = " << game->getStatusStackSize() << endl;
//		if(game->getStatusStackSize())
//		cout << "topStack = " << game->topStatus()[0] << " " << game->topStatus()[1] << " "
//			 << game->topStatus()[2] << " " << game->topStatus()[3] << endl;
//		cout << endl;
//		//fim de teste da stack
//
//		int n = 0;
//		while(n++ < 6)
//			cout << " " << player1->getHoleSeeds(n);
//
//		cout << endl;
//		n = 0;
//		while(n++ < 6)
//			cout << " " << player2->getHoleSeeds(n);
//
//		cout << endl;
//
//		cout << "player1 points: " << player1->getScore() << endl;
//		cout << "player2 points: " << player2->getScore() << endl;
//
//		if(game->getPlayerTurn() == "2"){
//			int hole = 0, option = 1;
//			if(game->undoIsReadyToUse()){
//				cout << " 1 - jogar\n"
//					 << " 2 - undo\n"
//					 << " 3 - movie\n";
//				do{
//					cin >> option;
//				}while(option < 1 || option > 3);
//			}
//			if(option == 3){
//				for(unsigned int i = 1; i <= game->getMovie().size(); i++)
//					cout << game->getMovieFrame(i)[0] << " " << game->getMovieFrame(i)[1] << " "
//					     << game->getMovieFrame(i)[2] << " " << game->getMovieFrame(i)[3]<< endl;
//
//				option = 1;
//			}
//			if(option == 1){
//				do{
//					cout << "hole?: ";
//					cin >> hole;
//
//					if(hole == 0)
//						break;
//				}
//				while(player2->play(s1, hole));
//				if(hole == 0)
//					break;
//				game->setPlayerTurn("1");
//			}
//			else if(option == 2)
//				game->undo(s1);
//		}
//		else
//			game->setPlayerTurn("2");
//	}
//
//	if(game->getWinner() == 1){
//		player1->setScore(game->getFinalPoints());
//		cout << "venceu o jogador 1 com " << player1->getScore() << " pontos" << endl;
//	}
//	else if(game->getWinner() == 2){
//		player2->setScore(game->getFinalPoints());
//		cout << "venceu o jogador 2 com " << player2->getScore() << " pontos" << endl;
//	}
//	else{
//		player1->setScore(game->getFinalPoints());
//		player2->setScore(game->getFinalPoints());
//		cout << "empate com " << game->getFinalPoints() << " pontos" << endl;
//	}
//
//	game->endGame(s1);
//
//	return 0;
//}
