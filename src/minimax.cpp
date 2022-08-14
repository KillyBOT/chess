#include <iostream>
#include <vector>
#include <chrono>
#include <exception>

#include "minimax.h"
#include "move_generator.h"
#include "board.h"

using std::pair;
using std::vector;

static MoveGenerator mg;

static const int kLossPenalty = 2147483647;
static const int kCheckPenalty = 1000;
static const int kMaterialCoefficient = 1;
static const int kAttackedCoefficient = 10;
static const int kPinnedCoefficient = 10;
static const int kPositionCoefficient = 1;

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

    mg.setBoard(board);
    Player opponent = (maxPlayer == kPlayerWhite) ? kPlayerBlack : kPlayerWhite;
    int score = 0;

    if(mg.hasLost()) score -= kLossPenalty;
    else if(mg.inCheck()) score -= kCheckPenalty;
    
    score += (board.playerScore(maxPlayer) - board.playerScore(opponent)) * kMaterialCoefficient;

    if(board.player() == opponent) score *= -1;

    return score;
}

//Does everything the basic heuristic search does, but also uses attacked squares, number of pinned pieces, and a position evaluation in the calculation
int heuristic_complex(ChessBoard &board, Player maxPlayer){
    mg.setBoard(board);
    Player opponent = (maxPlayer == kPlayerWhite) ? kPlayerBlack : kPlayerWhite;
    int score = 0;

    if(mg.hasLost()) score -= kLossPenalty;
    else if(mg.inCheck()) score -= kCheckPenalty;

    if(board.player() != maxPlayer) score *= -1;
    
    score += (board.playerScore(maxPlayer) - board.playerScore(opponent)) * kMaterialCoefficient;

    //score -= mg.attacked().size() * kAttackedCoefficient;
    //score -= mg.pinned().size() * kPinnedCoefficient;

    //mg.player_ = board.opponent();
    //mg.opponent_ = board.player();
    //mg.setKingPos();
    //mg.setAttacked();
    //mg.setPinned();

    //score += mg.attacked().size() * kAttackedCoefficient;
    //score += mg.pinned().size() * kPinnedCoefficient;

    //mg.setBoard(board);

    int positionScore = 0;
    int intPos;

    for(int i = 0; i < board.pieceNum(); i++){
        intPos = board.piecePositions()[i].pos;
        if(board.piece(board.piecePositions()[i]) == kPlayerWhite){
            switch(board.piece(board.piecePositions()[i]).type()){
                case kPiecePawn:
                positionScore += kPawnTableWhite[intPos];
                break;
                case kPieceKnight:
                positionScore += kKnightTable[intPos];
                break;
                case kPieceBishop:
                positionScore += kBishopTableWhite[intPos];
                break;
                case kPieceRook:
                positionScore += kRookTableWhite[intPos];
                break;
                case kPieceQueen:
                positionScore += kQueenTableWhite[intPos];
                break;
                case kPieceKing:
                positionScore += kKingTableWhite[intPos];
                break;
            }
        } else {
            switch(board.piece(board.piecePositions()[i]).type()){
                case kPiecePawn:
                positionScore -= kPawnTableBlack[intPos];
                break;
                case kPieceKnight:
                positionScore -= kKnightTable[intPos];
                break;
                case kPieceBishop:
                positionScore -= kBishopTableBlack[intPos];
                break;
                case kPieceRook:
                positionScore -= kRookTableBlack[intPos];
                break;
                case kPieceQueen:
                positionScore -= kQueenTableBlack[intPos];
                break;
                case kPieceKing:
                positionScore -= kKingTableBlack[intPos];
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

int Minimax::evalHelpMinimax(ChessBoard &board, int depth){

    if(mg.getMoves(board).empty() || depth <= 0) return this->evalBoard(board);

    int val;

    if(board.player() == this->maxPlayer_){

        val = -2147483647;

        for(ChessMove move : mg.getMoves(board)){
            board.doMove(move);
            val = std::max(evalHelpMinimax(board,depth-1),val);
            board.undoLastMove();
        } 

    } else {

        val = 2147483647;

        for(ChessMove move : mg.getMoves(board)){
            board.doMove(move);
            val = std::min(evalHelpMinimax(board,depth-1),val);
            board.undoLastMove();
        }

    }

    return val;
}
int Minimax::evalHelpAB(ChessBoard &board, int depth, int alpha, int beta){

    // std::cout << depth << std::endl;
    /*int val;

    if(!this->transpositionTable_.count(board.key()) || this->transpositionTable_.at(board.key()).depth < depth){
        int val;

        if(this->transpositionTable_.count(board.key())) this->transpositionTable_.erase(board.key());

        if(mg.getMoves(board).empty()) {
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

                for(ChessMove move : mg.getMoves(board)){
                    board.doMove(move);
                    val = std::max(evalHelpAB(board, depth-1, alpha, beta), val);
                    board.undoLastMove();
                    alpha = std::max(alpha, val);
                    if(beta <= alpha) break;
                }

            } else {

                val = 2147483647;

                for(ChessMove move : mg.getMoves(board)){
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

    if(mg.getMoves(board).empty() || depth <= 0){
        if(this->doQuiescence_) return this->evalHelpQuiescence(board, alpha, beta);
        else return this->evalBoard(board);
    }

    int val;
    if(board.player() == this->maxPlayer_){

        val = -2147483647;

        for(ChessMove move : mg.getMoves(board)){
            board.doMove(move);
            val = std::max(evalHelpAB(board, depth-1, alpha, beta), val);
            board.undoLastMove();
            alpha = std::max(alpha, val);
            if(beta <= alpha) break;
        }

    } else {

        val = 2147483647;

        for(ChessMove move : mg.getMoves(board)){
            board.doMove(move);
            val = std::min(evalHelpAB(board, depth-1, alpha, beta), val);
            board.undoLastMove();
            beta = std::min(beta, val);
            if(beta <= alpha) break;
        }

    }

    return val;
}
int Minimax::evalHelpQuiescence(ChessBoard &board, int alpha, int beta){
    int stand_pat = this->evalBoard(board);

    if(stand_pat >= beta) return beta;
    if (alpha < stand_pat) alpha = stand_pat;

    int score;

    for(ChessMove move : mg.getMoves(board)){
        if(move.captured.type() != kPieceNone){
            board.doMove(move);
            score = -this->evalHelpQuiescence(board, -beta, -alpha);
            board.undoLastMove();

            if(score >= beta) return beta;
            if(score > alpha) return alpha;
        }
    }

    return alpha;
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

    for(ChessMove move: mg.getMoves(board)){
        //std::cout << child.player() << std::endl;
        //child.printBoard();

        //if(this->doABPruining_) score = evalHelpAB(child, this->depth_, -2147483647, 2147483647, board.player());
        //else score = evalHelpMinimax(child, this->depth_, board.player());
        board.doMove(move);
        if(this->doABPruining_) score = evalHelpAB(board, this->depth_ - 1, -2147483647, 2147483647);
        else score = this->evalHelpMinimax(board, this->depth_-1);
        board.undoLastMove();

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