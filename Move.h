#ifndef MOVE_H
#define MOVE_H


enum MoveType{
    quiet = 0,
    doublePawnPush = 1,
    kingCastle = 2,
    queenCastle = 3,
    captures = 4,
    epCature = 5,
    knightPromotion = 8,
    bishopPromotion = 9,
    rookPromotion = 10,
    queenPromotion = 11,
    knightPromoCapture = 12,
    bishopPromoCapture = 13,
    rookPromoCapture = 14,
    queenPromoCapture = 15
};

enum Piece{
    emptysquare = 0,
    wpawn = 1 | 8,
    wknight = 2 | 8, 
    wbishop = 3 | 8,
    wrook = 4 | 8,
    wqueen = 5 | 8,
    wking = 6 | 8,
    bpawn = 1 | 16,
    bknight = 2 | 16,
    bbishop = 3 | 16,
    brook = 4 | 16,
    bqueen = 5 | 16,
    bking = 6 | 16
};

class Move{
public:
    int from,to;
    MoveType flag;

    int scoreGuess = 0;

    Move(){
        from = 0;
        to = 0;
        flag = quiet;
    }

    Move(int afrom,int ato, MoveType aflag=quiet){
        from = afrom;
        to = ato;
        flag = aflag;
    }
    unsigned int flatten(){
        return (((from << 6)+to)<<4)+flag;
    }

    bool operator < (const Move & m) const{
        return scoreGuess < m.scoreGuess;
    }

};

#endif