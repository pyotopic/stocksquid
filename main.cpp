#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include <string>
#include <set>
#include <ctime>
#include <sys/time.h>
#include <chrono>
#include <fstream>
#include <algorithm>
#include "Move.h"
#include "Game.h"
#include "helpers.h"
//#include <ofstream>
//#include <ifstream>

using namespace std;

const string initialFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0";

Move getUserMove(Game& game);

void moveTest();
void pseudoTest();
void speedTest();

void pseudoTest(){
    Game game = Game(initialFen);
    time_t start = time(NULL);
    int tot = 0;
    for (int i = 0; i < 6; i++){
        //time_t startStime = time(NULL);
        int x = game.countPseudo(i);
        tot += x;
        //time_t notTime = time(NULL);
        //cout << x<< " positions "<<endl;
        //int timeDif = notTime - startStime;
        //cout <<"This took "<<timeDif << " seconds"<<endl;
        //if (timeDif > 0){
        //    cout <<x / timeDif <<" nodes per second"<<endl; 
        //}
        
    }

    time_t end = time(NULL);
    cout <<" this in total took" << int(end - start) << " seconds"<<endl;
    cout << tot / (int(end - start)) << " nodes per second"<<endl;
}

void speedTest(){
    Game game = Game(initialFen);
    int totalMoves = 0;
    vector<Move> pseudos = game.generatePseudoLegal();
    time_t start = time(NULL);
    while (time(NULL) - start < 10){
        //pseudos = game.generatePseudoLegal();
        for (int i = 0; i < pseudos.size(); i++){
            game.makeMove(pseudos[i]);
            game.unmake();
        }
        totalMoves += pseudos.size();
    }
    cout <<"This took "<< time(NULL) - start << " seconds"<<endl;
    cout <<totalMoves <<" moves processed"<<endl;
    cout << totalMoves / (time(NULL) - start) <<" moves per second"<<endl;
}

void moveTest(){
    Game game = Game(initialFen);
    time_t start = time(NULL);
    int tot = 0;
    for (int i = 0; i < 6; i++){
        time_t startStime = time(NULL);
        int x = game.countMoves(i);
        tot += x;
        time_t notTime = time(NULL);
        cout << x<< " positions "<<endl;
        int timeDif = notTime - startStime;
        cout <<"This took "<<timeDif << " seconds"<<endl;
        if (timeDif > 0){
            cout <<x / timeDif <<" nodes per second"<<endl; 
        }
        
    }
    time_t end = time(NULL);
    cout <<" this in total took" << int(end - start) << " seconds"<<endl;
    cout << tot / (int(end - start)) << " nodes per second"<<endl;
}



int main(){

    string customStart = "r1b1k2r/ppppqp2/7p/4p1p1/4n3/2P1bQ2/PP3PPP/RN2KB1R w KQkq - 0 0";
    string customStart2 = "r1b1k2r/pppp1ppp/2n5/4p1B1/4P3/p1P3P1/2P1QP1P/R3KB1q b Qkq - 0 0";
    string customStart3 = "r1bqkb1r/ppp1pppp/3p1n2/4n3/2B1PP2/2N5/PPPPQ1PP/R1B1K1NR b KQkq - 0 0";
    string customStart4 = "r2k1b1r/ppN1ppp1/2q2n1p/8/P1nP4/5RQP/1PP3P1/R1B3K1 b - - 0 0";
    string customStart5 = "rnbqkbnr/ppppppPp/8/8/8/8/PPPPPP1P/RNBQKBNR w KQkq - 0 0";
    string customStart6 = "rnbqkbnr/pppppp2/8/8/3PP1p1/6PP/PPP3B1RNBQK1NR b KQkq - 0 0";
    string testPosition = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0";
    Game game = Game(initialFen);
    cout << game;

    //int numMoves = game.countMoves(0);

    //cout <<" there are "<<numMoves << endl;

    //moveTest();
    //pseudoTest();
    //speedTest();

    //int numMoves = game.countMoves(5);
    //cout<<"There are "<<numMoves <<" sequences available"<<endl;


    //int ai = 1;
    set<int> ais;
    ais.insert(0);
    //ais.insert(1);
    //CHANGE THE DEPTH HERE
    int depth = 4;

    while (true){
        
        // vector<Move> movers = game.generateLegal();
        // for (auto move: movers){
        //     game.printMove(move);
        // }
        if (ais.count(game.wtp) == 1){
            auto start  = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
            Move move = game.getBestMove2(depth);
            auto end  = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
            game.printMove(move);
            cout <<" this took "<<end - start <<" milliseconds"<<endl;
            game.makeMove(move);
        }
        else game.makeMove(getUserMove(game));
        cout << game;
        int result = game.gameOver();
        if (result >= 0){
            if (result == 0) cout <<"Black wins! "<<endl;
            else if (result == 1) cout <<"White wins!"<<endl;
            else cout <<"Draw."<<endl;
            break;
        }
    }
    return 0;
}



Move getUserMove(Game& game){
    string moveString;
    cout<<"Enter Move:\n";
    cin>>moveString;
    Piece piece = getCodeFor(moveString[0]);
    int toCol = moveString[1]-97;
    int toRow = 56 - moveString[2];
    vector<Move> goodMoves;
    for (auto move: game.generateLegal()){
        if ((move.to == 8 * toRow + toCol) && (game.board[move.from]==piece)) goodMoves.push_back(move);
    }

    if (goodMoves.size() == 0){
        cout<<"no move found"<<endl;
        return getUserMove(game);
    }
    else if (goodMoves.size() > 1){
        string from;
        cout <<"Enter start square:" << endl;
        cin >> from;
        int fromCol = from[0]-97;
        int fromRow = 56 - from[1];
        int fromSquare = 8 * fromRow + fromCol;
        for (int i = 0; i < goodMoves.size();){
            if (goodMoves[i].from != fromSquare){
                goodMoves.erase(goodMoves.begin()+i);
            } else i++;
        }
    }
    if (goodMoves.size() == 0){
        cout <<"no move found"<<endl;
        return getUserMove(game);
    }
    else if (goodMoves.size() > 1){
        string prom;
        cout << "enter promotion flag:"<<endl;
        cin >> prom;
        MoveType promFlag = MoveType(stoi(prom));
        for (int j = 0; j < goodMoves.size();){
            if (goodMoves[j].flag != promFlag){
                goodMoves.erase(goodMoves.begin()+j);
            } else j++;
        }
    }

    if (goodMoves.size() == 0){
        cout <<"no move found"<<endl;
        return getUserMove(game);
    }

    return goodMoves[0];
}
