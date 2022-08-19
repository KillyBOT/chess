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

    //board.printBoard();
    //board.printPieces();
    //print_bitboard(board.occupied());

    gMoveGenerator.setBoard(board);
    //gMoveGenerator.printAttacked();

    if(depth <= 0) return 1;
    int positionNum = 0;
    int toAdd;

    for(ChessMove move : gMoveGenerator.getMoves()){
        ChessBoard newBoard(board);
        newBoard.doMove(move);
        //std::cout << move.str() << std::endl;
        //newBoard.printBoard();
        toAdd = move_generation_test(newBoard, depth-1);
        //if(depth == 1) std::cout << move.strUCI() << '\t' << toAdd << std::endl;
        positionNum += toAdd;
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

    init_ray_table();
    init_knight_position_table();
    init_masks();
    init_zobrist_nums();

    MoveGenerator mg;

    // ChessBoard board(false);
    // board.addPiece(new_pos("a1"),new_piece(kPieceKing,  kPlayerWhite));
    // board.addPiece(new_pos("h8"),new_piece(kPieceKing, kPlayerBlack));
    // //board.addPiece(new_pos("e7"),new_piece(kPieceBishop, kPlayerBlack));
    // //board.addPiece(new_pos("b6"),new_piece(kPiecePawn, kPlayerWhite));
    // //board.addPiece(new_pos("f5"),new_piece(kPiecePawn, kPlayerWhite));
    // board.addPiece(new_pos("h4"),new_piece(kPieceRook, kPlayerBlack));
    // board.addPiece(new_pos("f4"),new_piece(kPiecePawn, kPlayerWhite));
    // //board.addPiece(new_pos("b4"),new_piece(kPiecePawn, kPlayerWhite));
    // //board.addPiece(new_pos("d2"),new_piece(kPiecePawn, kPlayerWhite));
    // //board.addPiece(new_pos("d7"),new_piece(kPiecePawn, kPlayerWhite));

    // board.printBoard();
    // mg.setBoard(board);
    // mg.printAttacked();

    ChessBoard board;
    board.fromFen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    // board.fromFen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/2Pn4/PP2N1PP/RNBQK2R w KQ - 0 1");
    board.printBoard();

    for(int i = 1; i < 5; i++){
        auto startTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        int positionNum = move_generation_test(board, i);
        auto endTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        cout << i << '\t' << positionNum << '\t' << endTime - startTime << endl;
    }

    // MCTS mcts = MCTS(2000);
    // Minimax minimax = Minimax(heuristic_basic, 4);

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
    // for(int i = 0; i < 8; i++){
    //     print_bitboard(kRankMasks[i]);
    //     print_bitboard(kFileMasks[i]);
    // }
    // for(ChessPos pos = 0; pos < 64; pos++){
    //     cout << pos_str(pos) << endl;
    //     print_bitboard(kDiagMasks[pos]);
    //     cout << endl;
    //     print_bitboard(kAntiDiagMasks[pos]);
    //     cout << endl << endl;
    // }
    // for(ChessPos pos = 0; pos < 64; pos++){
    //     cout << pos_str(pos) << endl;
    //     for(int dir = 0; dir < 8; dir++){
    //         print_bitboard(kRayMasks[pos][dir]);
    //         cout << endl;
    //     }
    //     cout << endl;
    // }
    return 0;
}