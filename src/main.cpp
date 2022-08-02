#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "move.h"
#include "board.h"

int main(){

    using std::cout;
    using std::endl;

    ChessBoard board = ChessBoard();
    board.printBoard();

    srand(time(NULL));

    vector<ChessMove> moves;
    int moveNum;
    for(int x = 0; x < 50; x++){
        moves = board.getPossibleMoves();
        moveNum = rand() % moves.size();
        board.doMove(moves[moveNum]);
        board.printBoard();
    }

    return 0;
}