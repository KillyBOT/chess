#include <iostream>
#include <vector>
#include <cstring>

#include "move_generator.h"
#include "ray.h"

bool attacked_[64];
bool pinned_[64];

bool MoveGenerator::willMoveCapture(ChessMove &move) const {
    if(this->pieces_[move.newPos.pos].data && this->pieces_[move.newPos.pos].player() == this->opponent_) {
        move.captured = this->pieces_[move.newPos.pos];
        return true;
    }
    return false;
}
bool MoveGenerator::enPassantCheck(ChessMove &move) const {
    if(move.newPos.file() - 'a' + 1 == this->enPassantFile_){
        ChessPos capturePos = move.newPos;
        if(move.piece.player() == kPlayerWhite) capturePos.pos -= 8;
        else capturePos.pos += 8;

        if(
            this->pieces_[capturePos.pos].type() == kPiecePawn &&
            this->pieces_[capturePos.pos].player() == this->opponent_
        ){
            //TODO: add code that checks for that annoying en passant edge case
            move.captured = this->pieces_[capturePos.pos];
            move.moveData = kMoveEnPassant;
            return true;
        }
    }

    return false;
    
}
//TODO: add code that checks for if a piece is pinned, and therefore is unable to move in any direction outside of the 
void MoveGenerator::setAttackedForPiece(ChessPiece piece, ChessPos start) {

    int pos;

    switch(piece.type()){
        case kPiecePawn:
        if(piece.player() == kPlayerWhite){
            if(kRays[start.pos][kRayDirNE].size()) this->attacked_[kRays[start.pos][kRayDirNE][0]] = true;
            if(kRays[start.pos][kRayDirNW].size()) this->attacked_[kRays[start.pos][kRayDirNW][0]] = true;
        } else {
            if(kRays[start.pos][kRayDirSE].size()) this->attacked_[kRays[start.pos][kRayDirSE][0]] = true;
            if(kRays[start.pos][kRayDirSW].size()) this->attacked_[kRays[start.pos][kRayDirSW][0]] = true;
        }

        break;

        case kPieceKnight:
        for(int pos : kKnightPositionTable[start.pos]) this->attacked_[pos] = true;

        break;

        case kPieceRook:
        for(int dir = 0; dir < 4; dir++){
            pos = 0;
            while(pos < kRays[start.pos][dir].size() && !this->pieces_[kRays[start.pos][dir][pos]].data) {
                this->attacked_[kRays[start.pos][dir][pos]] = true;
                pos++;
            }
            if(pos < kRays[start.pos][dir].size()) this->attacked_[kRays[start.pos][dir][pos]] = true;
        }

        break;

        case kPieceBishop:

        for(int dir = 4; dir < 8; dir++){
            pos = 0;
            while(pos < kRays[start.pos][dir].size() && !this->pieces_[kRays[start.pos][dir][pos]].data) {
                this->attacked_[kRays[start.pos][dir][pos]] = true;
                pos++;
            }
            if(pos < kRays[start.pos][dir].size()) this->attacked_[kRays[start.pos][dir][pos]] = true;
        }

        break;

        case kPieceQueen:

        for(int dir = 0; dir < 8; dir++){
            pos = 0;
            while(pos < kRays[start.pos][dir].size() && !this->pieces_[kRays[start.pos][dir][pos]].data) {
                this->attacked_[kRays[start.pos][dir][pos]] = true;
                pos++;
            }
            if(pos < kRays[start.pos][dir].size()) this->attacked_[kRays[start.pos][dir][pos]] = true;
        }

        break;

        case kPieceKing:
        for(int dir = 0; dir < 8; dir++){
            if(kRays[start.pos][dir].size()) this->attacked_[kRays[start.pos][dir][pos]] = true;
        }
        
        break;
    }
}

//vector<ChessMove> MoveGenerator::genMovesForPiece(ChessPiece piece, ChessPos pos) const;

//void MoveGenerator::setPinned();
void MoveGenerator::setAttacked() {
    memset(this->attacked_, false, 64);

    for(int i = 0; i < this->pieceNum_; i++){
        ChessPos &pos = this->piecePositions_[i];
        ChessPiece &piece = this->pieces_[pos.pos];
        if(piece.player() == this->opponent_) this->setAttackedForPiece(piece, pos);
    }
}

MoveGenerator::MoveGenerator() {
    return;
}

bool MoveGenerator::stalemate() const {
    if(this->movesSinceLastCapture_ >= 50) return true;

    return false;
}
bool MoveGenerator::stalemate(ChessBoard &board) {
    this->setBoard(board);
    return this->stalemate();
}

bool MoveGenerator::inCheck() const {
    return this->attacked_[this->kingPos_.pos];
}
bool MoveGenerator::inCheck(ChessBoard &board) {
    this->setBoard(board);
    return this->inCheck();
}

// bool MoveGenerator::hasLost() const;
// bool MoveGenerator::hasLost(ChessBoard &board);

// vector<ChessMove> getMoves();
// vector<ChessMove> getMoves(ChessBoard &board);

void MoveGenerator::setBoard(ChessBoard &board) {

    memset(this->attacked_, 0, 64);
    memset(this->pinned_, 0, 64);

    this->player_ = board.player();
    this->opponent_ = board.opponent();
    this->pieceNum_ = board.pieceNum();
    this->pieces_ = (ChessPiece*)board.pieces();
    this->piecePositions_ = (ChessPos*)board.piecePositions();
    this->kingPos_ = board.kingPos(this->player_);
    this->enPassantFile_ = board.enPassantFile();

    this->setAttacked();
}


void MoveGenerator::printAttacked() {

    using std::cout;
    using std::endl;

    int pos;

    for(int rank = 7; rank >= 0; rank--){
        cout << rank + 1 << ' ';
        for(int file = 0; file < 8; file++){
            pos = rank * 8 + file;
            if(this->attacked_[pos]) cout << 'X';
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