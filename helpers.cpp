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
#include "helpers.h"

using namespace std;


map<Piece,vector<int>> valueDifferentials = setValueDifferentials();
map<Piece,int> pieceValues = setPieceValues();

int offsets[6] = {0, 8, 4, 4, 8, 8}; /* knight or ray directions */
int offset[6][8] = {
	{   0,   0,  0,  0, 0,  0,  0,  0 },
	{ -21, -19,-12, -8, 8, 12, 19, 21 }, /* KNIGHT */
	{ -11,  -9,  9, 11, 0,  0,  0,  0 }, /* BISHOP */
	{ -10,  -1,  1, 10, 0,  0,  0,  0 }, /* ROOK */
	{ -11, -10, -9, -1, 1,  9, 10, 11 }, /* QUEEN */
	{ -11, -10, -9, -1, 1,  9, 10, 11 }  /* KING */
};

Piece getCodeFor(char c){
    if (c == 'P') return wpawn;
    else if (c == 'N') return wknight;
    else if (c == 'B') return wbishop;
    else if (c == 'R') return wrook;
    else if (c == 'Q') return wqueen;
    else if (c == 'K') return wking;
    else if (c == 'p') return bpawn;
    else if (c == 'n') return bknight;
    else if (c == 'b') return bbishop;
    else if (c == 'r') return brook;
    else if (c == 'q') return bqueen;
    else if (c == 'k') return bking;
    else return emptysquare;
}

char getCharFor(Piece piece){
    if (piece == emptysquare) return ' ';
    else if (piece == wpawn) return 'P';
    else if (piece == wknight) return 'N';
    else if (piece == wbishop) return 'B';
    else if (piece == wrook) return 'R';
    else if (piece == wqueen) return 'Q';
    else if (piece == wking) return 'K';
    else if (piece == bpawn) return 'p';
    else if (piece == bknight) return 'n';
    else if (piece == bbishop) return 'b';
    else if (piece == brook) return 'r';
    else if (piece == bqueen) return 'q';
    else if (piece == bking) return 'k';
    else return 'X';
}

Move unflatten(unsigned int flat){
    MoveType flag = MoveType(flat & 15);
    int to = (flat >> 4) & 63;
    int from = (flat >> 10);
    return Move(from,to,flag);
}

//WHAT????

vector<Move> unflattenList(vector<int> flatList){
    vector<Move> myMoves;
    for (auto f : flatList){
        myMoves.push_back(unflatten(f));
    }
    return myMoves;
}


bool onBoard(int x){
    return (0 <= x) && (x < 64);
}

bool colour(Piece piece){
    return 2 - (piece >> 3);
}

vector<int> flattenList(vector<Move> mlist){
    vector<int> myMoves;
    for (auto m : mlist){
        myMoves.push_back(m.flatten());
    }
    return myMoves;
}


string printLine(){
    string s = "";
    for (int i = 0; i < 8; i++){
        s += "____";
    }
    s += "_";
    return s;
}



int getIndexOfSquare(string a){
    int toCol = a[0]-97;
    int toRow = 56 - a[1];
    return 8 * toRow + toCol;
}

string getCoordinatesOfSquare(int n){
    int toRow = n / 8;
    int toCol = n% 8;
    char c = toCol + 97;
    char r = 56 - toRow;
    string s = "";
    s += c;
    s += r;
    return s;
}
