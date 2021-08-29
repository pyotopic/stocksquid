#ifndef GAME_H
#define GAME_H

#include <set>
#include <vector>
#include "Move.h"

using namespace std;

class Game{
public:
    Piece board[64];
    set<int> locations;
    //white to play
    bool wtp = true;
    //white kingside, white queenside, black kingside, black queenside; the value represents number of moves since last castling option was valid
    int castling[4] = {1,1,1,1};
    // castling states
    bool whiteCastled = false;
    bool blackCastled = false;
    //king locations
    int whiteKing = 60;
    int blackKing = 4;
    //50 move rule
    int halfMoves = 0;
    //moves describes what has happened in the game; in <move,piece>, move is the move made and piece is the piece type that was captured on move.to (0 by default)
    vector<pair<Move,Piece>> moves;
    //vector<unsigned int> flatMoves;
    //map<vector<unsigned int>,vector<Move>> savedMoves;
    //map<vector<unsigned int>,bool> seen;

    Game(string);
    friend ostream& operator<<(ostream& os, const Game& game);

    void makeMove(Move);
    void unmake();

    void clean();

    vector<Move> generatePseudoLegal();
    vector<Move> generateLegal();

    bool isInCheck(bool);

    int shallowValue();
    int heuristicValue();
    int evaluate(int);
    int evaluate2(int,int,int);
    Move getBestMove(int);
    Move getBestMove2(int);

    void printMove(Move);

    bool hasLegal();
    bool isLegal(Move);

    int gameOver();

    bool locationTest();

    int countMoves(int);
    int countPseudo(int);

    string gameString;
    void updateGameString();

    string convertMoveList();

    string convertMove(Move);

    Move convertString(string);

    void setMoveScore(Move&);

};


#endif