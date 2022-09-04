#include <iostream>
#include <vector>
#include <chrono>
#include <exception>

#include "minimax.h"
#include "move_generator.h"
#include "board.h"

using std::pair;
using std::vector;

static const int kLossPenalty = 100000;
static const int kCheckPenalty = 1000;
static const int kMaterialCoefficient = 1;
static const int kAttackedCoefficient = 10;
static const int kPinnedCoefficient = 10;
static const int kPositionCoefficient = 1;

static const int kQuiescenceDepth = 10;

static const int kPawnTableWhite[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    5, 10, 10, -20, -20, 10, 10, 5,
    5, -5, -10, 0, 0, -10, -5, 5,
    0, 0, 0, 20, 20, 0, 0, 0,
    5, 5, 10, 25, 25, 10, 5, 5,
    10, 10, 20, 30, 30, 20, 10, 10,
    50, 50, 50, 50, 50, 50, 50, 50,
    0, 0, 0, 0, 0, 0, 0, 0
};
static const int kPawnTableBlack[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5, 5, 10, 25, 25, 10, 5, 5,
    0, 0, 0, 20, 20, 0, 0, 0,
    5, -5, -10, 0, 0, -10, -5, 5,
    5, 10, 10, -20, -20, 10, 10, 5,
    0, 0, 0, 0, 0, 0, 0, 0
};
static const int kKnightTable[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20, 0, 5, 5, 0, -20, -40,
    -30, 5, 10, 15, 15, 10, 5, -30,
    -30, 0, 15, 20, 20, 15, 0, -30,
    -30, 5, 15, 20, 20, 15, 5, -30,
    -30, 0, 10, 15, 15, 10, 0, -30,
    -40, -20, 0, 0, 0, 0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50
};
static const int kBishopTableWhite[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10, 5, 0, 0, 0, 0, 5, -10,
    -10, 10, 10, 10, 10, 10, 10, -10,
    -10, 0, 10, 10, 10, 10, 0, -10,
    -10, 5, 5, 10, 10, 5, 5, -10,
    -10, 0, 5, 10, 10, 5, 0, -10,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -20, -10, -10, -10, -10, -10, -10, -20
};
static const int kBishopTableBlack[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 10, 10, 5, 0, -10,
    -10, 5, 5, 10, 10, 5, 5, -10,
    -10, 0, 10, 10, 10, 10, 0, -10,
    -10, 10, 10, 10, 10, 10, 10, -10,
    -10, 5, 0, 0, 0, 0, 5, -10,
    -20, -10, -10, -10, -10, -10, -10, -20
};
static const int kRookTableWhite[64] = {
    0, 0, 0, 5, 5, 0, 0, 0,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    5, 10, 10, 10, 10, 10, 10, 5,
    0, 0, 0, 0, 0, 0, 0, 0
};
static const int kRookTableBlack[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    5, 10, 10, 10, 10, 10, 10, 5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    0, 0, 0, 5, 5, 0, 0, 0
};
static const int kQueenTableWhite[64] = {
    -20, -10, -10, -5, -5, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 5, 5, 5, 5, 5, 0, -10,
    0, 0, 5, 5, 5, 5, 0, -5,
    -5, 0, 5, 5, 5, 5, 0, -5,
    -10, 0, 5, 5, 5, 5, 0, -10,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -20, -10, -10, -5, -5, -10, -10, -20
};
static const int kQueenTableBlack[64] = {
    -20, -10, -10, -5, -5, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 5, 5, 5, 0, -10,
    -5, 0, 5, 5, 5, 5, 0, -5,
    0, 0, 5, 5, 5, 5, 0, -5,
    -10, 5, 5, 5, 5, 5, 0, -10,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -20, -10, -10, -5, -5, -10, -10, -20
};
static const int kKingTableWhite[64] = {
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    20, 20, 0, 0, 0, 0, 20, 20,
    20, 30, 10, 0, 0, 10, 30, 20
};
static const int kKingTableBlack[64] = {
    20, 30, 10, 0, 0, 10, 30, 20,
    20, 20, 0, 0, 0, 0, 20, 20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30
};

//Only checks how much material you have, and of course if you've won or not
int heuristic_basic(ChessBoard &board, Player maxPlayer) {

    gMoveGenerator.setBoard(board);
    Player opponent = player_opponent(maxPlayer);
    int score = 0;

    if(gMoveGenerator.hasLost()) score -= kLossPenalty;
    else if(board.inCheck(maxPlayer)) score -= kCheckPenalty;
    
    if(board.player() == opponent) score *= -1;

    score += (board.playerScore(maxPlayer) - board.playerScore(opponent)) * kMaterialCoefficient;

    // if(score){
    //     std::cout << score << std::endl;
    //     board.printBoard();
    // }


    return score;
}

//Does everything the basic heuristic search does, but also uses attacked squares, number of pinned pieces, and a position evaluation in the calculation
int heuristic_complex(ChessBoard &board, Player maxPlayer){
    gMoveGenerator.setBoard(board);
    Player opponent = (maxPlayer == kPlayerWhite) ? kPlayerBlack : kPlayerWhite;
    int score = 0;

    if(gMoveGenerator.hasLost()) score -= kLossPenalty;
    else if(board.inCheck(maxPlayer)) score -= kCheckPenalty;
    
    if(board.player() == opponent) score *= -1;

    score += (board.playerScore(maxPlayer) - board.playerScore(opponent)) * kMaterialCoefficient;

    //score -= gMoveGenerator.attacked().size() * kAttackedCoefficient;
    //score -= gMoveGenerator.pinned().size() * kPinnedCoefficient;

    //gMoveGenerator.player_ = board.opponent();
    //gMoveGenerator.opponent_ = board.player();
    //gMoveGenerator.setKingPos();
    //gMoveGenerator.setAttacked();
    //gMoveGenerator.setPinned();

    //score += gMoveGenerator.attacked().size() * kAttackedCoefficient;
    //score += gMoveGenerator.pinned().size() * kPinnedCoefficient;

    //gMoveGenerator.setBoard(board);

    int positionScore = 0;
    int pos;
    U64 occupied = board.totalOccupied;
    ChessPiece piece;

    while(occupied){
        pos = bitscan_forward_iter(occupied);
        piece = board.piece(pos);
        if(piece_player(piece) == kPlayerWhite){
            switch(piece_type(piece)){
                case kPiecePawn:
                positionScore += kPawnTableWhite[pos];
                break;
                case kPieceKnight:
                positionScore += kKnightTable[pos];
                break;
                case kPieceBishop:
                positionScore += kBishopTableWhite[pos];
                break;
                case kPieceRook:
                positionScore += kRookTableWhite[pos];
                break;
                case kPieceQueen:
                positionScore += kQueenTableWhite[pos];
                break;
                case kPieceKing:
                positionScore += kKingTableWhite[pos];
                break;
                default:
                break;
            }
        } else {
            switch(piece_type(piece)){
                case kPiecePawn:
                positionScore -= kPawnTableBlack[pos];
                break;
                case kPieceKnight:
                positionScore -= kKnightTable[pos];
                break;
                case kPieceBishop:
                positionScore -= kBishopTableBlack[pos];
                break;
                case kPieceRook:
                positionScore -= kRookTableBlack[pos];
                break;
                case kPieceQueen:
                positionScore -= kQueenTableBlack[pos];
                break;
                case kPieceKing:
                positionScore -= kKingTableBlack[pos];
                break;
                default:
                break;
            }
        }
    }

    score += (maxPlayer == kPlayerWhite ? positionScore : -positionScore) * kPositionCoefficient;

    return score;
}

TranspositionTableEntry::TranspositionTableEntry(int val, int depth){
    this->val = val;
    this->depth = depth;
}

int Minimax::evalBoard(ChessBoard &board){
    return this->heuristicFunc_(board, this->maxPlayer_);
    if(!this->boardScores_.count(board.key())) {
        int boardEval = this->heuristicFunc_(board, this->maxPlayer_);
        this->boardScores_.emplace(board.key(), this->heuristicFunc_(board, this->maxPlayer_));
    }

    // if(this->boardScores_.at(board.zobristKey()) != 0) {
    //     std::cout << this->boardScores_.at(board.zobristKey()) << std::endl;
    //     board.printBoard();
    // }

    return this->boardScores_.at(board.key());
}

int Minimax::evalHelpMinimax(ChessBoard &board, int depth, bool isQuiet){

    if(gMoveGenerator.getMoves(board).empty() || depth <= 0) return this->evalBoard(board);

    int val;

    if(board.player() == this->maxPlayer_){

        val = -2147483647;

        for(ChessMove move : gMoveGenerator.getMoves(board)){
            ChessBoard newBoard(board);
            newBoard.doMove(move);
            val = std::max(evalHelpMinimax(newBoard, depth-1, move.isQuiet()),val);
        } 

    } else {

        val = 2147483647;

        for(ChessMove move : gMoveGenerator.getMoves(board)){
            ChessBoard newBoard(board);
            newBoard.doMove(move);
            val = std::min(evalHelpMinimax(newBoard, depth-1, move.isQuiet()),val);
        }

    }

    return val;
}
int Minimax::evalHelpAB(ChessBoard &board, int depth, int alpha, int beta, bool isQuiet, bool isQuiescence){

    // std::cout << depth << std::endl;
    /*int val;

    if(!this->transpositionTable_.count(board.key()) || this->transpositionTable_.at(board.key()).depth < depth){
        int val;

        if(this->transpositionTable_.count(board.key())) this->transpositionTable_.erase(board.key());

        if(gMoveGenerator.getMoves(board).empty()) {
            depth = 2147483647;
            val = this->evalBoard(board);
        }
        else if(depth <= 0){
            if(this->doQuiescence_) val = this->evalHelpQuiescence(board, alpha, beta);
            else val = this->evalBoard(board);
        }
        else {

            if(board.player() == this->maxPlayer_){

                val = -2147483647;

                for(ChessMove move : gMoveGenerator.getMoves(board)){
                    board.doMove(move);
                    val = std::max(evalHelpAB(board, depth-1, alpha, beta), val);
                    board.undoLastMove();
                    alpha = std::max(alpha, val);
                    if(beta <= alpha) break;
                }

            } else {

                val = 2147483647;

                for(ChessMove move : gMoveGenerator.getMoves(board)){
                    board.doMove(move);
                    val = std::min(evalHelpAB(board, depth-1, alpha, beta), val);
                    board.undoLastMove();
                    beta = std::min(beta, val);
                    if(beta <= alpha) break;
                }

            }
        }

        this->transpositionTable_.emplace(board.key(),TranspositionTableEntry(val, depth));
    
    }

    return this->transpositionTable_.at(board.key()).val;*/

    //board.printBoard();
    if(isQuiescence && isQuiet) return this->evalBoard(board);
    else if(gMoveGenerator.getMoves(board).empty()) return this->evalBoard(board);
    else if(depth == 0){
        if(this->doQuiescence_) return this->evalHelpAB(board, kQuiescenceDepth, -2147483647, 2147483647, isQuiet, true);
        else return this->evalBoard(board);
    }

    int val;
    if(board.player() == this->maxPlayer_){

        val = -2147483647;

        for(ChessMove move : gMoveGenerator.getMoves(board, !doQuiescence_)){
            ChessBoard newBoard(board);
            newBoard.doMove(move);
            val = std::max(evalHelpAB(newBoard, depth-1, alpha, beta, move.isQuiet(), isQuiescence), val);
            alpha = std::max(alpha, val);
            if(val >= beta) break;
        }

    } else {

        val = 2147483647;

        for(ChessMove move : gMoveGenerator.getMoves(board, !doQuiescence_)){
            ChessBoard newBoard(board);
            newBoard.doMove(move);
            val = std::min(evalHelpAB(newBoard, depth-1, alpha, beta, move.isQuiet(), isQuiescence), val);
            beta = std::min(val, beta);
            if(val <= alpha) break;
        }

    }

    return val;
}


Minimax::Minimax(int(*heuristicFunc)(ChessBoard&, Player), int depth, bool doABPruning, bool doQuiescence) : ChessAI("Minimax"){
    this->heuristicFunc_ = heuristicFunc;
    this->depth_ = depth;
    this->doABPruining_ = doABPruning;
    this->doQuiescence_ = doQuiescence;
}

ChessMove Minimax::findOptimalMove(ChessBoard &board){

    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::seconds;
    using std::chrono::system_clock;

    int score;
    int bestScore = -2147483647;
    ChessMove bestMove;
    this->maxPlayer_ = board.player();

    //board.printBoard();
    //std::cout << board.player() << std::endl;

    auto startTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    for(ChessMove move: gMoveGenerator.getMoves(board)){
        //std::cout << child.player() << std::endl;
        //child.printBoard();

        //if(this->doABPruining_) score = evalHelpAB(child, this->depth_, -2147483647, 2147483647, board.player());
        //else score = evalHelpMinimax(child, this->depth_, board.player());
        ChessBoard newBoard(board);
        newBoard.doMove(move);
        if(this->doABPruining_) score = evalHelpAB(newBoard, this->depth_ - 1, -2147483647, 2147483647, move.isQuiet(), false);
        else score = this->evalHelpMinimax(newBoard, this->depth_ - 1, move.isQuiet());

        // child.printBoard();
        // std::cout << score << std::endl;

        if(score > bestScore){
            //std::cout << move.str() << std::endl;
            //board.printBoard();
            bestScore = score;
            bestMove = move;
        }
    }

    auto endTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    std::cout << "Found move in [" << endTime - startTime << "] ms" << std::endl;

    return bestMove;
}

bool Minimax::setABPruning(){
    this->doABPruining_ = !this->doABPruining_;
    return this->doABPruining_;
}