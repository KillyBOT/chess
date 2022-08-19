#include <iostream>
#include <vector>

#include "ray.h"
#include "bitboard.h"
#include "move_generator.h"

MoveGenerator gMoveGenerator;

bool MoveGenerator::hasPieceAtPos(int ind) const {
    return this->occupied_ & kPosMasks[ind];
}
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

/*U64 MoveGenerator::dirAttacks(ChessPos start, int dir, U64 occupied) const {
    int first;
    switch(dir){
        case kRayDirN:
        case kRayDirE:
        case kRayDirNE:
        case kRayDirNW:
        first = bitscan_forward(occupied & kRayMasks[start][dir]);
        break;
        default:
        first = bitscan_reverse(occupied & kRayMasks[start][dir]);
        break;
    }
    if(first < 0) return kRayMasks[start][dir];
    else return kRayMasks[start][dir] & ~kRayMasks[first][dir];
}*/
/*void MoveGenerator::setAttacked(const ChessBoard *board) {
    this->attacked_ = 0;
    reset_bit(this->occupied_, this->kingPos_);
    this->genPawnAttacks(board);
    this->genKingAttacks(board);
    this->genKnightAttacks(board);
    this->genSlidingAttacks(board);
    set_bit(this->occupied_, this->kingPos_);
}
void MoveGenerator::genPawnAttacks(const ChessBoard *board) {
    // char dirL, dirR;

    U64 pawns = board->occupied[kPiecePawn, this->opponent_);
    ChessPos start;
    // if(this->opponent_ == kPlayerWhite){
    //     dirL = kRayDirNW;
    //     dirR = kRayDirNE;
    // } else {
    //     dirL = kRayDirSW;
    //     dirR = kRayDirSE;
    // }

    while(pawns){
        start = bitscan_forward_iter(pawns);
        //const ChessPos &start = list[i];
        this->attacked_ |= (this->opponent_ == kPlayerWhite ? kWhitePawnAttackMasks[start] : kBlackPawnAttackMasks[start]);

        //if(kRaySizes[start][dirL]) set_bit(this->attacked_,kRays[start][dirL][0]);
        //if(kRaySizes[start][dirR]) set_bit(this->attacked_,kRays[start][dirR][0]);
    }
}
void MoveGenerator::genKnightAttacks(const ChessBoard *board) {
    U64 knights = board->occupied(kPieceKnight, this->opponent_);
    ChessPos start;

    while(knights){
        start = bitscan_forward_iter(knights);
        // const ChessPos &start = list[i];
        this->attacked_ |= kKnightAttackMasks[start];
        // for(int j = 0; j < kKnightPositionTableSize[start]; j++) set_bit(this->attacked_,kKnightPositionTable[start][j]);
    }
}
void MoveGenerator::genKingAttacks(const ChessBoard *board) {

    this->attacked_ |= kKingAttackMasks[board->kingPos(this->opponent_)];
    // ChessPos start = this->board_->kingPos(this->opponent_);

    // for(int dir = 0; dir < 8; dir++){
    //     if(kRaySizes[start][dir]) set_bit(this->attacked_,kRays[start][dir][0]);
    // }
}
void MoveGenerator::genSlidingAttacks(const ChessBoard *board) {
    U64 rooks = board->occupied(kPieceRook, this->opponent_);
    U64 bishops = board->occupied(kPieceBishop, this->opponent_);
    U64 queens = board->occupied(kPieceQueen, this->opponent_);

    bool isPinned;
    int posInDir;
    ChessPos start;

    while(rooks){

        start = bitscan_forward_iter(rooks);
        for(int dir = 0; dir < 4; dir++) this->attacked_ |= this->dir_attacks(start, dir);
    }

    while(bishops){

        start = bitscan_forward_iter(bishops);
        for(int dir = 4; dir < 8; dir++) this->attacked_ |= this->dir_attacks(start, dir);

    }

    while(queens){

        start = bitscan_forward_iter(queens);
        for(int dir = 0; dir < 8; dir++) this->attacked_ |= this->dir_attacks(start, dir);
    }
}
*/
void MoveGenerator::genPseudoLegalMoves(vector<ChessMove> &moves) const {
    this->genKingMoves(moves);
    this->genPawnMoves(moves);
    this->genKnightMoves(moves);
    this->genSlidingMoves(moves);
}
void MoveGenerator::genLegalMoves(vector<ChessMove> &legalMoves, vector<ChessMove> &moves) {
    ChessBoard board(*this->board_);
    //for(ChessMove move : moves) std::cout << move.str() << std::endl;
    for(ChessMove &move : moves){
        board.doMove(move, false);
        //std::cout << move.str() << std::endl;
        //newBoard.printBoard();
        //this->occupied_ = board.totalOccupied;
        //this->kingPos_ = board.kingPos(this->player_);
        //this->setAttacked(&board);
        if(!this->inCheck(board,this->player_)) legalMoves.push_back(move);
        board.undoMove(move, false);
    }
}
void MoveGenerator::genKingMoves(vector<ChessMove> &moves) const {

    ChessMove move = ChessMove(this->board_->piece(this->kingPos_),this->kingPos_,this->kingPos_);
    //First check eight directions
    for(int dir = 0; dir < 8; dir++){
        if(kRaySizes[this->kingPos_][dir]){
            move.newPos = kRays[this->kingPos_][dir][0];
            //std::cout << move.newPos.str() << '\t' << this->attacked_[move.newPos.pos] << std::endl;
            if(!this->hasPieceAtPos(move.newPos) || this->willMoveCapture(move)) {
                moves.push_back(move);
                move.captured = 0;
            }
        }
    }

    //Then check if you can castle
    if(!this->inCheck() && ((this->player_ == kPlayerWhite && this->kingPos_ == new_pos("e1")) || (this->player_ == kPlayerBlack && this->kingPos_ == new_pos("e8"))) && !this->inCheck()){
        move.moveData = kMoveFlagIsCastling;
        //Queenside
        if(
            this->board_->canCastle(this->player_, false) &&
            this->board_->piece(new_pos('a',pos_rank_char(this->kingPos_))) == new_piece(kPieceRook, this->player_) &&
            !this->board_->piece(new_pos('b',pos_rank_char(this->kingPos_))) &&
            !this->board_->piece(new_pos('c',pos_rank_char(this->kingPos_))) &&
            !this->board_->piece(new_pos('d',pos_rank_char(this->kingPos_))) &&
            !bit(this->attacked_,new_pos('b',pos_rank_char(this->kingPos_))) &&
            !bit(this->attacked_,new_pos('c',pos_rank_char(this->kingPos_))) &&
            !bit(this->attacked_,new_pos('d',pos_rank_char(this->kingPos_)))
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
            !bit(this->attacked_,new_pos('f',pos_rank_char(this->kingPos_))) &&
            !bit(this->attacked_,new_pos('g',pos_rank_char(this->kingPos_)))
        ) {
            move.newPos = new_pos('g',pos_rank_char(this->kingPos_));
            moves.push_back(move);
        }
    }
}
void MoveGenerator::genKnightMoves(vector<ChessMove> &moves) const {

    U64 knights = this->board_->occupied[kPieceListInd[this->player_][kPieceKnight]];
    ChessPos start;

    while(knights){
        start = bitscan_forward_iter(knights);
        ChessMove move = ChessMove(this->board_->piece(start), start, start);

        for(int j = 0; j < kKnightPositionTableSize[start]; j++){
            move.newPos = kKnightPositionTable[start][j];
            if(!this->hasPieceAtPos(move.newPos) || this->willMoveCapture(move)) {
                moves.push_back(move);
                move.captured = 0;
            }
        }
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
        if(kRaySizes[start][dirF]){
            move.newPos = kRays[start][dirF][0];
            if(!this->hasPieceAtPos(move.newPos)){
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
        }

        if(doTwoMoveCheck && kRaySizes[start][dirF] > 1 && pos_rank(start) == startRank){
            move.newPos = kRays[start][dirF][1];
            if(!this->hasPieceAtPos(move.newPos)){
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

    while(rooks){

        start = bitscan_forward_iter(rooks);
        move = ChessMove(this->board_->piece(start),start,start);

        for(int dir = 0; dir < 4; dir++){
            //std::cout << start.str() << std::endl;
            for(posInDir = 0; posInDir < kRaySizes[start][dir] && !this->hasPieceAtPos(kRays[start][dir][posInDir]); posInDir++) {
                move.newPos = kRays[start][dir][posInDir];
                moves.push_back(move);
            }
            
            if(posInDir < kRaySizes[start][dir]) {
                move.newPos = kRays[start][dir][posInDir];
                if(this->willMoveCapture(move)) {
                    moves.push_back(move);
                    move.captured = 0;
                }
            }
        }
    }

    while(bishops){

        start = bitscan_forward_iter(bishops);
        move = ChessMove(this->board_->piece(start),start,start);

        for(int dir = 4; dir < 8; dir++){
            //std::cout << start.str() << std::endl;
            for(posInDir = 0; posInDir < kRaySizes[start][dir] && !this->hasPieceAtPos(kRays[start][dir][posInDir]); posInDir++) {
                move.newPos = kRays[start][dir][posInDir];
                moves.push_back(move);
            }
            
            if(posInDir < kRaySizes[start][dir]) {
                move.newPos = kRays[start][dir][posInDir];
                if(this->willMoveCapture(move)) {
                    moves.push_back(move);
                    move.captured = 0;
                }
            }
        }
    }

    while(queens){

        start = bitscan_forward_iter(queens);
        move = ChessMove(this->board_->piece(start),start,start);

        for(int dir = 0; dir < 8; dir++){
            //std::cout << start.str() << std::endl;
            for(posInDir = 0; posInDir < kRaySizes[start][dir] && !this->hasPieceAtPos(kRays[start][dir][posInDir]); posInDir++) {
                move.newPos = kRays[start][dir][posInDir];
                moves.push_back(move);
            }
            
            if(posInDir < kRaySizes[start][dir]) {
                move.newPos = kRays[start][dir][posInDir];
                if(this->willMoveCapture(move)) {
                    moves.push_back(move);
                    move.captured = 0;
                }
            }
        }
    }
}

MoveGenerator::MoveGenerator() {
    return;
}

bool MoveGenerator::fiftyMoveRuleStalemate() const {
    return this->board_->movesSinceLastCapture() > 50;
}
bool MoveGenerator::fiftyMoveRuleStalemate(ChessBoard &board) const {
    return board.movesSinceLastCapture() > 50;
}

bool MoveGenerator::stalemate() {
    if(this->fiftyMoveRuleStalemate()) return true;
    return this->getMoves().empty() && !this->inCheck();
}
bool MoveGenerator::stalemate(ChessBoard &board){
    this->setBoard(board);
    return this->stalemate();
}

bool MoveGenerator::inCheck() const {
    //return kPosMasks[this->kingPos_] & this->attacked_;
    U64 mask;
    U64 rooksAndQueens = this->board_->occupied[kPieceListInd[this->opponent_][kPieceRook]] | this->board_->occupied[kPieceListInd[this->opponent_][kPieceQueen]];
    U64 bishopsAndQueens = this->board_->occupied[kPieceListInd[this->opponent_][kPieceBishop]] | this->board_->occupied[kPieceListInd[this->opponent_][kPieceQueen]];

    //First check for pawns
    if(this->player_ == kPlayerWhite) mask = kWhitePawnAttackMasks[this->kingPos_];
    else mask = kBlackPawnAttackMasks[this->kingPos_];

    if(mask & this->board_->occupied[kPieceListInd[this->opponent_][kPiecePawn]]) return true;

    //Then check for kings
    if(kKingAttackMasks[this->kingPos_] & this->board_->occupied[kPieceListInd[this->opponent_][kPieceKing]]) return true;

    //Then check for knights
    if(kKnightAttackMasks[this->kingPos_] & this->board_->occupied[kPieceListInd[this->opponent_][kPieceKnight]]) return true;

    //Then check for rooks and queens
    // for(int dir = 0; dir < 4; dir++){
    //     //mask = dir_attacks(this->kingPos_, dir, this->occupied_);
    //     if(mask & rooksAndQueens) return true;
    // }
    // //Finally, check for bishops and queens
    // for(int dir = 4; dir < 8; dir++){
    //     //mask = dir_attacks(this->kingPos_, dir, this->occupied_);
    //     if(mask & bishopsAndQueens) return true;
    // }

    if(rookAttacks(this->kingPos_, this->board_->totalOccupied) & rooksAndQueens) return true;
    if(bishopAttacks(this->kingPos_, this->board_->totalOccupied) & bishopsAndQueens) return true;

    return false;
}
bool MoveGenerator::inCheck(ChessBoard &board, Player player) const {
    U64 mask;
    Player opponent = player_opponent(player);
    ChessPos kingPos = board.kingPos(player);
    U64 occupied = board.totalOccupied;
    U64 rooksAndQueens = board.occupied[kPieceListInd[opponent][kPieceRook]] | board.occupied[kPieceListInd[opponent][kPieceQueen]];
    U64 bishopsAndQueens = board.occupied[kPieceListInd[opponent][kPieceBishop]] | board.occupied[kPieceListInd[opponent][kPieceQueen]];

    //First check for pawns
    if(player == kPlayerWhite) mask = kWhitePawnAttackMasks[kingPos];
    else mask = kBlackPawnAttackMasks[kingPos];

    if(mask & board.occupied[kPieceListInd[opponent][kPiecePawn]]) return true;

    //Then check for kings
    if(kKingAttackMasks[kingPos] & board.occupied[kPieceListInd[opponent][kPieceKing]]) return true;

    //Then check for knights
    if(kKnightAttackMasks[kingPos] & board.occupied[kPieceListInd[opponent][kPieceKnight]]) return true;

    // //Then check for rooks and queens
    // for(int dir = 0; dir < 4; dir++){
    //     mask = dir_attacks(kingPos, dir, occupied);
    //     if(mask & rooksAndQueens) return true;
    // }
    // //Finally, check for bishops and queens
    // for(int dir = 4; dir < 8; dir++){
    //     mask = dir_attacks(kingPos, dir, occupied);
    //     if(mask & bishopsAndQueens) return true;
    // }

    if(rookAttacks(kingPos, occupied) & rooksAndQueens) return true;
    if(bishopAttacks(kingPos, occupied) & bishopsAndQueens) return true;

    return false;
}

bool MoveGenerator::hasLost() {
    return this->inCheck() & this->getMoves().empty();
}
bool MoveGenerator::hasLost(ChessBoard &board) {
    this->setBoard(board);
    return this->hasLost();
}

vector<ChessMove> MoveGenerator::getMoves() {
    vector<ChessMove> pseudoLegalMoves, legalMoves;
    this->genPseudoLegalMoves(pseudoLegalMoves);
    this->genLegalMoves(legalMoves, pseudoLegalMoves);
    return legalMoves;
}
vector<ChessMove> MoveGenerator::getMoves(ChessBoard &board) {
    this->setBoard(board);
    return this->getMoves();
}

void MoveGenerator::setBoard(ChessBoard &board) {
    this->board_ = &board;
    this->player_ = board.player();
    this->opponent_ = board.opponent();
    this->kingPos_ = board.kingPos(this->player_);
    this->enPassantSquare_ = board.enPassantSquare();

    this->playerOccupied_ = 0;
    this->opponentOccupied_ = 0;
    for(PieceType type = kPiecePawn; type <= kPieceKing; type++){
        this->playerOccupied_ |= board.occupied[kPieceListInd[this->player_][type]];
        this->opponentOccupied_ |= board.occupied[kPieceListInd[this->opponent_][type]];
    }
    
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