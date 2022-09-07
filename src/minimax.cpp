#include <iostream>
#include <vector>
#include <chrono>
#include <exception>

#include "minimax.h"
#include "move_generator.h"
#include "board.h"

using std::pair;
using std::vector;

static const int kLossPenalty = 100000000;
static const int kCheckPenalty = 100000;
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
    20, 30, 10, 0, 0, 10, 30, 20,
    20, 20, 0, 0, 0, 0, 20, 20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30
};
static const int kKingTableBlack[64] = {
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    20, 20, 0, 0, 0, 0, 20, 20,
    20, 30, 10, 0, 0, 10, 30, 20
};
static TranspositionTableEntry invalidEntry = TranspositionTableEntry();

//Only checks how much material you have, and of course if you've won or not
int heuristic_basic(ChessBoard &board) {

    int score = 0;
    if(gMoveGenerator.hasWon(board)) score += kLossPenalty;
    else if(gMoveGenerator.hasLost(board)) score -= kLossPenalty;
    // else if(board.inCheck(board.player())) score -= kCheckPenalty;
    // else if(board.inCheck(board.opponent())) score += kCheckPenalty;
    
    score += (board.playerScore(board.player()) - board.playerScore(board.opponent())) * kMaterialCoefficient;

    // if(score){
    //     std::cout << score << std::endl;
    //     board.printBoard();
    // }


    return score;
}

//Does everything the basic heuristic search does, but also uses attacked squares, number of pinned pieces, and a position evaluation in the calculation
int heuristic_complex(ChessBoard &board){
    int score = 0;

    if(gMoveGenerator.hasWon(board)) score += kLossPenalty;
    else if(gMoveGenerator.hasLost(board)) score -= kLossPenalty;
    // else if(board.inCheck(board.player())) score -= kCheckPenalty;
    // else if(board.inCheck(board.opponent())) score += kCheckPenalty;
    
    score += (board.playerScore(board.player()) - board.playerScore(board.opponent())) * kMaterialCoefficient;

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

    score += (board.player() == kPlayerWhite ? positionScore : -positionScore) * kPositionCoefficient;

    return score;
}

TranspositionTableEntry::TranspositionTableEntry(ChessMove bestMove, int val, int depth, int entryType){
    this->bestMove = bestMove;
    this->val = val;
    this->depth = depth;
    this->entryType = entryType;

    this->isValid = false;
}

TranspositionTable::TranspositionTable(){
}

    TranspositionTableEntry TranspositionTable::getTTEntry(ChessBoard &board, int depth, int alpha, int beta) const {
        if(this->tt_.count(board.key())){
            TranspositionTableEntry entry = this->tt_.at(board.key());
            if(entry.depth >= depth){
                if(entry.entryType == kExactEntry) return entry;
                else if(entry.entryType == kUpperEntry && entry.val <= alpha) return entry;
                else if(entry.entryType == kLowerEntry && entry.val >= beta ) return entry;
            }
        }
        return invalidEntry;
    }
    int TranspositionTable::size() const {
        return tt_.size();
    }

    void TranspositionTable::addTTEntry(ChessBoard &board, ChessMove bestMove, int val, int depth, int entryType){
        TranspositionTableEntry entry = TranspositionTableEntry(bestMove, val, depth, entryType);
        entry.isValid = true;
        this->tt_[board.key()] = entry;
    }

int Minimax::evalBoard(ChessBoard &board){
    //return this->heuristicFunc_(board, this->maxPlayer_);
    if(!this->boardScores_.count(board.key())) {
        int boardEval = this->heuristicFunc_(board);
        this->boardScores_.emplace(board.key(), this->heuristicFunc_(board));
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
int Minimax::evalHelpAB(ChessBoard &board, int depth, int alpha, int beta){

    // gMoveGenerator.setBoard(board);

    // if(this->transpositionTable_.count(board.key()) && this->transpositionTable_.at(board.key()).depth >= depth) return this->transpositionTable_.at(board.key()).val;

    // if(gMoveGenerator.getMoves().empty()) return this->evalBoard(board);
    // else if(depth == 0){
    //     if(this->doQuiescence_) return this->evalHelpQuiescence(board, kQuiescenceDepth, alpha, beta);
    //     return this->evalBoard(board);
    // }

    // int val;
    // if(board.player() == this->maxPlayer_){

    //     val = -2147483647;

    //     for(ChessMove move : gMoveGenerator.getMoves(true, true)){
    //         ChessBoard newBoard(board);
    //         newBoard.doMove(move);
    //         val = std::max(evalHelpAB(newBoard, depth-1, alpha, beta), val);
    //         alpha = std::max(alpha, val);
    //         if(val >= beta) break;
    //     }

    // } else {

    //     val = 2147483647;

    //     for(ChessMove move : gMoveGenerator.getMoves(true, true)){
    //         ChessBoard newBoard(board);
    //         newBoard.doMove(move);
    //         val = std::min(evalHelpAB(newBoard, depth-1, alpha, beta), val);
    //         beta = std::min(val, beta);
    //         if(val <= alpha) break;
    //     }

    // }

    // this->transpositionTable_.emplace(board.key(), TranspositionTableEntry(val, depth));

    // return val;

    if(this->tt_.getTTEntry(board, depth, alpha, beta).isValid) return this->tt_.getTTEntry(board, depth, alpha, beta).val;

    if(depth == 0){
        if(this->doQuiescence_) return this->evalHelpQuiescence(board, alpha, beta);
        else return this->evalBoard(board);
    }

    vector<ChessMove> moves = gMoveGenerator.getMoves(board, true, true);
    if(moves.empty()) return this->evalBoard(board);

    ChessMove bestMove;

    int evalType = kUpperEntry;
    for(ChessMove move : moves){
        ChessBoard newBoard(board);
        newBoard.doMove(move);

        int eval = -evalHelpAB(newBoard, depth - 1, -beta, -alpha);

        if(eval >= beta) {
            this->tt_.addTTEntry(board, move, beta, depth, kLowerEntry);
            return beta;
        }

        if(eval > alpha) {
            evalType = kExactEntry;
            bestMove = move;
            alpha = eval;
        }
    }
    this->tt_.addTTEntry(board, bestMove, alpha, depth, evalType);
    
    return alpha;

}
int Minimax::evalHelpQuiescence(ChessBoard &board, int alpha, int beta){
    
    int eval = this->evalBoard(board);
    if(eval >= beta) return beta;
    if(eval > alpha) alpha = eval;

    vector<ChessMove> moves = gMoveGenerator.getMoves(board, false, true);

    for(ChessMove move : moves){
        ChessBoard newBoard(board);
        newBoard.doMove(move);
        eval = -evalHelpQuiescence(newBoard, -beta, -alpha);

        if(eval >= beta) return beta;
        if(eval > alpha) alpha = eval;
    }

    return alpha;
}

Minimax::Minimax(int(*heuristicFunc)(ChessBoard&), int searchTime, bool doABPruning, bool doQuiescence) : ChessAI("Minimax"){
    this->heuristicFunc_ = heuristicFunc;
    this->searchTime_ = searchTime;
    this->doABPruning_ = doABPruning;
    this->doQuiescence_ = doQuiescence;
}

ChessMove Minimax::findOptimalMove(ChessBoard &board){

    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::seconds;
    using std::chrono::system_clock;

    int score;
    int bestScore = 0;
    int bestEval;
    ChessMove bestMove;
    this->maxPlayer_ = board.player();

    //board.printBoard();
    //std::cout << board.player() << std::endl;
    
    bool doIDDFS = true;
    int maxDepth = 1;
    vector<ChessMove> moves = gMoveGenerator.getMoves(board);

    auto startTime = std::chrono::steady_clock::now();
    
    while(doIDDFS){

        auto checkStartTime = std::chrono::steady_clock::now();

        bestEval = evalHelpAB(board, maxDepth, -2147483647, 2147483647);
        bestMove = this->tt_.getTTEntry(board, 0, 2147483647, -2147483647).bestMove;

        ChessBoard newBoard(board);
        newBoard.doMove(bestMove);
        if(gMoveGenerator.hasLost(newBoard)) doIDDFS = false;
        // for(ChessMove move : moves){
        //     ChessBoard newBoard(board);
        //     newBoard.doMove(move);

        //     if(this->doABPruning_) score = evalHelpAB(newBoard, maxDepth, -2147483647, 2147483647);
        //     else score = evalHelpMinimax(newBoard, maxDepth, move.isQuiet());

        //     if(score > bestScore){
        //         bestScore = score;
        //         bestMove = move;
        //     }
        // }

        auto checkEndTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> checkElapsed = checkEndTime - checkStartTime;

        std::cout << maxDepth << '\t' << bestMove.strUCI() << '\t' << bestEval << std::endl;

        maxDepth++;

        if( static_cast<int>(checkElapsed.count() * 1000) > (this->searchTime_ >> 1)) doIDDFS = false;
    }

    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    std::cout << "Found move in [" << static_cast<int>(elapsed.count() * 1000) << "] ms" << std::endl;

    return bestMove;
}

bool Minimax::setABPruning(){
    this->doABPruning_ = !this->doABPruning_;
    return this->doABPruning_;
}