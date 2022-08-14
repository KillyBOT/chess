#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <unordered_set>

#include "pos.h"
#include "piece.h"
#include "ray.h"
#include "board.h"
#include "move_generator.h"
#include "mcts.h"
#include "minimax.h"

int move_generation_test(ChessBoard &board, int depth){

    static MoveGenerator mg;

    if(depth <= 0) {
        board.printBoard();
        board.printMoves();
    }

    if(depth <= 0) return 1;

    int positionNum = 0;

    for(ChessMove move : mg.getMoves(board)){
        board.doMove(move);
        positionNum += move_generation_test(board, depth-1);
        board.undoLastMove();
    }

    return positionNum;
}

int main()
{
    using std::cout;
    using std::endl;

    using std::vector;
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::seconds;
    using std::chrono::system_clock;

    initRayTable();
    initKnightPositionTable();
    init_zobrist_nums();

    MoveGenerator mg;

    // ChessBoard board(false);
    // board.addPiece(ChessPos("a1"),ChessPiece(kPieceKing,  kPlayerWhite));
    // board.addPiece(ChessPos("h8"),ChessPiece(kPieceKing,kPlayerBlack));
    // board.addPiece(ChessPos("c2"),ChessPiece(kPiecePawn, kPlayerWhite));
    // board.addPiece(ChessPos("d4"),ChessPiece(kPiecePawn, kPlayerBlack));

    // board.printBoard();
    // board.doMove(mg.getMoves(board)[4]);
    // mg.setBoard(board);
    // board.printBoard();
    // mg.printAttacked();
    // for(ChessMove move : mg.getMoves()) cout << move.str() << endl;

    ChessBoard board;
    board.printBoard();

    for(int i = 1; i < 4; i++){
        auto startTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        int positionNum = move_generation_test(board, i);
        auto endTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        cout << i << '\t' << positionNum << '\t' << endTime - startTime << endl;
    }

    // MCTS mcts = MCTS(2000);
    // Minimax minimax = Minimax(heuristic_complex, 5);

    // mg.setBoard(board);
    // srand(time(NULL));
    // board.printBoard();

    // while(!mg.hasLost() && !mg.stalemate()){
    //     //mg.printAttacked();
    //     //mg.printForced();

    //     //board.doMove(mcts.findOptimalMove(board));
    //     board.doMove(minimax.findOptimalMove(board));
    //     board.printBoard();

    //     mg.setBoard(board);
    //     if(mg.hasLost() || mg.stalemate()) break;

    //     const vector<ChessMove> &moves = mg.getMoves(board);
    //     board.doMove(moves[rand() % moves.size()]);
    //     board.printBoard();
    //     mg.setBoard(board);
    // }
    // board.printBoard();
    // if(mg.hasLost()) cout << "Lost!" << endl;
    // else cout << "Stalemate!" << endl;

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