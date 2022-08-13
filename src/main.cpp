#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>

#include "pos.h"
#include "piece.h"
#include "ray.h"
#include "board.h"
#include "move_generator.h"
#include "mcts.h"

int main()
{
    using std::cout;
    using std::endl;

    using std::vector;

    initRayTable();
    initKnightPositionTable();
    init_zobrist_nums();

    MoveGenerator mg;

    // ChessBoard board(false);
    // board.addPiece(ChessPos("a1"),ChessPiece(kPieceKing,kPlayerWhite));
    // board.addPiece(ChessPos("a8"),ChessPiece(kPieceRook,kPlayerBlack));
    // board.addPiece(ChessPos("b4"),ChessPiece(kPieceQueen,kPlayerWhite));

    // board.printBoard();
    // mg.setBoard(board);
    // mg.printAttacked();
    // mg.printForced();
    // cout << mg.inCheck(board) << endl;
    // for(ChessMove move : mg.getMoves()) cout << move.str() << endl;

    ChessBoard board;
    board.printBoard();

    MCTS mcts = MCTS(500);

    mg.setBoard(board);
    srand(time(NULL));
    board.printBoard();

    while(!mg.hasLost() && !mg.stalemate()){
        //mg.printAttacked();
        //mg.printForced();

        board.doMove(mcts.findOptimalMove(board));
        board.printBoard();

        mg.setBoard(board);
        if(mg.hasLost() || mg.stalemate()) break;

        const vector<ChessMove> &moves = mg.getMoves(board);
        board.doMove(moves[rand() % moves.size()]);
        mg.setBoard(board);
        board.printBoard();
    }

    if(mg.hasLost()) cout << "Lost!" << endl;
    else cout << "Stalemate!" << endl;


    // cout << ChessPos("a3").pos << endl;
    // cout << ChessPos("a3").str() << endl;
    // cout << sizeof(ChessPos("a3")) << endl;
    // cout << sizeof(ChessPiece(kPieceKing, kPlayerWhite)) << endl;
    // for(int start = 0; start < 64; start++){
    //     cout << ChessPos(start).str() << endl;
    //     cout << "Rays:" << endl;
    //     for(int dir = 0; dir < 8; dir++){
    //         cout << dir << endl;
    //         for(ChessPos pos : kRays[start][dir]) cout << pos.str() << endl;
    //     }
    //     cout << "Knight positions:" << endl;
    //     for(ChessPos pos : kKnightPositionTable[start]) cout << pos.str() << endl;
    //     cout << endl;
    // }
    return 0;
}