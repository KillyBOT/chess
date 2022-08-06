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


    ChessBoard board = ChessBoard(false);
    
    board.addPiece(ChessPos('a',1),ChessPiece(kPieceKing,kPlayerWhite));
    board.addPiece(ChessPos('a',2),ChessPiece(kPieceQueen,kPlayerWhite));
    board.addPiece(ChessPos('a',5),ChessPiece(kPieceRook,kPlayerBlack));
    board.addPiece(ChessPos('a',6),ChessPiece(kPieceRook,kPlayerBlack));
    board.addPiece(ChessPos('b',2),ChessPiece(kPiecePawn,kPlayerWhite));
    board.addPiece(ChessPos('c',3),ChessPiece(kPieceBishop,kPlayerBlack));
    board.printBoard();
    //cout << board.kingPos(kPlayerWhite).str() << endl;
    //cout << board.kingPos(kPlayerBlack).str() << endl;

    for(ChessPos pinned : board.pinned()){
        cout << pinned.str() << endl;
    }

    // ChessBoard board = ChessBoard();
    // board.printBoard();
    // srand(time(NULL));

    // vector<ChessMove> moves;
    // //MCTS mcts = MCTS();
    // Minimax minimax = Minimax(heuristic_basic, 3, true);
    
    // while(!board.hasWon(kPlayerWhite) && !board.hasWon(kPlayerBlack)){
    //     //board.doMove(mcts.findOptimalMove(board));
    //     moves = board.getPossibleMoves();
    //     board.doMove(moves[rand() % moves.size()]);
    //     board.printBoard();

    //     if(board.state() == kStateWhiteWin || board.state() == kStateBlackWin || board.state() == kStateStalemate) break;

    //     moves = board.getPossibleMoves();
    //     board.doMove(moves[rand() % moves.size()]);
    //     board.printBoard();
        
    // }

    return 0;
}