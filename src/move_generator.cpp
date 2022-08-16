#include <iostream>
#include <vector>
#include <cstring>
#include <unordered_map>

#include "board.h"
#include "piece_list.h"
#include "move_generator.h"
#include "ray.h"
#include "bitboard.h"

MoveGenerator gMoveGenerator;

static const char kPieceListInd[2][7] = {0, 0b0001, 0b0010, 0b0011, 0b0100, 0b0101, 0b0110, 0, 0b1001, 0b1010, 0b1011, 0b1100, 0b1101, 0b1110};

//unordered_map<std::size_t, std::vector<ChessMove>> MoveGenerator::knownBoards_ = unordered_map<std::size_t, std::vector<ChessMove>>();

bool MoveGenerator::willMoveCapture(ChessMove &move) const {
    if(this->board_->piece(move.newPos) && piece_player(this->board_->piece(move.newPos)) == this->opponent_) {
        move.captured = this->board_->piece(move.newPos);
        return true;
    }
    return false;
}
bool MoveGenerator::enPassantCheck(ChessMove &move) const {
    if(this->doEnPassantCheck_ && move.newPos == this->board_->enPassantSquare()){
        ChessPos capturePos = move.newPos;
        if(piece_player(move.piece) == kPlayerWhite) capturePos -= 8;
        else capturePos += 8;

        if(
            piece_type(this->board_->piece(capturePos)) == kPiecePawn &&
            piece_player(this->board_->piece(capturePos)) == this->opponent_
        ){

            //Check for annoying edge case where the en passant capture puts your king in check
            vector<ChessPos> pieces;
            this->addPiecesInDir(pieces, this->kingPos_, kRayDirE);
            if(
                pieces.size() > 2 &&
                pieces[0] == move.oldPos &&
                pieces[1] == capturePos &&
                piece_player(this->board_->piece(pieces[2])) == this->opponent_ &&
                (piece_type(this->board_->piece(pieces[2]))== kPieceRook || piece_type(this->board_->piece(pieces[2])) == kPieceQueen)
            ) return false;

            pieces.clear();
            this->addPiecesInDir(pieces, this->kingPos_, kRayDirW);
            if(
                pieces.size() > 2 &&
                pieces[0] == move.oldPos &&
                pieces[1] == capturePos &&
                piece_player(this->board_->piece(pieces[2])) == this->opponent_ &&
                (piece_type(this->board_->piece(pieces[2])) == kPieceRook || piece_type(this->board_->piece(pieces[2])) == kPieceQueen)
            ) return false;

            move.captured = this->board_->piece(capturePos);
            move.moveData = kMoveEnPassant;

            return true;
        }
    }

    return false;
    
}

void MoveGenerator::addPiecesInDir(vector<ChessPos> &positions, ChessPos start, int dir) const{
    //std::cout << start.str() << '\t' << dir << '\t' << kRays[start.pos][dir].size() << std::endl;
    for(ChessPos i = 0; i < kRaySizes[i][dir]; i++){
        ChessPos pos = kRays[start][dir][i];
        if(this->board_->piece(pos)) positions.push_back(pos);
    }
}

void MoveGenerator::genDiagAttacks(ChessPos start) {
    //std::cout << pos_str(start) << std::endl;
    BitBoard forward, reverse;
    forward = this->occupied_ & kDiagMasks[start];
    reverse  = flip_vertical(forward);
    forward -= kPosMasks[start] << 1;
    reverse -= flip_vertical(kPosMasks[start]) << 1;
    forward ^= flip_vertical(reverse);
    forward &= kDiagMasks[start];

    //print_bitboard(this->attacked_);
    this->attacked_ |= forward;
    //print_bitboard(this->attacked_);

}

void MoveGenerator::genAntiDiagAttacks(ChessPos start) {
    //std::cout << pos_str(start) << std::endl;
    BitBoard forward, reverse;
    forward  = this->occupied_ & kAntiDiagMasks[start];
    reverse  = flip_vertical(forward);
    forward -= kPosMasks[start] << 1;
    reverse -= flip_vertical(kPosMasks[start]) << 1;
    forward ^= flip_vertical(reverse);
    forward &= kAntiDiagMasks[start];

    //print_bitboard(this->attacked_);
    this->attacked_ |= forward;
    //print_bitboard(this->attacked_);

}

void MoveGenerator::genFileAttacks(ChessPos start) {
    //std::cout << pos_str(start) << std::endl;
    BitBoard forward, reverse;
    forward  = this->occupied_ & kFileMasks[pos_file(start)];
    reverse  = flip_vertical(forward);
    forward -= kPosMasks[start] << 1;
    reverse -= flip_vertical(kPosMasks[start]) << 1;
    forward ^= flip_vertical(reverse);
    forward &= kFileMasks[pos_file(start)];

    //print_bitboard(this->attacked_);
    this->attacked_ |= forward;
    //print_bitboard(this->attacked_);

}
void MoveGenerator::genRankAttacks(ChessPos start){
    //std::cout << pos_str(start) << std::endl;
    BitBoard forward, reverse;
    forward  = this->occupied_ & kRankMasks[pos_rank(start)];
    reverse  = flip_horizontal(forward);
    forward -= kPosMasks[start] << 1;
    reverse -= flip_horizontal(kPosMasks[start] >> 1);
    //reverse ^= flip_horizontal(this->board_->occupied());
    //forward ^= this->board_->occupied();
    forward ^= flip_horizontal(reverse);
    forward &= kRankMasks[pos_rank(start)];

    //print_bitboard(this->attacked_);
    this->attacked_ |= forward;
    //print_bitboard(this->attacked_);
}
void MoveGenerator::genPawnAttacks() {
    char dirL, dirR;

    const ChessPieceList &list = this->board_->pieceList(kPieceListInd[this->opponent_][kPiecePawn]);

    if(this->opponent_ == kPlayerWhite){
        dirL = kRayDirNW;
        dirR = kRayDirNE;
    } else {
        dirL = kRayDirSW;
        dirR = kRayDirSE;
    }

    for(int i = 0; i < list.size(); i++){
        const ChessPos &start = list[i];

        if(kRaySizes[start][dirL]) set_bit(this->attacked_,kRays[start][dirL][0]);
        if(kRaySizes[start][dirR]) set_bit(this->attacked_,kRays[start][dirR][0]);
    }
}
void MoveGenerator::genKnightAttacks() {
    const ChessPieceList &list = this->board_->pieceList(kPieceListInd[this->opponent_][kPieceKnight]);

    for(int i = 0; i < list.size(); i++){
        const ChessPos &start = list[i];
        for(int j = 0; j < kKnightPositionTableSize[start]; j++) set_bit(this->attacked_,kKnightPositionTable[start][j]);
    }
}
void MoveGenerator::genKingAttacks() {

    ChessPos start = this->board_->kingPos(this->opponent_);

    for(int dir = 0; dir < 8; dir++){
        if(kRaySizes[start][dir]) set_bit(this->attacked_,kRays[start][dir][0]);
    }
}
void MoveGenerator::genSlidingAttacks() {
    const ChessPieceList &rookList = this->board_->pieceList(kPieceListInd[this->opponent_][kPieceRook]);
    const ChessPieceList &bishopList = this->board_->pieceList(kPieceListInd[this->opponent_][kPieceBishop]);
    const ChessPieceList &queenList = this->board_->pieceList(kPieceListInd[this->opponent_][kPieceQueen]);

    bool isPinned;
    int posInDir;
    ChessMove move;

    BitBoard occupied = this->board_->occupied();
    BitBoard potBlocks;
    reset_bit(occupied, this->kingPos_);

    for(int i = 0; i < rookList.size(); i++){

        const ChessPos &start = rookList[i];
        this->genFileAttacks(start);
        this->genRankAttacks(start);
        // this->attacked_ |= (occupied ^ ((occupied) - (kOne << (start + 1)))) & kFileMasks[pos_file(start)];
        //this->attacked_ |= (occupied ^ (occupied - (kOne << (start + 1)))) & kRankMasks[pos_rank(start)];
        //print_bitboard(this->attacked_);
        //this->attacked_ |= ((occupied & kFileMasks[pos_file(start)])) & kFileMasks[pos_file(start)];
        //this->attacked_ |= ((occupied & kRankMasks[pos_rank(start)])) & kRankMasks[pos_rank(start)];

        // for(int dir = 0; dir < 4; dir++){
            
        //     for(posInDir = 0; posInDir < kRaySizes[start][dir] && (!this->board_->piece(kRays[start][dir][posInDir]) || kRays[start][dir][posInDir] == this->kingPos_); posInDir++){
        //         set_bit(this->attacked_, kRays[start][dir][posInDir]);
        //     }
            
        //     if(posInDir < kRaySizes[start][dir]) set_bit(this->attacked_,kRays[start][dir][posInDir]);
        // }
    }

    for(int i = 0; i < bishopList.size(); i++){

        const ChessPos &start = bishopList[i];
        this->genDiagAttacks(start);
        this->genAntiDiagAttacks(start);

        // for(int dir = 4; dir < 8; dir++){
            
        //     for(posInDir = 0; posInDir < kRaySizes[start][dir] && (!this->board_->piece(kRays[start][dir][posInDir]) || kRays[start][dir][posInDir] == this->kingPos_); posInDir++){
        //         set_bit(this->attacked_, kRays[start][dir][posInDir]);
        //     }
            
        //     if(posInDir < kRaySizes[start][dir]) set_bit(this->attacked_,kRays[start][dir][posInDir]);
        // }

    }

    for(int i = 0; i < queenList.size(); i++){

        const ChessPos &start = queenList[i];
        this->genFileAttacks(start);
        this->genRankAttacks(start);
        this->genDiagAttacks(start);
        this->genAntiDiagAttacks(start);

        // for(int dir = 0; dir < 8; dir++){
            
        //     for(posInDir = 0; posInDir < kRaySizes[start][dir] && (!this->board_->piece(kRays[start][dir][posInDir]) || kRays[start][dir][posInDir] == this->kingPos_); posInDir++){
        //         set_bit(this->attacked_, kRays[start][dir][posInDir]);
        //     }
            
        //     if(posInDir < kRaySizes[start][dir]) set_bit(this->attacked_,kRays[start][dir][posInDir]);
        // }
    }
}

void MoveGenerator::genKingMoves(vector<ChessMove> &moves) const {

    ChessMove move = ChessMove(this->board_->piece(this->kingPos_),this->kingPos_,this->kingPos_);
    //First check eight directions
    for(int dir = 0; dir < 8; dir++){
        if(kRaySizes[this->kingPos_][dir]){
            move.newPos = kRays[this->kingPos_][dir][0];
            //std::cout << move.newPos.str() << '\t' << this->attacked_[move.newPos.pos] << std::endl;
            if(!bit(this->attacked_, move.newPos) && (!this->board_->piece(move.newPos) || this->willMoveCapture(move))){
                moves.push_back(move);
                move.captured = 0;
            }
        }
    }

    //Then check if you can castle
    if(((this->player_ == kPlayerWhite && this->kingPos_ == new_pos("e1")) || (this->player_ == kPlayerBlack && this->kingPos_ == new_pos("e8"))) && !this->inCheck()){
        move.moveData = kMoveIsCastling;
        //Queenside
        if(
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

    const ChessPieceList &list = this->board_->pieceList(kPieceListInd[this->player_][kPieceKnight]);

    for(int i = 0; i < list.size(); i++){
        const ChessPos &start = list[i];

        //If a knight is pinned, it cannot move
        if(bit(this->pinned_,start)) continue;

        for(int j = 0; j < kKnightPositionTableSize[start]; j++){
            ChessMove move(this->board_->piece(start), start, kKnightPositionTable[start][j]);
            if(!this->board_->piece(move.newPos) || this->willMoveCapture(move)) this->addMove(moves,move);
        }
    }
}
void MoveGenerator::genPawnMoves(vector<ChessMove> &moves) const {

    char dirL, dirR, dirF, startRank, promoteRank;
    bool isPinned, doTwoMoveCheck;
    ChessMove move;

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

    const ChessPieceList &list = this->board_->pieceList(kPieceListInd[this->player_][kPiecePawn]);

    for(int i = 0; i < list.size(); i++){
        const ChessPos &start = list[i];
        move = ChessMove(this->board_->piece(start), start, start);
        isPinned = bit(this->pinned_,start);

        if(kRaySizes[start][dirL] && (!isPinned || this->pinnedDirs_[start][dirL])){
            move.newPos = kRays[start][dirL][0];
            if(this->willMoveCapture(move) || this->enPassantCheck(move)){
                if(pos_rank(move.newPos) == promoteRank){
                    move.moveData = kMovePromotingToBishop;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToRook;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToKnight;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToQueen;
                }

                this->addMove(moves,move);
                move.moveData = kMoveNone;
            }
        }
        if(kRaySizes[start][dirR] && (!isPinned || this->pinnedDirs_[start][dirR])){
            move.newPos = kRays[start][dirR][0];
            if(this->willMoveCapture(move) || this->enPassantCheck(move)){
                if(pos_rank(move.newPos) == promoteRank){
                    move.moveData = kMovePromotingToBishop;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToRook;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToKnight;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToQueen;
                }

                this->addMove(moves,move);
                move.moveData = kMoveNone;
            }
        }

        move.captured = ChessPiece();

        doTwoMoveCheck = false;
        if(kRaySizes[start][dirF] && (!isPinned || this->pinnedDirs_[start][dirF])){
            move.newPos = kRays[start][dirF][0];
            if(!this->board_->piece(move.newPos)){
                doTwoMoveCheck = true;
                if(pos_rank(move.newPos) == promoteRank){
                    move.moveData = kMovePromotingToBishop;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToRook;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToKnight;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToQueen;
                }
                this->addMove(moves,move);
                move.moveData = kMoveNone;
            }
        }

        if(doTwoMoveCheck && kRaySizes[start][dirF] > 1 && pos_rank(start) == startRank){
            move.newPos = kRays[start][dirF][1];
            if(!this->board_->piece(move.newPos)){
                if(pos_rank(move.newPos) == promoteRank){
                    move.moveData = kMovePromotingToBishop;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToRook;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToKnight;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToQueen;
                } else {
                    move.moveData = kMoveEnPassantEligible;
                }

                this->addMove(moves,move);
            }
        }
    }

}
void MoveGenerator::genSlidingMoves(vector<ChessMove> &moves) const {
    const ChessPieceList &rookList = this->board_->pieceList(kPieceListInd[this->player_][kPieceRook]);
    const ChessPieceList &bishopList = this->board_->pieceList(kPieceListInd[this->player_][kPieceBishop]);
    const ChessPieceList &queenList = this->board_->pieceList(kPieceListInd[this->player_][kPieceQueen]);

    bool isPinned;
    int posInDir;
    ChessMove move;

    for(int i = 0; i < rookList.size(); i++){

        const ChessPos &start = rookList[i];
        isPinned = bit(this->pinned_,start);
        move = ChessMove(this->board_->piece(start),start,start);

        for(int dir = 0; dir < 4; dir++){
            if(isPinned && !this->pinnedDirs_[start][dir]) continue;
            //std::cout << start.str() << std::endl;
            for(posInDir = 0; posInDir < kRaySizes[start][dir] && !this->board_->piece(kRays[start][dir][posInDir]); posInDir++) {
                move.newPos = kRays[start][dir][posInDir];
                this->addMove(moves,move);
            }
            
            if(posInDir < kRaySizes[start][dir]) {
                move.newPos = kRays[start][dir][posInDir];
                if(this->willMoveCapture(move)) {
                    this->addMove(moves,move);
                    move.captured = 0;
                }
            }
        }
    }

    for(int i = 0; i < bishopList.size(); i++){

        const ChessPos &start = bishopList[i];
        isPinned = bit(this->pinned_,start);
        move = ChessMove(this->board_->piece(start),start,start);

        for(int dir = 4; dir < 8; dir++){
            if(isPinned && !this->pinnedDirs_[start][dir]) continue;
            //std::cout << start.str() << std::endl;
            for(posInDir = 0; posInDir < kRaySizes[start][dir] && !this->board_->piece(kRays[start][dir][posInDir]); posInDir++) {
                move.newPos = kRays[start][dir][posInDir];
                this->addMove(moves,move);
            }
            
            if(posInDir < kRaySizes[start][dir]) {
                move.newPos = kRays[start][dir][posInDir];
                if(this->willMoveCapture(move)) {
                    this->addMove(moves,move);
                    move.captured = 0;
                }
            }
        }

    }

    for(int i = 0; i < queenList.size(); i++){

        const ChessPos &start = queenList[i];
        isPinned = bit(this->pinned_,start);
        move = ChessMove(this->board_->piece(start),start,start);

        for(int dir = 0; dir < 8; dir++){
            if(isPinned && !this->pinnedDirs_[start][dir]) continue;
            //std::cout << start.str() << std::endl;
            for(posInDir = 0; posInDir < kRaySizes[start][dir] && !this->board_->piece(kRays[start][dir][posInDir]); posInDir++) {
                move.newPos = kRays[start][dir][posInDir];
                this->addMove(moves,move);
            }
            
            if(posInDir < kRaySizes[start][dir]) {
                move.newPos = kRays[start][dir][posInDir];
                if(this->willMoveCapture(move)) {
                    this->addMove(moves,move);
                    move.captured = 0;
                }
            }
        }
    }
}

void MoveGenerator::setAttacked() {
    this->attacked_ = 0;
    // for(int i = 0; i < this->board_->pieceNum(); i++){
    //     ChessPos pos = this->board_->piecePositions()[i];
    //     ChessPiece piece = this->board_->piece(pos);
    //     if(piece.player() == this->opponent_) this->setAttackedForPiece(piece, pos);
    // }

    this->genPawnAttacks();
    this->genKingAttacks();
    this->genKnightAttacks();
    this->genSlidingAttacks();
}
void MoveGenerator::setPinnedAndForced() {
    memset(this->pinnedDirs_,false, 512);
    this->pinned_ = 0;
    this->forced_ = 0;
    this->hasForced_ = false;
    this->cannotMove_ = false;
    PieceType pieceToCheck = kPieceRook;

    char dirL, dirR;

    //First look in 8 directions for sliding pieces

    bool foundPinned;
    char pinnedPos;

    //First look in 8 directions for rooks, bishops, and queens
    for(int dir = 0; dir < 8; dir++){
        foundPinned = false;
        if(dir == 4) pieceToCheck = kPieceBishop;
        //For each position in the direction
        for(int posInDir = 0; posInDir < kRaySizes[this->kingPos_][dir]; posInDir++){
            if(this->board_->piece(kRays[this->kingPos_][dir][posInDir])){
                const ChessPiece &piece = this->board_->piece(kRays[this->kingPos_][dir][posInDir]);

                if(piece_player(piece) == this->player_){
                    if(!foundPinned){
                        //This could potentially be a pinned piece
                        foundPinned = true;
                        pinnedPos = kRays[this->kingPos_][dir][posInDir];

                    } else {
                        //Two friendly pieces in this direction, stop looking in this direction
                        break;

                    }

                } else {
                    if(piece_type(piece) == pieceToCheck || piece_type(piece) == kPieceQueen){
                        if(!foundPinned){
                            if(!hasForced_){
                                //Update forced positions to include everything up to and including posInDir
                                hasForced_ = true;
                                for(int posToAdd = 0; posToAdd <= posInDir; posToAdd++) set_bit(this->forced_, kRays[this->kingPos_][dir][posToAdd]);
                                break;
                            } else {
                                //Nothing but the king can move, so set cannotMove to true and return
                                cannotMove_ = true;
                                return;

                            }
                        } else { //Make the previously found pinned piece pinned and stop looking in this direction
                            set_bit(this->pinned_, pinnedPos);
                            this->pinnedDirs_[pinnedPos][dir] = true;
                            this->pinnedDirs_[pinnedPos][opposite_dir(dir)] = true;
                            break;

                        }
                    } else { //Found an opponent knight or pawn or knight, stop looking in this direction
                        break;
                    }
                }
            }
        }
    }

    //Then look for pawns
    if(this->player_ == kPlayerWhite){
        dirL = kRayDirNW;
        dirR = kRayDirNE;
    } else {
        dirL = kRayDirSW;
        dirR = kRayDirSE;
    }

    if(
        kRaySizes[this->kingPos_][dirL] &&
        this->board_->piece(kRays[this->kingPos_][dirL][0]) &&
        piece_type(this->board_->piece(kRays[this->kingPos_][dirL][0]))== kPiecePawn &&
        piece_player(this->board_->piece(kRays[this->kingPos_][dirL][0])) == this->opponent_
    ){
        if(!this->hasForced_){
            this->hasForced_ = true;
            set_bit(this->forced_, kRays[this->kingPos_][dirL][0]);
        } else {
            this->cannotMove_ = true;
            return;
        }
    }
    if(
        kRaySizes[this->kingPos_][dirR] &&
        this->board_->piece(kRays[this->kingPos_][dirR][0]) &&
        piece_type(this->board_->piece(kRays[this->kingPos_][dirR][0])) == kPiecePawn &&
        piece_player(this->board_->piece(kRays[this->kingPos_][dirR][0])) == this->opponent_
    ){
        if(!this->hasForced_){
            this->hasForced_ = true;
            set_bit(this->forced_, kRays[this->kingPos_][dirL][0]);
        } else {
            this->cannotMove_ = true;
            return;
        }
    }

    //Finally, look for knights
    for(int i = 0; i < kKnightPositionTableSize[this->kingPos_]; i++){
        ChessPos pos = kKnightPositionTable[this->kingPos_][i];
        if(
            this->board_->piece(pos) &&
            piece_type(this->board_->piece(pos)) == kPieceKnight &&
            piece_player(this->board_->piece(pos)) == this->opponent_
        ){
            if(!this->hasForced_){
                this->hasForced_ = true;
                set_bit(this->forced_, pos);
            } else {
                this->cannotMove_ = true;
                return;
            }
        }
    }
}

MoveGenerator::MoveGenerator() {
    return;
}

bool MoveGenerator::stalemate() {
    if(this->getMoves().empty() && !this->inCheck()) return true;
    if(this->board_->movesSinceLastCapture() >= 100) return true;

    return false;
}
bool MoveGenerator::stalemate(ChessBoard &board) {
    this->setBoard(board);
    return this->stalemate();
}

bool MoveGenerator::fiftyMoveRuleStalemate() const {
    return this->board_->movesSinceLastCapture() >= 100;
}
bool MoveGenerator::fiftyMoveRuleStalemate(ChessBoard &board) const {
    return board.movesSinceLastCapture() >= 100;
}

bool MoveGenerator::inCheck() const {
    return this->hasForced_;
}
bool MoveGenerator::inCheck(ChessBoard &board) {
    this->setBoard(board);
    return this->hasForced_;
}

bool MoveGenerator::hasLost() const {
    return (this->inCheck() && this->getMoves().empty());
}
bool MoveGenerator::hasLost(ChessBoard &board) {
    this->setBoard(board);
    return this->hasLost();
}

vector<ChessMove> MoveGenerator::getMoves() const {
    //std::cout << "Generating moves for board:" << std::endl;
    //std::cout << this->board_->pieceNum() << std::endl;
    //this->board_->printBoard();

        //this->board_->printBoard();
    //std::cout << this->kingPos_.str() << std::endl;
    vector<ChessMove> moves;

    this->genKingMoves(moves);

    if(this->cannotMove_) return moves;

    this->genPawnMoves(moves);
    this->genKnightMoves(moves);
    this->genSlidingMoves(moves);

    return moves;

    
}
vector<ChessMove> MoveGenerator::getMoves(ChessBoard &board) {
    this->setBoard(board);
    return this->getMoves();
}

/*const vector<ChessMove> &MoveGenerator::getMoves() const {

    if(!MoveGenerator::knownBoards_.count(this->board_->key())){

        //std::cout << "Generating moves for board:" << std::endl;
        //std::cout << this->board_->pieceNum() << std::endl;
        //this->board_->printBoard();

        vector<ChessMove> moves;

        for(int i = 0; i < this->board_->pieceNum(); i++){
            ChessPos pos = this->board_->piecePositions()[i];
            ChessPiece piece = this->board_->piece(pos);
            if(piece.player() == this->player_) this->genMovesForPiece(moves, piece, pos);
        }

        if(this->hasForced_){
            //std::cout << "In check!" << std::endl;
            vector<ChessMove> filteredMoves;
            for(ChessMove move : moves){
                if(move.piece.type() == kPieceKing) filteredMoves.push_back(move);
                else if(!this->cannotMove_ && this->forced_[move.newPos.pos]) filteredMoves.push_back(move);
            }

            moves = filteredMoves;
        }

        // std::cout << "Moves generated: ";
        // for(ChessMove move : moves) std::cout << move.str() << ", ";
        // std::cout << std::endl;

        MoveGenerator::knownBoards_.emplace(this->board_->key(),moves);

    }

    return MoveGenerator::knownBoards_.at(this->board_->key());
    
}
const vector<ChessMove> &MoveGenerator::getMoves(ChessBoard &board) {
    if(!MoveGenerator::knownBoards_.count(board.key())) {
        this->setBoard(board);
        return this->getMoves();
    }
    else return MoveGenerator::knownBoards_.at(board.key());
}*/

void MoveGenerator::setBoard(ChessBoard &board) {

    this->board_ = &board;
    this->player_ = board.player();
    this->opponent_ = board.opponent();
    this->kingPos_ = board.kingPos(this->player_);
    this->doEnPassantCheck_ = pos_in_bounds(board.enPassantSquare());

    //std::cout << this->kingPos_.str() << std::endl;

    this->occupied_ = board_->occupied();
    reset_bit(this->occupied_, this->kingPos_);
    this->setAttacked();
    this->setPinnedAndForced();
    //this->setPinned();
    //this->setForced();
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

void MoveGenerator::printForced() const {

    using std::cout;
    using std::endl;

    int pos;

    for(int rank = 7; rank >= 0; rank--){
        cout << rank + 1 << ' ';
        for(int file = 0; file < 8; file++){
            pos = rank * 8 + file;
            if(bit(this->forced_,pos)) cout << 'X';
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