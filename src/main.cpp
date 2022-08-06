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
    
    // board.addPiece(ChessPos('a',1),ChessPiece(kPieceKing,kPlayerBlack));
    // board.addPiece(ChessPos('b',3),ChessPiece(kPieceQueen,kPlayerBlack));
    // board.addPiece(ChessPos('a',5),ChessPiece(kPieceRook,kPlayerWhite));
    // board.addPiece(ChessPos('a',6),ChessPiece(kPieceRook,kPlayerWhite));
    // board.addPiece(ChessPos('b',2),ChessPiece(kPiecePawn,kPlayerBlack));
    // board.addPiece(ChessPos('c',3),ChessPiece(kPieceBishop,kPlayerWhite));
    // board.resetAttacked();
    // board.printBoard();
    // //cout << board.kingPos(kPlayerWhite).str() << endl;
    // //cout << board.kingPos(kPlayerBlack).str() << endl;

    // board.printAttacked();

    // for(ChessMove move : board.getPossibleMoves()){
    //     cout << move.str() << endl;
    // }

    // ChessBoard board = ChessBoard();
    // board.doMove(ChessMove(ChessPiece(kPiecePawn, kPlayerWhite),ChessPos('d',2),ChessPos('d',4)));
    // board.doMove(ChessMove(ChessPiece(kPiecePawn, kPlayerBlack),ChessPos('f',7),ChessPos('f',6)));
    
    // board.printBoard();
    // for(ChessMove move : board.getPossibleMoves()) cout << move.str() << endl;

    ChessBoard board = ChessBoard();
    board.printBoard();
    srand(time(NULL));

    vector<ChessMove> moves;
    MCTS mcts = MCTS(50);
    Minimax minimax = Minimax(heuristic_basic, 3, true);
    
    while(!board.hasWon(kPlayerWhite) && !board.hasWon(kPlayerBlack)){
        board.doMove(mcts.findOptimalMove(board));
        //board.doMove(minimax.findOptimalMove(board));
        //moves = board.getPossibleMoves();
        //board.doMove(moves[rand() % moves.size()]);
        board.printBoard();

        if(board.state() == kStateWhiteWin || board.state() == kStateBlackWin || board.state() == kStateStalemate) break;

        moves = board.getPossibleMoves();
        board.doMove(moves[rand() % moves.size()]);
        board.printBoard();
        
    }

    return 0;
}