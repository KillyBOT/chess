#include <iostream>
#include <vector>
#include <string>

#include "pos.h"
#include "piece.h"
#include "ray.h"
#include "board.h"
#include "move_generator.h"

int main()
{
    using std::cout;
    using std::endl;

    initRayTable();
    initKnightPositionTable();
    init_zobrist_nums();

    ChessBoard board;

    cout << board.key() << endl;
    board.printBoard();

    MoveGenerator mg;

    mg.setBoard(board);
    mg.printAttacked();

    // cout << ChessPos("a3").pos << endl;
    // cout << ChessPos("a3").str() << endl;
    // cout << sizeof(ChessPos("a3")) << endl;
    // cout << sizeof(ChessPiece(kPieceKing, kPlayerWhite)) << endl;
    // for(int start = 0; start < 64; start++){
    //     cout << start << endl;
    //     cout << "Rays:" << endl;
    //     for(int dir = 0; dir < 8; dir++){
    //         cout << dir << endl;
    //         for(int i : gRays[start][dir]) cout << i << endl;
    //     }
    //     cout << "Knight positions:" << endl;
    //     for(int i : gKnightPositionTable[start]) cout << i << endl;
    //     cout << endl;
    // }
    return 0;
}