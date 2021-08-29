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

#include "Game.h"
#include "Move.h"
#include "helpers.h"

using namespace std;


Game::Game(string fenString){
    //board = new Piece [64];
    for(int i = 0; i < 64; i++){
        board[i] = emptysquare;
    }
    int i = 0;
    int j = 0;
    int spaces = 0;
    Move lastMove = Move();
    for (int k = 0; k < fenString.length()-1; k++){
        if (fenString[k] == ' '){
            spaces += 1;
            continue;
        }
        if (spaces == 0){
            if (fenString[k]=='/'){
                i += 1;
                j = 0;
            }
            else if (48 < fenString[k] && fenString[k] <= 56) j += fenString[k] - 48;
            else{
                board[8*i+j] = getCodeFor(fenString[k]);
                locations.insert(8*i+j);
                //cout<<"good for position "<<8 * i  + j << endl;
                if (fenString[k] == 'K') whiteKing = 8*i + j;
                else if (fenString[k] == 'k') blackKing = 8 * i + j;
                j += 1;
            }
        }
        else if (spaces == 1){
            //player to play
            if (fenString[k] == 'w') wtp = true;
            else wtp = false;
        }
        else if (spaces == 2){
            //castling availability
            if (fenString[k] == '-'){
                for (int i = 0; i < 4; i++) castling[i] = 1;
            }
            else if (fenString[k] == 'K') castling[0] = 0;
            else if (fenString[k] == 'Q') castling[1] = 0;
            else if (fenString[k] == 'k') castling[2] = 0;
            else if (fenString[k] == 'q') castling[3] = 0;
        }
        else if (spaces == 3){
            if (fenString[k] == '-') continue;
            int col = fenString[k]-97;
            int row = 56 - fenString[k+1];
            int start = row + 8 - 16 * wtp;
            int end = row - 8 + 16 * wtp;
            lastMove.from = 8 * start + col;
            lastMove.to = 8 * end + col;
            lastMove.flag = doublePawnPush;
            moves.push_back(make_pair(lastMove,emptysquare));
            k++;
        }
        else if (spaces == 4){
            halfMoves = fenString[k] - 48;
        }
        else if (spaces == 5){
            //I lazily don't care about this
            break;
        }
    }
    cout<<"done"<<endl;
}


int Game::countPseudo(int depth){
    vector<Move> pseudos = generatePseudoLegal();
    if (depth == 0) return pseudos.size();
    int totalMoves = 0;
    for (int i = 0; i < pseudos.size(); i++){
        makeMove(pseudos[i]);
        totalMoves += countPseudo(depth-1);
        unmake();
    }
    return totalMoves;
}

int Game::countMoves(int depth){
    vector<Move> legalMoves = generateLegal();
    if (depth == 0) return legalMoves.size();
    int totalMoves = 0;
    for (int i = 0; i < legalMoves.size(); i++){
        makeMove(legalMoves[i]);
        totalMoves += countMoves(depth-1);
        unmake();
    }
    return totalMoves;
}

void Game::clean(){
    return;
}

int Game::heuristicValue(){
    int score = 0;
    //slow, but good maybe?
    
    vector<Move> movesSide = generateLegal();

    if (movesSide.size() == 0){
        if (isInCheck(wtp)){
            return bigScore * (1 - 2 * wtp);
        } else return 0;
    }
    int space = 0;
    

    //can be alpha/betaed
    // if (depth > 0){
    //     bool foundCaptures = false;
    //     int value = 0;
    //     for (int i = 0; i < movesSide.size(); i++){
    //         if (movesSide[i].flag == captures || movesSide[i].flag > 11){
    //             foundCaptures = true;
    //             //there is a capture, play it
    //             makeMove(movesSide[i]);
    //             if (wtp) value = max(value,heuristicValue(depth-1));
    //             else value = min(value,heuristicValue(depth-1));
    //             unmake();
    //         }
    //     }
    //     if (foundCaptures) return value;
    // }
    
    
    Move emptyMove = Move(24,24,quiet);
    makeMove(emptyMove);
    vector<Move> movesNotSide = generateLegal();
    unmake();

    int playerMobility = movesSide.size();
    int notPlayerMobility = movesNotSide.size();

    int spaceDifference = (playerMobility - notPlayerMobility) * (2 * wtp - 1);
    score += 10 * spaceDifference;
    score += 50 * (whiteCastled - blackCastled);
    
    for (auto n : locations){
        score += pieceValues[board[n]];
    }
    
    //size -= 2 * pieceValues[wking];
    //int sparse = (7800 - (size * 100))/100;
    //score += sparse * score * (2 * wtp - 1) / 100;
    return score;
}

int Game::gameOver(){
    //returns:
    //-1 if not over
    // 0 if win for black
    // 1 if win for white
    // 2 if draw
    if (!hasLegal()){
        if (isInCheck(wtp)) return !wtp;
        else return 2;
    }

    return -1;
}

bool Game::locationTest(){
    for (auto n: locations){
        if (board[n] == emptysquare){
            return false;
        }
    }
    for (int i = 0; i < 64; i++){
        if (board[i] != emptysquare){
            if (locations.count(i) == 0){
                return false;
            }
        }
    }
    return true;
}

void Game::printMove(Move move){
    Piece piece = board[move.from];
    char pieceCode = getCharFor(piece);
    cout << pieceCode;
    char file = 97 + (move.to%8);
    int row = 8 - (move.to / 8);
    cout << file << row<<endl;
}
int Game::shallowValue(){
    //vector<Move> a = generateLegal();
    // if (a.size() == 0){
    //      if (isInCheck(wtp)) return bigScore * (1 - 2 * wtp);
    //      else return 0;
    // }

    if (!hasLegal()){
        if (isInCheck(wtp)) return (bigScore) * (1 - 2 * wtp);
        else return 0;
    }

    int score = 0;
    for (auto n: locations){
        score += pieceValues[board[n]];
        //cout << "n is "<<n << " board is "<<board[n]<<endl;
        if (board[n] == emptysquare){
            cout<<"location error" << endl;
            cout <<"run location test"<<endl;
        }
        score += valueDifferentials[board[n]][n];
    }
    return score;
}

int Game::evaluate(int depth){
    if (depth == 0) return shallowValue();
    
    vector<Move> legalMoves = generateLegal();
    if (legalMoves.size() == 0){
        if (isInCheck(wtp)) return (bigScore + depth) * (1 - 2 * wtp);
        else return 0;
    }
    int bestScore = bigScore * (1 - 2 * wtp);
    //int bestIndex = 0;
    for (int i = 0; i < legalMoves.size(); i++){
        makeMove(legalMoves[i]);
        int newValue = evaluate(depth - 1);
        unmake();
        bool change = ((wtp && newValue > bestScore) || (!wtp && newValue < bestScore));
        bestScore += (newValue - bestScore) * change;
        //bestIndex += (i - bestIndex) * change;
    }
    
    return bestScore;
}

int Game::evaluate2(int depth, int alpha, int beta){
    //testing heuristic value, likely slower but better play
    if (depth == 0) return heuristicValue();
    
    vector<Move> legalMoves = generateLegal();
    if (legalMoves.size() == 0){
        if (isInCheck(wtp)) return (bigScore + depth) * (1 - 2 * wtp);
        else return 0;
    }

    if (wtp){
        int value = negativeInfinity;
        for (int i = 0; i < legalMoves.size(); i++){
            makeMove(legalMoves[i]);
            value = max(value,evaluate2(depth-1,alpha,beta));
            unmake();
            alpha = max(alpha,value);
            if (alpha >= beta) break;
        }
        return value;
    }
    else{
        int value = positiveInfinity;
        for (int i = legalMoves.size()-1; i >= 0; i--){
            makeMove(legalMoves[i]);
            value = min(value, evaluate2(depth-1,alpha,beta));
            unmake();
            beta = min(beta, value);
            if (beta <= alpha) break;
        }
        return value;
    }
}

Move Game::getBestMove2(int depth){
    int bestIndex = 0;
    cout <<"It is white to play: "<<wtp<<endl;
    int bestScore = wtp * negativeInfinity + (1-wtp) * positiveInfinity;
    int alpha = negativeInfinity;
    int beta = positiveInfinity;
    vector<Move> legalMoves = generateLegal();
    //sort(legalMoves.begin(), legalMoves.end());
    int value;
    if (wtp){
        value = negativeInfinity;
        for (int i = 0; i < legalMoves.size();i++){
            cout << "The move is "<<legalMoves[i].from <<" "<<legalMoves[i].to << endl;
            makeMove(legalMoves[i]);
            int newValue = evaluate2(depth-1,alpha,beta);
            unmake();
            if (newValue > value){
                value = newValue;
                bestIndex = i;
            }
            alpha = max(alpha,value);
            if (alpha >= beta) break;
        }
    }
    else{
        value = positiveInfinity;
        for (int i = legalMoves.size()-1; i >= 0;i--){
            cout << "The move is "<<legalMoves[i].from <<" "<<legalMoves[i].to << endl;
            makeMove(legalMoves[i]);
            int newValue = evaluate2(depth-1,alpha,beta);
            unmake();
            if (newValue < value){
                value = newValue;
                bestIndex = i;
            }
            beta = min(beta,value);
            if (beta <= alpha) break;
        }
    }
    cout << "The score is "<<value<<endl;
    return legalMoves[bestIndex];
}


Move Game::getBestMove(int depth){
    int bestIndex = 0;
    cout <<"It is white to play: "<<wtp<<endl;
    int bestScore = bigScore * (1 - 2 * wtp);
    vector<Move> legalMoves = generateLegal();
    for (int i = 0; i < legalMoves.size(); i++){
        cout <<"The move is "<<legalMoves[i].from<<" "<<legalMoves[i].to<<endl;
        makeMove(legalMoves[i]);
        //cout << this;
        int newValue = evaluate(depth - 1);
        unmake();
        bool change = ((wtp && newValue > bestScore) || (!wtp && newValue < bestScore));
        bestScore += (newValue - bestScore) * change;
        bestIndex += (i - bestIndex) * change;
    }
    cout<<"Score is "<<bestScore<<endl;
    return legalMoves[bestIndex];
}

void Game::setMoveScore(Move& move){
    int guess = 0;
    if (board[move.to] != emptysquare){
        guess =  10 * pieceValues[board[move.to]] - pieceValues[board[move.from]];
    }
    if (move.flag > 7){
        // 8 + 8 * wtp is colour, 2 + last 2 bits of move.flag give piece
        guess -= pieceValues[Piece(8 + 8 * wtp + (move.flag & 3) + 2)];
    }
    move.scoreGuess = guess;
}

vector<Move> Game::generateLegal(){

    // if (seen[flatMoves]){
    //     return savedMoves[flatMoves];
    // }

    vector<Move> pseudos = generatePseudoLegal();
    for (int i = 0; i < pseudos.size();){
        Move move = pseudos[i];
        makeMove(move);
        if (isInCheck(!wtp)){
            unmake();
            pseudos.erase(pseudos.begin()+i);
            setMoveScore(move);
        } else{
            unmake();
            i++;
        }
        //unmake();
        //i++;
    }
    //sort(pseudos.begin(),pseudos.end());
    // seen[flatMoves] = true;
    // savedMoves[flatMoves] = pseudos;

    return pseudos;
}

bool Game::hasLegal(){
    //bad solution; during the loop through locations, `locations' is edited during makeMove/unmake, changing the order and creating an infinite loop whereby we never finish iterating through locations. Making a shallow copy resolves this issue, but negates the speedup from having the locations array in the first place.
    set<int> shlocations = locations;
    for (auto n: shlocations){
        Piece piece = board[n];
        if (colour(piece) != wtp) continue;
        int pieceCode = piece & 7;
        //pawn
        if (pieceCode == 1){
            int d = 1 - 2 * wtp;
            int startRow = 1 + 5 * wtp;
            int endRow = 7 *(1-wtp);
            if (board[n+d * 8] == emptysquare){
                if (n/8 + d == endRow){
                    for (int i = 0; i < 4; i++){
                        if (isLegal(Move(n,n+d*8,MoveType(8+i)))) return true;
                    }
                }
                else{
                    if (isLegal(Move(n,n+d*8))) return true;
                }

                if (n/8==startRow && board[n+2 * d*8] == emptysquare){
                    if (isLegal(Move(n,n+2*d*8,doublePawnPush))) return true;
                }
            }
            for (int i = 0; i < 2; i++){
                int c = 2 * i - 1;
                int target = mailbox[mailboxes[n] + d * 10 + c];
                if (target == -1) continue;
                if (board[target] != emptysquare){
                    if (colour(board[target]) != wtp){
                        //can capture
                        if (target/8 == endRow){
                            for (int i = 0; i < 4; i++){
                                if (isLegal(Move(n,target,MoveType(12+i)))) return true;
                            }
                        }
                        else if (isLegal(Move(n,target,captures))) return true;
                    }
                }
                else {
                    if (n/8 != 4 - wtp) continue;
                    int epSquare  =  8 * (n / 8) + target % 8;
                    if (board[epSquare] == (8 + 8 * (wtp) + 1)){
                        if (moves.size() == 0) continue;
                        Move lastMove = moves[moves.size()-1].first;
                        if (lastMove.to == epSquare && lastMove.flag == doublePawnPush){
                            if (isLegal(Move(n,target,epCature))) return true;
                        }
                    }
                }
            }
        }
        else{
            if (pieceCode == 6){
                //castling
                if ((wtp && (castling[0]==0)) || (!wtp && (castling[2]==0))){
                    if (board[n+1]==emptysquare && board[n+2]==emptysquare){
                        Move castle = Move(n,n+2,kingCastle);
                        //check to see if castling through check
                        //kingside
                        //need n, n+1 to be check free for pseudo-legality
                        bool legal = true;
                        if (isInCheck(wtp)) legal = false;
                        else if (!isLegal(Move(n,n+1,quiet))) legal = false;
                        //if (isInCheck(!wtp)) legal = false;
                        if (legal) if (isLegal(castle)) return true;
                        //if (isLegal(castle);
                    }
                }
                if ((wtp && (castling[1]==0)) || (!wtp && (castling[3]==0))){
                    if ((board[n-1] | (board[n-2] | board[n-3])) == 0){
                        Move castle = Move(n,n-2,queenCastle);
                        //check to see if castling through check...
                        bool legal = true;
                        if (isInCheck(wtp)) legal = false;
                        if (!isLegal(Move(n,n-1,quiet))) legal = false;
                        if (legal) if (isLegal(castle)) return true;
                        //if (isLegal(castle);
                    }
                }
            }

            int p = pieceCode - 1;
            for (int j = 0; j < offsets[p]; ++j) { /* for all knight or ray directions */
                for (int m=n;;) { /* starting with from square */
                    m = mailbox[mailboxes[m] + offset[p][j]]; /* next square along the ray j */
                    if (m == -1) break; /* outside board */

                    if (board[m] != emptysquare) {
                        if (2-(board[m] >> 3) != wtp) if (isLegal(Move(n,m,captures))) return true;
                        break;
                    }
                    if (isLegal(Move(n,m))) return true;
                    if (!slide[p]) break; /* next direction */
                }
            }
        }
    }

    return false;
}

bool Game::isInCheck(bool side){
    int n = (side * whiteKing) + ((!side) * blackKing);
    for (int p = 1; p < 4; p++){
        int pieceCode = p + 1;
        for (int j = 0; j < offsets[p]; ++j) { /* for all knight or ray directions */
            bool oneAway = true;
            for (int m=n;;) { /* starting with from square */
                m = mailbox[mailboxes[m] + offset[p][j]]; /* next square along the ray j */
                if (m == -1) break; /* outside board */

                if (board[m] != emptysquare) {
                    if (colour(board[m]) != side){
                        if (pieceCode == 3){
                            if (((board[m]&7) == 3) || ((board[m]&7) == 5)) return true;
                            if (oneAway){
                                if ((board[m]&7) == 6) return true;
                                if ((board[m]&7) == 1){
                                    if (side && (offset[p][j] == -11 || offset[p][j] == -9)) return true;
                                    if (!side && (offset[p][j] == 11 || offset[p][j] ==  9)) return true;
                                }
                            }
                        }
                        else if (pieceCode == 4){
                            if (((board[m]&7) == 4) || ((board[m]&7) == 5)) return true;
                            if (oneAway && ((board[m]&7)==6)) return true;
                        }
                        else if (pieceCode == 2 && ((board[m] & 7) == 2)) return true;
                    }
                    break;
                }
                oneAway = false;
                if (!slide[p]) break; /* next direction */
            }
        }
    }

    return false;
}

bool Game::isLegal(Move move){
    makeMove(move);
    if (isInCheck(!wtp)){
        unmake();
        return false;
    } else{
        unmake();
        return true;
    }
}

vector<Move> Game::generatePseudoLegal(){
    vector<Move> moveList;
    //bad solution; during the loop through locations, `locations' is edited during makeMove/unmake, changing the order and creating an infinite loop whereby we never finish iterating through locations. Making a shallow copy resolves this issue, but negates the speedup from having the locations array in the first place.
    set<int> shlocations = locations;
    for (auto n: shlocations){
        Piece piece = board[n];
        if (colour(piece) != wtp) continue;
        int pieceCode = piece & 7;
        //pawn
        if (pieceCode == 1){
            int d = 1 - 2 * wtp;
            int startRow = 1 + 5 * wtp;
            int endRow = 7 *(1-wtp);
            if (board[n+d * 8] == emptysquare){
                if (n/8 + d == endRow){
                    for (int i = 0; i < 4; i++){
                        moveList.push_back(Move(n,n+d*8,MoveType(8+i)));
                    }
                }
                else{
                    moveList.push_back(Move(n,n+d*8));
                }

                if (n/8==startRow && board[n+2 * d*8] == emptysquare){
                    moveList.push_back(Move(n,n+2*d*8,doublePawnPush));
                }
            }
            for (int i = 0; i < 2; i++){
                int c = 2 * i - 1;
                int target = mailbox[mailboxes[n] + d * 10 + c];
                if (target == -1) continue;
                if (board[target] != emptysquare){
                    if (colour(board[target]) != wtp){
                        //can capture
                        if (target/8 == endRow){
                            for (int i = 0; i < 4; i++){
                                moveList.push_back(Move(n,target,MoveType(12+i)));
                            }
                        }
                        else moveList.push_back(Move(n,target,captures));
                    }
                }
                else {
                    if (n/8 != 4 - wtp) continue;
                    int epSquare  =  8 * (n / 8) + target % 8;
                    if (board[epSquare] == (8 + 8 * (wtp) + 1)){
                        if (moves.size() == 0) continue;
                        Move lastMove = moves[moves.size()-1].first;
                        if (lastMove.to == epSquare && lastMove.flag == doublePawnPush){
                            moveList.push_back(Move(n,target,epCature));
                        }
                    }
                }
            }
        }
        else{
            if (pieceCode == 6){
                //castling
                if ((wtp && (castling[0]==0)) || (!wtp && (castling[2]==0))){
                    if (board[n+1]==emptysquare && board[n+2]==emptysquare){
                        Move castle = Move(n,n+2,kingCastle);
                        //check to see if castling through check
                        //kingside
                        //need n, n+1 to be check free for pseudo-legality
                        bool legal = true;
                        if (isInCheck(wtp)) legal = false;
                        else if (!isLegal(Move(n,n+1,quiet))) legal = false;
                        //if (isInCheck(!wtp)) legal = false;
                        if (legal) moveList.push_back(castle);
                        //moveList.push_back(castle);
                    }
                }
                if ((wtp && (castling[1]==0)) || (!wtp && (castling[3]==0))){
                    if ((board[n-1] | (board[n-2] | board[n-3])) == 0){
                        Move castle = Move(n,n-2,queenCastle);
                        //check to see if castling through check...
                        bool legal = true;
                        if (isInCheck(wtp)) legal = false;
                        if (!isLegal(Move(n,n-1,quiet))) legal = false;
                        if (legal) moveList.push_back(castle);
                        //moveList.push_back(castle);
                    }
                }
            }

            int p = pieceCode - 1;
            for (int j = 0; j < offsets[p]; ++j) { /* for all knight or ray directions */
                for (int m=n;;) { /* starting with from square */
                    m = mailbox[mailboxes[m] + offset[p][j]]; /* next square along the ray j */
                    if (m == -1) break; /* outside board */

                    if (board[m] != emptysquare) {
                        if (2-(board[m] >> 3) != wtp) moveList.push_back(Move(n,m,captures));
                        break;
                    }
                    moveList.push_back(Move(n,m));
                    if (!slide[p]) break; /* next direction */
                }
            }
        }
    }

    return moveList;
}


void Game::unmake(){

    // if (!locationTest()){
    //     cout <<" failed pre-unmake"<<endl;
    //     cout<<"location error"<<endl;
    // }

    pair<Move,Piece> lastInfo = moves[moves.size()-1];
    Move move = lastInfo.first;
    Piece captured = lastInfo.second;

    if (move.from == move.to){
        //empty move
        moves.pop_back();
        //flatMoves.pop_back();
        wtp = !wtp;
        return;
    }


    locations.insert(move.from);

    for (int i = 0; i < 4; i++){
        castling[i] -= (castling[i] > 0);
    }
    halfMoves -= 1;

    if ((move.flag == 0) || (move.flag == 1)){
        board[move.from] = board[move.to];
        board[move.to] = emptysquare;
        locations.erase(move.to);
        if (board[move.from] == wking) whiteKing = move.from;
        else if (board[move.from] == bking) blackKing = move.from;
    }
    else if (move.flag == 4){
        board[move.from] = board[move.to];
        board[move.to] = captured;
        if (board[move.from] == wking) whiteKing = move.from;
        else if (board[move.from] == bking) blackKing = move.from;
    }
    else if (move.flag >= 8 && move.flag < 12){
        board[move.from] = Piece((board[move.to] & 24) + 1);
        board[move.to] = emptysquare;
        locations.erase(move.to);
    }
    else if (move.flag >= 12){
        board[move.from] = Piece((board[move.to] & 24) + 1);
        board[move.to] = captured;
    }
    else if (move.flag == 5){
        //ep-capture
        int epSquare = 8 * (move.from / 8) + move.to % 8;
        board[move.from] = board[move.to];
        board[move.to] = emptysquare;
        board[epSquare] = captured;
        locations.erase(move.to);
        locations.insert(epSquare);
    }
    else if (move.flag == 2){
        board[move.from] = board[move.to];
        board[move.to] = emptysquare;
        locations.erase(move.to);
        board[move.to+1] = board[move.to-1];
        board[move.to-1] = emptysquare;
        locations.insert(move.to+1);
        locations.erase(move.to-1);

        if (!wtp){
            whiteCastled = false;
        } else blackCastled = false;

        if (board[move.from] == wking) whiteKing = move.from;
        else if (board[move.from] == bking) blackKing = move.from;
    }
    else{
        //queen castle
        board[move.from] = board[move.to];
        board[move.to] = emptysquare;
        locations.erase(move.to);
        board[move.to-2] = board[move.to+1];
        board[move.to+1] = emptysquare;
        locations.insert(move.to-2);
        locations.erase(move.to+1);
        if (!wtp){
            whiteCastled = false;
        } else blackCastled = false;

        if (board[move.from] == wking) whiteKing = move.from;
        else if (board[move.from] == bking) blackKing = move.from; 
    }
    moves.pop_back();
    //flatMoves.pop_back();
    wtp = !wtp;

    // if (!locationTest()){
    //     cout <<" failed post-unmake"<<endl;
    //     cout<<"location error"<<endl;
    // }
}

void Game::makeMove(Move move){
    //move.from = 0, move.to = 1

    // if (!locationTest()){
    //     cout <<" failed pre-make"<<endl;
    //     cout<<" location error "<<endl;
    // }

    if (move.from == move.to){
        //empty move
        wtp = !wtp;
        moves.push_back(make_pair(move,emptysquare));
        //flatMoves.push_back(move.flatten());
        return;
    }

    locations.erase(move.from);
    locations.insert(move.to);
    Piece piece = board[move.from];

    for (int i = 0; i < 4; i++){
        castling[i] += (castling[i] > 0);
    }

    halfMoves += 1;

    if ((move.flag == 0) || (move.flag == 1 || move.flag == 4)){
        Piece captured = board[move.to];
        board[move.to] = board[move.from];
        board[move.from] = emptysquare;
        moves.push_back(make_pair(move,captured));

        // if (!((piece & 7) == 6 || (captured & 7) == 4)){
        //     wtp = !wtp;
        //     return;
        // }

        if (piece == wking){
            castling[0] += (castling[0] == 0);
            castling[1] += (castling[1] == 0);
            whiteKing = move.to;
        }
        else if (piece == wrook){
            castling[0] += ((move.from % 8) == 7) * (castling[0] == 0);
            castling[1] += ((move.from % 8) == 0) * (castling[1] == 0);
        }
        else if (piece == bking){
            castling[2] += (castling[2] == 0);
            castling[3] += (castling[3] == 0);
            blackKing = move.to;
        }
        else if (piece == brook){
            castling[2] += ((move.from % 8) == 7) * (castling[2] == 0);
            castling[3] += ((move.from % 8) == 0) * (castling[3] == 0);
        }

        if (captured == brook){
            castling[2] += ((move.to % 8) == 7) * (castling[2] == 0);
            castling[3] += ((move.to % 8) == 0) * (castling[3] == 0);
        }
        else if (captured == wrook){
            castling[0] += ((move.to % 8) == 7) * (castling[0] == 0);
            castling[1] += ((move.to % 8) == 0) * (castling[1] == 0);
        }
        
    }
    else if ((move.flag & 8)>>3){
        Piece newPiece = Piece((move.flag & 3) + 2 + (piece & 24));
        Piece captured = board[move.to];
        board[move.to] = newPiece;
        board[move.from] = emptysquare;
        moves.push_back(make_pair(move,captured));
    }
    else if (move.flag == 5){
        int epSquare = 8 * (move.from / 8) + move.to % 8;
        Piece captured = board[epSquare];
        board[epSquare] = emptysquare;
        locations.erase(epSquare);
        board[move.to] = board[move.from];
        board[move.from] = emptysquare;
        moves.push_back(make_pair(move,captured));
    }
    else if (move.flag == 2){
        //king castle
        board[move.to] = board[move.from];
        board[move.from] = emptysquare;
        board[move.to-1] = board[move.to+1];
        board[move.to+1] = emptysquare;
        locations.insert(move.to-1);
        locations.erase(move.to+1);
        moves.push_back(make_pair(move,emptysquare));
        if (wtp){
            castling[0] += (castling[0] == 0);
            castling[1] += (castling[1] == 0);
            whiteKing = move.to;
            whiteCastled = true;
        } else{
            castling[2] += (castling[2] == 0);
            castling[3] += (castling[3] == 0);
            blackKing = move.to;
            blackCastled = true;
        }
    }
    else{
        //queen castle
        board[move.to] = board[move.from];
        board[move.from] = emptysquare;
        board[move.to+1] = board[move.to-2];
        board[move.to-2] = emptysquare;
        locations.insert(move.to+1);
        locations.erase(move.to-2);
        moves.push_back(make_pair(move,emptysquare));
        if (wtp){
            castling[0] += (castling[0] == 0);
            castling[1] += (castling[1] == 0);
            whiteKing = move.to;
            whiteCastled = true;
        } else{
            castling[2] += (castling[2] == 0);
            castling[3] += (castling[3] == 0);
            blackKing = move.to;
            blackCastled = true;
        }
    }

    //flatMoves.push_back(move.flatten());

    wtp = !wtp;

    // if (!locationTest()){
    //     cout <<" failed post-make "<<endl;
    //     cout<<" location error "<<endl;
    // }

}



string Game::convertMoveList(){
    string s;


    return s;
}

string Game::convertMove(Move move){
    string s;

    if (wtp){
        s += to_string(1 + moves.size()/2);
        s += ".";
        s += " ";
    }

    bool inCheck = false;
    string check = "";
    makeMove(move);
    if (isInCheck(wtp)){
        check = "+";
        if (generateLegal().size() == 0){
            check = "#";
        }
    }
    unmake();

    string capture = "";

    if (move.flag == kingCastle){
        s += "O-O ";
        return s;
    } else if (move.flag == queenCastle){
        s += "O-O-O ";
        return s;
    } else if (move.flag == captures || (move.flag > 11 || move.flag == epCature)){
        capture = "x";
    }

    if ((board[move.from]&7) == 1){
        //pawn
        if (move.flag == quiet || move.flag == doublePawnPush){
            s += getCoordinatesOfSquare(move.to);
            s += check;
            s += " ";
            return s;
        }
        if (move.flag > 11){
            //capture promo
            Piece toPiece = Piece(8|(move.flag & 7));
            char p = getCharFor(toPiece);
            char c = 97 + (move.from % 8);
            s += c;
            s += capture;
            s += getCoordinatesOfSquare(move.to);
            s += "=";
            s += p;
            s += check;
            s += " ";
            return s;
        }
        else if (move.flag > 7){
            Piece toPiece = Piece(8|(move.flag & 7));
            char p = getCharFor(toPiece);
            //char c = 97 + (move.from % 8);
            s += getCoordinatesOfSquare(move.to);
            //s += capture;
            //s += target;
            s += "=";
            s += p;
            s += check;
            s += " ";
            return s;
        } else{
            //ep
            char c = 97 + (move.from % 8);
            s += c;
            s += captures;
            s += getCoordinatesOfSquare(move.to);
            s += check;
            s += " ";
            return s;
        }
    }


    char code = getCharFor(board[move.from]);
    code = toupper(code);
    s += code;

    string target = getCoordinatesOfSquare(move.to);    

    for (auto mv : generateLegal()){
        if (mv.to == move.to){
            if (board[move.from] == board[mv.from]){
                if (move.from == mv.from){
                    //might be promotion
                    if (move.flag == mv.flag) continue;
                    Piece toPiece = Piece(8|(move.flag & 7));
                    char p = getCharFor(toPiece);
                    s += capture;
                    s += target;
                    s += "=";
                    s += p;
                    s += check;
                    s += " ";
                    return s;
                }
                if (mv.from / 8 == move.from / 8){
                    char col = 97 + (move.from % 8);
                    s += col;
                    s += capture;
                    s += target;
                    s += check;
                    s += " ";
                    return s;
                }
                char row = 56 - (move.from / 8);
                s += row;
                s += capture;
                s += target;
                s += check;
                s += " ";
                return s;
            }
        }
    }
    s += capture;
    s += target;
    s += check;
    s += " ";

    return s;
}

Move Game::convertString(string s){
    Move move;
    //MESSY MESS NEEDS TO BE FIXED;

    vector<Move> legalMoves = generateLegal();

    //need to identify fromSquare, toSquare, flag
    //so need to search pieces to identify square
    //if multiple results, look at next char
    
    int from;
    int to;
    MoveType flag;

    if ((97 <= s[0]) && (s[0] < 105)){
        //pawn
        //deal with later
        if (s[1] == 'x'){

        }
        


        return move;
    }

    Piece piece = Piece(getCodeFor(s[0]) + wtp * 8);
    //piece identified
    vector<int> squares;
    
    for (auto n : locations){
        if (board[n] == piece){
            squares.push_back(n);
        }
    }


    return move;
}

ostream& operator<<(ostream& os, const Game& game){
    os<<printLine();
    os << "\n";
    for (int i = 0; i < 8; i++){
        os << "|";
        for (int j = 0; j < 8; j++){
            os << " ";
            char c = getCharFor(game.board[8*i+j]);
            os << c;
            os << " |";
        }
        os << "\n";
        os<<printLine();
        os << "\n";
    }
    return os;
}