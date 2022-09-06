#include <iostream>
#include <vector>
#include <bits/stdc++.h>

#include "ray.h"
#include "bitboard.h"
#include "move_generator.h"

MoveGenerator gMoveGenerator;

static const int kMaxMoveNum = 218; //The theoretical maximum number of moves that could be done in a position

bool MoveGenerator::willMoveCapture(ChessMove &move) const {
    if(this->opponentOccupied_ & kPosMasks[move.newPos]){
        move.captured = this->board_->piece(move.newPos);
        return true;
    }
    return false;
}
bool MoveGenerator::enPassantCheck(ChessMove &move) const {
    if(move.newPos == this->enPassantSquare_){
        move.moveData = kMoveFlagEnPassant;
        if(this->player_ == kPlayerWhite) move.captured = this->board_->piece(move.newPos - 8);
        else move.captured = this->board_->piece(move.newPos + 8);

        return true;
    }
    return false;
}

void MoveGenerator::genPseudoLegalMoves(vector<ChessMove> &moves) const {
    this->genPawnMoves(moves);
    this->genSlidingMoves(moves);
    this->genKnightMoves(moves);
    this->genKingMoves(moves);


}
void MoveGenerator::genLegalMoves(vector<ChessMove> &legalMoves, vector<ChessMove> &moves, bool incQuiet) {
    //ChessBoard board(*this->board_);
    //for(ChessMove move : moves) std::cout << move.str() << std::endl;
    for(ChessMove &move : moves){
        if(!incQuiet && move.isQuiet()) continue;
        ChessBoard newBoard(*this->board_);
        newBoard.doMove(move, false);
        //std::cout << move.str() << std::endl;
        //board.printBoard();
        //this->occupied_ = board.totalOccupied;
        //this->kingPos_ = board.kingPos(this->player_);
        //this->setAttacked(&board);
        if(!newBoard.inCheck(this->player_)) legalMoves.push_back(move);
        //board.undoMove(move, false);
    }
}
void MoveGenerator::genKingMoves(vector<ChessMove> &moves) const {

    ChessMove move = ChessMove(this->board_->piece(this->kingPos_),this->kingPos_,this->kingPos_);
    //First check eight directions
    // for(int dir = 0; dir < 8; dir++){
    //     if(kRaySizes[this->kingPos_][dir]){
    //         move.newPos = kRays[this->kingPos_][dir][0];
    //         //std::cout << move.newPos.str() << '\t' << this->attacked_[move.newPos.pos] << std::endl;
    //         if(!this->hasPieceAtPos(move.newPos) || this->willMoveCapture(move)) {
    //             moves.push_back(move);
    //             move.captured = 0;
    //         }
    //     }
    // }
    U64 attacks = kKingAttackMasks[this->kingPos_] & this->validPositions_;
    while(attacks){
        move.newPos = bitscan_forward_iter(attacks);
        move.captured = this->board_->piece(move.newPos);
        moves.push_back(move);
    }

    //Then check if you can castle
    if(!this->board_->inCheck(this->player_) && ((this->player_ == kPlayerWhite && this->kingPos_ == new_pos("e1")) || (this->player_ == kPlayerBlack && this->kingPos_ == new_pos("e8"))) ){
        move.moveData = kMoveFlagIsCastling;
        //Queenside
        if(
            this->board_->canCastle(this->player_, false) &&
            this->board_->piece(new_pos('a',pos_rank_char(this->kingPos_))) == new_piece(kPieceRook, this->player_) &&
            !this->board_->piece(new_pos('b',pos_rank_char(this->kingPos_))) &&
            !this->board_->piece(new_pos('c',pos_rank_char(this->kingPos_))) &&
            !this->board_->piece(new_pos('d',pos_rank_char(this->kingPos_))) &&
            !this->board_->posAttacked(new_pos('b',pos_rank_char(this->kingPos_)), this->opponent_) &&
            !this->board_->posAttacked(new_pos('c',pos_rank_char(this->kingPos_)), this->opponent_) &&
            !this->board_->posAttacked(new_pos('d',pos_rank_char(this->kingPos_)), this->opponent_)
        ) {
            move.newPos = new_pos('b',pos_rank_char(this->kingPos_));
            moves.push_back(move);
        }

        //Kingside
        if(
            this->board_->canCastle(this->player_, true) &&
            this->board_->piece(new_pos('h',pos_rank_char(this->kingPos_))) == new_piece(kPieceRook, this->player_) &&
            !this->board_->piece(new_pos('f',pos_rank_char(this->kingPos_))) &&
            !this->board_->piece(new_pos('g',pos_rank_char(this->kingPos_))) &&
            !this->board_->posAttacked(new_pos('f',pos_rank_char(this->kingPos_)), this->opponent_) &&
            !this->board_->posAttacked(new_pos('g',pos_rank_char(this->kingPos_)), this->opponent_)
        ) {
            move.newPos = new_pos('g',pos_rank_char(this->kingPos_));
            moves.push_back(move);
        }
    }
}
void MoveGenerator::genKnightMoves(vector<ChessMove> &moves) const {

    U64 knights = this->board_->occupied[kPieceListInd[this->player_][kPieceKnight]];
    ChessPos start;
    U64 attacks;

    while(knights){
        start = bitscan_forward_iter(knights);
        ChessMove move = ChessMove(this->board_->piece(start), start, start);

        attacks = kKnightAttackMasks[start] & this->validPositions_;
        while(attacks){
            move.newPos = bitscan_forward_iter(attacks);
            move.captured = this->board_->piece(move.newPos);
            moves.push_back(move);
        }

        // for(int j = 0; j < kKnightPositionTableSize[start]; j++){
        //     move.newPos = kKnightPositionTable[start][j];
        //     if(!this->hasPieceAtPos(move.newPos) || this->willMoveCapture(move)) {
        //         moves.push_back(move);
        //         move.captured = 0;
        //     }
        // }
    }
}
void MoveGenerator::genPawnMoves(vector<ChessMove> &moves) const {

    char dirL, dirR, dirF, startRank, promoteRank;
    bool isPinned, doTwoMoveCheck;
    ChessMove move;
    ChessPos start;

    if(this->player_ == kPlayerWhite){
        dirL = kRayDirNE;
        dirR = kRayDirNW;
        dirF = kRayDirN;
        promoteRank = 7;
        startRank = 1;
    } else {
        dirL = kRayDirSW;
        dirR = kRayDirSE;
        dirF = kRayDirS;
        promoteRank = 0;
        startRank = 6;
    }

    U64 pawns = this->board_->occupied[kPieceListInd[this->player_][kPiecePawn]];

    while(pawns){
        start = bitscan_forward_iter(pawns);
        move = ChessMove(this->board_->piece(start), start, start);

        if(kRaySizes[start][dirL]){
            move.newPos = kRays[start][dirL][0];
            if(this->willMoveCapture(move) || this->enPassantCheck(move)){
                if(pos_rank(move.newPos) == promoteRank){
                    move.moveData = kMoveFlagPromotingToBishop;
                    moves.push_back(move);
                    move.moveData = kMoveFlagPromotingToRook;
                    moves.push_back(move);
                    move.moveData = kMoveFlagPromotingToKnight;
                    moves.push_back(move);
                    move.moveData = kMoveFlagPromotingToQueen;
                }

                moves.push_back(move);
                move.moveData = kMoveFlagNone;
            }
        }
        if(kRaySizes[start][dirR]){
            move.newPos = kRays[start][dirR][0];
            if(this->willMoveCapture(move) || this->enPassantCheck(move)){
                if(pos_rank(move.newPos) == promoteRank){
                    move.moveData = kMoveFlagPromotingToBishop;
                    moves.push_back(move);
                    move.moveData = kMoveFlagPromotingToRook;
                    moves.push_back(move);
                    move.moveData = kMoveFlagPromotingToKnight;
                    moves.push_back(move);
                    move.moveData = kMoveFlagPromotingToQueen;
                }

                moves.push_back(move);
                move.moveData = kMoveFlagNone;
            }
        }

        move.captured = 0;

        doTwoMoveCheck = false;
        move.newPos = kRays[start][dirF][0];
        if(!this->board_->hasPieceAtPos(move.newPos)){
            doTwoMoveCheck = true;
            if(pos_rank(move.newPos) == promoteRank){
                move.moveData = kMoveFlagPromotingToBishop;
                moves.push_back(move);
                move.moveData = kMoveFlagPromotingToRook;
                moves.push_back(move);
                move.moveData = kMoveFlagPromotingToKnight;
                moves.push_back(move);
                move.moveData = kMoveFlagPromotingToQueen;
            }
            moves.push_back(move);
            move.moveData = kMoveFlagNone;
        }

        if(doTwoMoveCheck && pos_rank(start) == startRank){
            move.newPos = kRays[start][dirF][1];
            if(!this->board_->hasPieceAtPos(move.newPos)){
                if(pos_rank(move.newPos) == promoteRank){
                    move.moveData = kMoveFlagPromotingToBishop;
                    moves.push_back(move);
                    move.moveData = kMoveFlagPromotingToRook;
                    moves.push_back(move);
                    move.moveData = kMoveFlagPromotingToKnight;
                    moves.push_back(move);
                    move.moveData = kMoveFlagPromotingToQueen;
                } else {
                    move.moveData = kMoveFlagEnPassantEligible;
                }

                moves.push_back(move);
            }
        }
    }

}
void MoveGenerator::genSlidingMoves(vector<ChessMove> &moves) const {
    U64 rooks = this->board_->occupied[kPieceListInd[this->player_][kPieceRook]];
    U64 bishops = this->board_->occupied[kPieceListInd[this->player_][kPieceBishop]];
    U64 queens = this->board_->occupied[kPieceListInd[this->player_][kPieceQueen]];

    bool isPinned;
    int posInDir;
    ChessMove move;
    ChessPos start;
    U64 attacks;
    // this->board_->printBoard();
    // print_bitboard(notOccupied);

    while(rooks){

        start = bitscan_forward_iter(rooks);
        move = ChessMove(this->board_->piece(start),start,start);

        attacks = this->board_->rookAttacks(start, this->occupied_) & this->validPositions_;
        while(attacks){
            move.newPos = bitscan_forward_iter(attacks);
            move.captured = this->board_->piece(move.newPos);
            moves.push_back(move);
        }
    }

    while(bishops){

        start = bitscan_forward_iter(bishops);
        move = ChessMove(this->board_->piece(start),start,start);

        attacks = this->board_->bishopAttacks(start, this->occupied_) & this->validPositions_;
        while(attacks){
            move.newPos = bitscan_forward_iter(attacks);
            move.captured = this->board_->piece(move.newPos);
            moves.push_back(move);
        }
    }

    while(queens){

        start = bitscan_forward_iter(queens);
        move = ChessMove(this->board_->piece(start),start,start);

        attacks = (this->board_->rookAttacks(start, this->occupied_) | this->board_->bishopAttacks(start, this->occupied_)) & this->validPositions_;
        while(attacks){
            move.newPos = bitscan_forward_iter(attacks);
            move.captured = this->board_->piece(move.newPos);
            moves.push_back(move);
        }
    }
}

MoveGenerator::MoveGenerator() {
    return;
}

bool MoveGenerator::stalemate() {
    if(this->board_->fiftyMoveStalemate()) return true;
    return this->getMoves().empty() && !this->board_->inCheck(this->player_);
}
bool MoveGenerator::stalemate(ChessBoard &board){
    this->setBoard(board);
    return this->stalemate();
}

bool MoveGenerator::hasLost() {
    return this->board_->inCheck(this->player_) & this->getMoves().empty();
}
bool MoveGenerator::hasLost(ChessBoard &board) {
    this->setBoard(board);
    return this->hasLost();
}

vector<ChessMove> MoveGenerator::getMoves(bool incQuiet, bool orderMoves) {
    vector<ChessMove> pseudoLegalMoves, legalMoves;

    pseudoLegalMoves.reserve(kMaxMoveNum);
    this->genPseudoLegalMoves(pseudoLegalMoves);

    legalMoves.reserve(pseudoLegalMoves.size());
    this->genLegalMoves(legalMoves, pseudoLegalMoves, incQuiet);

    if(orderMoves) std::sort(legalMoves.begin(), legalMoves.end(), compareChessMoves);

    return legalMoves;
}
vector<ChessMove> MoveGenerator::getMoves(ChessBoard &board, bool incQuiet, bool orderMoves) {
    this->setBoard(board);
    return this->getMoves(incQuiet, orderMoves);
}

bool MoveGenerator::isNodeQuiet(){
    vector<ChessMove> moves = this->getMoves();
    for(ChessMove &move : moves){
        if(!move.isQuiet()) return false;
    }
    return true;
}
bool MoveGenerator::isNodeQuiet(ChessBoard &board){
    this->setBoard(board);
    return this->isNodeQuiet();
}

void MoveGenerator::setBoard(ChessBoard &board) {
    this->board_ = &board;
    this->player_ = board.player();
    this->opponent_ = board.opponent();
    this->kingPos_ = board.kingPos(this->player_);
    this->enPassantSquare_ = board.enPassantSquare();

    this->validPositions_ = 0;
    this->opponentOccupied_ = 0;
    for(PieceType type = kPiecePawn; type <= kPieceKing; type++) {
        this->validPositions_ |= board.occupied[kPieceListInd[this->player_][type]];
        this->opponentOccupied_ |= board.occupied[kPieceListInd[this->opponent_][type]];
    }
    //this->validPositions_ |= board.kingPos(this->opponent_);
    this->validPositions_ = ~this->validPositions_;
    this->occupied_ = this->board_->totalOccupied;
    //this->setAttacked(this->board_);
}
void MoveGenerator::printAttacked() const {

    using std::cout;
    using std::endl;

    int pos;

    for(int rank = 7; rank >= 0; rank--){
        cout << rank + 1 << ' ';
        for(int file = 0; file < 8; file++){
            pos = rank * 8 + file;
            if(bit(this->attacked_,pos)) cout << 'X';
            else cout << (((rank + file) & 1) ? ' ' : '#');
            cout << ' ';
        }
        cout << endl;
    }
    cout << "  ";
    for(char col = 'a'; col <= 'h'; col++){
        cout << col << ' ';
    }
    cout << endl;
}