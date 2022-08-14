#include <iostream>

#include "pos.h"
#include "piece_list.h"

ChessPieceList::ChessPieceList() {
    this->pieceNum_ = 0;
}

void ChessPieceList::addPiece(ChessPos pos){
    this->positions_[this->pieceNum_] = pos;
    this->map_[pos.pos] = this->pieceNum_;
    this->pieceNum_++;
}
void ChessPieceList::removePiece(ChessPos pos){
    char ind = this->map_[pos.pos];
    this->positions_[ind] = this->positions_[this->pieceNum_ - 1];
    this->map_[this->positions_[ind].pos] = ind;
    this->pieceNum_--;
}
void ChessPieceList::movePiece(ChessPos oldPos, ChessPos newPos){
    char ind = this->map_[oldPos.pos];
    this->positions_[ind] = newPos;
    this->map_[newPos.pos] = ind;
}

void ChessPieceList::printList() const {
    for(int i = 0; i < this->pieceNum_; i++) std::cout << this->positions_[i].str() << ", ";
    std::cout << std::endl;
}