#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "move.h"
#include "board.h"
#include "mcts.h"


int main(){

    using std::cout;
    using std::endl;

    ChessBoard board = ChessBoard();
    // board.addPiece(ChessPos('a',8),ChessPiece(kPieceKing,kPlayerWhite));
    // board.addPiece(ChessPos('h',8),ChessPiece(kPieceQueen,kPlayerBlack));
    // for(ChessMove move : board.getPossibleMoves()){
    //     cout << move.pos.str() << ' ' << move.newPos.str() << endl;
    // }
    board.printBoard();

    srand(time(NULL));

    vector<ChessMove> moves;
    MCTS mcts = MCTS();
    
    while(!board.hasWon(kPlayerWhite) && !board.hasWon(kPlayerBlack)){
        board.doMove(mcts.findOptimalPath(board));
        board.printBoard();

        moves = board.getPossibleMoves();
        board.doMove(moves[rand() % moves.size()]);
        board.printBoard();
    }

    return 0;
}