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

int perft_helper(ChessBoard &board, int depth){

    //board.printBoard();
    //std::cout << board.inCheck(board.opponent()) << std::endl;
    //board.printPieces();
    //print_bitboard(board.occupied());

    //gMoveGenerator.printAttacked();

    if(depth <= 0) return 1;
    int positionNum = 0;
    int toAdd;

    for(ChessMove move : gMoveGenerator.getMoves(board)){
        ChessBoard newBoard(board);
        newBoard.doMove(move);
        //std::cout << move.str() << std::endl;
        //newBoard.printBoard();
        positionNum += perft_helper(newBoard, depth-1);
    }

    return positionNum;
}
void perft(ChessBoard &board, int depth){

    using std::cout;
    using std::endl;

        
    int toAdd;
    int total = 0;
    auto startTime = std::chrono::steady_clock::now();

    for(ChessMove move : gMoveGenerator.getMoves(board)){
        ChessBoard newBoard(board);
        newBoard.doMove(move);
        toAdd = perft_helper(newBoard, depth-1);
        cout << move.strUCI() << ":\t" << toAdd << endl;
        total += toAdd;
    }

    auto endTime = std::chrono::steady_clock::now();

    std::chrono::duration<double> elapsed = endTime - startTime;

    cout << "Time elapsed (ms): " << static_cast<int>(elapsed.count() * 1000) << endl;
    cout << "Nodes searched: " << total << endl;
    cout << "Nodes per second: " << static_cast<int>(total / elapsed.count()) << endl;

}

int main()
{
    using std::cout;
    using std::endl;

    using std::vector;

    init_ray_table();
    init_knight_position_table();
    init_masks();
    init_zobrist_nums();
    init_magic_databases();

    MoveGenerator mg;

    srand(time(nullptr));

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
    // board.fromFen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    // board.fromFen("rnbqkbnr/1ppppppp/p7/8/Q7/2P5/PP1PPPPP/RNB1KBNR b KQkq - 0 1");
    board.printBoard();
    perft(board, 6);

    // for(int i = 1; i < 7; i++){
    //     auto startTime = std::chrono::steady_clock::now();
    //     int positionNum = perft(board, i, 100);
    //     auto endTime = std::chrono::steady_clock::now();

    //     std::chrono::duration<double> elapsed = endTime - startTime;
    //     cout << i << '\t' << positionNum << '\t' << static_cast<int>(elapsed.count() * 1000) << '\t' << static_cast<int>(positionNum / elapsed.count()) << endl;
    // }
    //cout << std::hex << find_magic(0, kMagicBishopIndBits[0], true) << endl;
    //create_magic_databases();

    // MCTS mcts = MCTS();
    // Minimax minimax = Minimax(heuristic_basic, 4, false, false);

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
    // if(mg.stalemate()) cout << "Stalemate!" << endl;
    // else{
    //     if(board.player() == kPlayerWhite) cout << "Black wins!" << endl;
    //     else cout << "White wins!" << endl;
    // }

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