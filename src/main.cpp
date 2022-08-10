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
    // board.addPiece(ChessPos('c',2),ChessPiece(kPiecePawn,kPlayerWhite));
    // board.addPiece(ChessPos('b',4),ChessPiece(kPiecePawn,kPlayerBlack));
    // board.addPiece(ChessPos('a',4),ChessPiece(kPieceKing,kPlayerBlack));
    // board.addPiece(ChessPos('h',4),ChessPiece(kPieceQueen,kPlayerWhite));

    // mg.setBoard(board);
    // board.printBoard();

    // vector<ChessMove> moves = mg.getMoves();
    // for(ChessMove move : moves) cout << move.str() << endl;
    // board.doMove(moves.back());
    // board.printBoard();

    // moves = mg.getMoves(board);
    // for(ChessMove move : moves) cout << move.str() << endl;
    // board.doMove(moves[0]);
    // board.printBoard();
    // board.undoLastMove();
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
    //Minimax minimax = Minimax(heuristic_basic, 4, false);
    
    while(!mg.hasLost() && !mg.stalemate()){
        board.doMove(mcts.findOptimalMove(board));
        //board.doMove(minimax.findOptimalMove(board));
        // for(ChessMove move : moves) cout << move.str() << ", ";
        // cout << endl;
        // moves = mg.getMoves(board);
        // board.doMove(moves[rand() % moves.size()]);
        mg.setBoard(board);
        board.printBoard();
        if(mg.hasLost() || mg.stalemate()) break;

        mg.setBoard(board);
        moves = mg.getMoves(board);
        // for(ChessMove move : moves) cout << move.str() << ", ";
        // cout << endl;
        board.doMove(moves[rand() % moves.size()]);
        board.printBoard();

        mg.setBoard(board);
        
    }

    return 0;
}