#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "move.h"
#include "board.h"
#include "mcts.h"
#include "minimax.h"

int main(){

    using std::cout;
    using std::endl;
    using std::vector;

    // ChessBoard board = ChessBoard(false);
    
    // board.addPiece(ChessPos('e',1),ChessPiece(kPieceKing,kPlayerWhite));
    // board.addPiece(ChessPos('a',1),ChessPiece(kPieceRook,kPlayerWhite));
    // board.addPiece(ChessPos('h',1),ChessPiece(kPieceRook,kPlayerWhite));
    // board.addPiece(ChessPos('g',1),ChessPiece(kPiecePawn,kPlayerWhite));
    // board.printBoard();

    // for(ChessMove move : board.getPossibleMoves()){
    //     cout << move.basicStr() << endl;
    // }

    //board.doMove(ChessMove(ChessPiece(kPieceKing,kPlayerBlack),ChessPos('e',8),ChessPos('b',8)));
    //board.printBoard();

    ChessBoard board = ChessBoard();
    board.printBoard();
    srand(time(NULL));

    vector<ChessMove> moves;
    //MCTS mcts = MCTS();
    Minimax minimax = Minimax(heuristic_basic, 3, true);
    
    while(!board.hasWon(kPlayerWhite) && !board.hasWon(kPlayerBlack)){
        //board.doMove(mcts.findOptimalMove(board));
        board.doMove(minimax.findOptimalMove(board));
        board.printBoard();

        if(board.state() == kStateWhiteWin || board.state() == kStateBlackWin || board.state() == kStateStalemate) break;

        vector<ChessMove> moves = board.getPossibleMoves();
        board.doMove(moves[rand() % moves.size()]);
        board.printBoard();
        
    }

    return 0;
}