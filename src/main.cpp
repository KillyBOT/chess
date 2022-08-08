#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "move.h"
#include "board.h"
#include "move_generator.h"
#include "mcts.h"
#include "minimax.h"

int main(){

    using std::cout;
    using std::endl;
    using std::vector;

    MoveGenerator mg;

    // ChessBoard board = ChessBoard(false);
    // // board.addPiece(ChessPos('a',1),ChessPiece(kPieceKing,kPlayerWhite));
    // // board.addPiece(ChessPos('b',3),ChessPiece(kPieceQueen,kPlayerWhite));
    // // board.addPiece(ChessPos('a',5),ChessPiece(kPieceRook,kPlayerBlack));
    // // board.addPiece(ChessPos('a',6),ChessPiece(kPieceRook,kPlayerBlack));
    // // board.addPiece(ChessPos('b',2),ChessPiece(kPiecePawn,kPlayerWhite));
    // // board.addPiece(ChessPos('c',3),ChessPiece(kPieceBishop,kPlayerBlack));
    // board.addPiece(ChessPos('a',1),ChessPiece(kPieceKing,kPlayerWhite));
    // board.addPiece(ChessPos('h',1),ChessPiece(kPieceRook,kPlayerBlack));
    // board.addPiece(ChessPos('h',2),ChessPiece(kPieceRook,kPlayerBlack));

    // mg.setBoard(board);
    // board.printBoard();

    // mg.printAttacked();
    // if(mg.inCheck()) cout << "Check" << endl;
    // if(mg.hasLost()) cout << "Lost" << endl;
    // for(ChessMove move : mg.getMoves()) cout << move.str() << endl;

    // for(ChessMove move : board.getPossibleMoves()){
    //     cout << move.str() << endl;
    // }

    // ChessBoard board = ChessBoard();
    // board.doMove(ChessMove(ChessPiece(kPiecePawn, kPlayerWhite),ChessPos('d',2),ChessPos('d',4)));
    // board.doMove(ChessMove(ChessPiece(kPiecePawn, kPlayerBlack),ChessPos('f',7),ChessPos('f',6)));
    
    // board.printBoard();
    // for(ChessMove move : board.getPossibleMoves()) cout << move.str() << endl;

    ChessBoard board = ChessBoard();
    mg.setBoard(board);
    board.printBoard();
    srand(time(NULL));

    vector<ChessMove> moves;
    MCTS mcts = MCTS(200);
    Minimax minimax = Minimax(heuristic_basic, 3, true);
    
    while(!mg.hasLost() && !mg.stalemate()){
        //board.doMove(mcts.findOptimalMove(board));
        board.doMove(minimax.findOptimalMove(board));
        board.printBoard();
        mg.setBoard(board);

        if(mg.hasLost() || mg.stalemate()) break;
        moves = mg.getMoves(board);
        // for(ChessMove move : moves) cout << move.str() << ", ";
        // cout << endl;
        board.doMove(moves[rand() % moves.size()]);
        board.printBoard();
        mg.setBoard(board);
        
    }

    return 0;
}