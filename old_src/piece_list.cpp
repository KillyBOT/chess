#include <iostream>
#include <cstring>

#include "pos.h"
#include "piece_list.h"
#include "bitboard.h"

ChessPieceList::ChessPieceList() {
    this->pieceNum_ = 0;
    memset(this->positions_, 0, 64 * sizeof(ChessPos));
    memset(this->map_, 0, 64);
}

void ChessPieceList::addPiece(ChessPos pos){
    this->positions_[this->pieceNum_] = pos;
    this->map_[pos] = this->pieceNum_;
    set_bit(this->bb_, pos);
    this->pieceNum_++;
}
void ChessPieceList::removePiece(ChessPos pos){
    char ind = this->map_[pos];
    this->positions_[ind] = this->positions_[this->pieceNum_ - 1];
    this->map_[this->positions_[ind]] = ind;
    reset_bit(this->bb_, pos);
    this->pieceNum_--;
}
void ChessPieceList::movePiece(ChessPos oldPos, ChessPos newPos){
    char ind = this->map_[oldPos];
    this->positions_[ind] = newPos;
    this->map_[newPos] = ind;
    set_bit(this->bb_, newPos);
    reset_bit(this->bb_,oldPos);
}

void ChessPieceList::printList() const {
    for(int i = 0; i < this->pieceNum_; i++) std::cout << pos_str(this->positions_[i]) << ", ";
    std::cout << std::endl;
}