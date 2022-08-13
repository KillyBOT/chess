#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstring>
#include <cstdlib>

#include "board.h"

using std::vector;
using std::size_t;

size_t kZobristPieceNums[64][12];
size_t kZobristBlackToMoveNum;
size_t kZobristCastlingNums[16];
size_t kZobristEnPassantNums[9];

void init_zobrist_nums() {
    srand(43252003); //The first 8 digits of the number of possible rubik's cube configurations

    for(int pos = 0; pos < 64; pos++){
        for(int type = 0; type < 12; type++){
            kZobristPieceNums[pos][type] = rand();
        }
    }

    kZobristBlackToMoveNum = rand();
    for(int i = 0; i < 16; i++) kZobristCastlingNums[i] = rand();
    for(int i = 0; i < 9; i++) kZobristEnPassantNums[i] = rand();
}

ChessBoard::ChessBoard(bool initBoard){

    using std::string;

    this->pieceNum_ = 0;
    this->blackToMove_ = false;
    this->whiteKingPos_ = ChessPos();
    this->blackKingPos_ = ChessPos();

    memset(this->pieces_, 0, 64);
    memset(this->pieceMap_, 0, 64);
    memset(this->piecePositions_, 0, 64);

    if(initBoard){

        string whitePawnStr, blackPawnStr;

        this->addPiece(ChessPos("a1"), ChessPiece(kPieceRook, kPlayerWhite));
        this->addPiece(ChessPos("b1"), ChessPiece(kPieceKnight, kPlayerWhite));
        this->addPiece(ChessPos("c1"), ChessPiece(kPieceBishop, kPlayerWhite));
        this->addPiece(ChessPos("d1"), ChessPiece(kPieceQueen, kPlayerWhite));
        this->addPiece(ChessPos("e1"), ChessPiece(kPieceKing, kPlayerWhite));
        this->addPiece(ChessPos("f1"), ChessPiece(kPieceBishop, kPlayerWhite));
        this->addPiece(ChessPos("g1"), ChessPiece(kPieceKnight, kPlayerWhite));
        this->addPiece(ChessPos("h1"), ChessPiece(kPieceRook, kPlayerWhite));

        this->addPiece(ChessPos("a8"), ChessPiece(kPieceRook, kPlayerBlack));
        this->addPiece(ChessPos("b8"), ChessPiece(kPieceKnight, kPlayerBlack));
        this->addPiece(ChessPos("c8"), ChessPiece(kPieceBishop, kPlayerBlack));
        this->addPiece(ChessPos("d8"), ChessPiece(kPieceQueen, kPlayerBlack));
        this->addPiece(ChessPos("e8"), ChessPiece(kPieceKing, kPlayerBlack));
        this->addPiece(ChessPos("f8"), ChessPiece(kPieceBishop, kPlayerBlack));
        this->addPiece(ChessPos("g8"), ChessPiece(kPieceKnight, kPlayerBlack));
        this->addPiece(ChessPos("h8"), ChessPiece(kPieceRook, kPlayerBlack));

        for(char c = 'a'; c <= 'h'; c++){ //Yoo like C++ that's so crazy bro
            whitePawnStr = c;
            whitePawnStr += '2';
            blackPawnStr = c;
            blackPawnStr += '7';
            this->addPiece(ChessPos(whitePawnStr), ChessPiece(kPiecePawn, kPlayerWhite));
            this->addPiece(ChessPos(blackPawnStr), ChessPiece(kPiecePawn, kPlayerBlack));
        }
    }

    this->resetKeys();
}
ChessBoard::ChessBoard(const ChessBoard &board) {
    memcpy(this->pieces_,board.pieces_, 64 * sizeof(ChessPiece));
    memcpy(this->piecePositions_, board.piecePositions_, 64 * sizeof(ChessPos));
    memcpy(this->pieceMap_, board.pieceMap_, 64);

    this->boardData_ = board.boardData_;
    this->boardHistory_ = board.boardHistory_;
    this->moves_ = board.moves_;

    this->blackToMove_ = board.blackToMove_;
    this->pieceNum_ = board.pieceNum_;
    this->whiteKingPos_ = board.whiteKingPos_;
    this->blackKingPos_ = board.blackKingPos_;
}

void ChessBoard::addNewKey(ChessMove move) {
    size_t newKey = this->key();
    Byte castlingInfo = this->boardData_.back() & 0b1111;
    unsigned short newData = 0;

    newKey ^= kZobristEnPassantNums[(this->boardData_.back() >> 4) & 0b1111];
    newKey ^= kZobristCastlingNums[castlingInfo];

    //If castling, remove the rook from the old position and add it at the new position
    if(move.isCastling()){
        if(move.isCastlingKingside()){
            newKey ^= kZobristPieceNums[ChessPos('h',move.oldPos.rank()).pos][(kPieceRook - 1) + 6 * move.piece.player()];
            newKey ^= kZobristPieceNums[ChessPos('f',move.oldPos.rank()).pos][(kPieceRook - 1) + 6 * move.piece.player()];
        } else {
            newKey ^= kZobristPieceNums[ChessPos('a',move.oldPos.rank()).pos][(kPieceRook - 1) + 6 * move.piece.player()];
            newKey ^= kZobristPieceNums[ChessPos('c',move.oldPos.rank()).pos][(kPieceRook - 1) + 6 * move.piece.player()];
        }
    }

    //If capturing, remove the captured piece
    if(move.isEnPassant()){
        ChessPos capturePos = move.newPos;
        if(move.piece.player() == kPlayerWhite) capturePos.pos -= 8;
        else capturePos.pos += 8;

        newKey ^= kZobristPieceNums[capturePos.pos][6*move.captured.player()];
    } else if (move.isCapturing()) newKey ^= kZobristPieceNums[move.newPos.pos][(move.captured.type() - 1) + 6 * move.captured.player()];

    //Remove the piece at oldPos and add the piece at newPos
    newKey ^= kZobristPieceNums[move.oldPos.pos][move.piece.type() - 1 + 6 * move.piece.player()];
    newKey ^= kZobristPieceNums[move.newPos.pos][move.piece.type() - 1 + 6 * move.piece.player()];

    //Generate the new data
    if(!move.isCapturing()) newData = this->movesSinceLastCapture() + 1;
    newData <<= 4;
    if(move.isEnPassantEligible()) newData |= (move.newPos.file() - 'a' + 1);
    newData <<= 4;
    if(move.isCastling()){
        if(move.isCastlingKingside()){
            if(move.piece.player() == kPlayerWhite) castlingInfo &= kBoardDataWhiteKingside;
            else castlingInfo &= kBoardDataBlackKingside;
        } else {
            if(move.piece.player() == kPlayerWhite) castlingInfo &= kBoardDataWhiteQueenside;
            else castlingInfo &= kBoardDataBlackQueenside;
        }
    }
    newData |= castlingInfo;

    newKey ^= kZobristEnPassantNums[((newKey >> 4) & 0b1111)];
    newKey ^= kZobristCastlingNums[newKey & 0b1111];
    newKey ^= kZobristBlackToMoveNum;

    this->boardHistory_.push_back(newKey);
    this->boardData_.push_back(newData);
}
void ChessBoard::resetKeys() {

    this->boardData_.clear();
    this->boardHistory_.clear();

    this->boardData_.push_back(0b1111);

    size_t key = 0;

    for(int posInList = 0; posInList < this->pieceNum_; posInList++){
        key ^= kZobristPieceNums[this->piecePositions_[posInList].pos][(this->pieces_[this->piecePositions_[posInList].pos].type() - 1) + 6 * this->pieces_[this->piecePositions_[posInList].pos].player()];
    }

    key ^= kZobristCastlingNums[this->boardData_.back() & 0b1111];

    this->boardHistory_.push_back(key);
}

int ChessBoard::turnNum() const {
    return (this->moves_.size() >> 1) + 1;
}
int ChessBoard::playerScore(Player player) const {
    int scores[2] = {0,0};
    for(int pos = 0; pos < 64; pos++) scores[this->pieces_[pos].player()] += kPieceValue[this->pieces_[pos].type()];

    return (player == kPlayerWhite ? scores[0] : scores[1]);
}

void ChessBoard::printBoard() const {
    using std::cout;
    using std::endl;

    int pos;
    cout << this->playerScore(kPlayerWhite) / 100 << " | " << this->playerScore(kPlayerBlack) / 100 << endl;

    for(int rank = 7; rank >= 0; rank--){
        cout << rank + 1 << ' ';
        for(int file = 0; file < 8; file++){
            pos = rank * 8 + file;
            if(this->piece(pos).data) cout << this->piece(pos).pieceChar();
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

    if(!this->moves_.empty()){
        if(this->lastMove().piece.player() == kPlayerWhite) cout << this->turnNum() << ": " << this->lastMove().str() << endl;
        else cout << this->turnNum()-1 << ": " << this->moves_[this->moves_.size()-2].str() << ' ' << this->moves_[this->moves_.size()-1].str() << endl;
    }

    //cout << endl;
}
void ChessBoard::printMoves() const {
    using std::cout;
    using std::endl;
    if(this->turnNum() == 1){
        cout << "1: " << endl;
        return;
    }
    for(int turn = 1; turn < this->turnNum(); turn++) cout << turn << ": " << this->moves_.at((turn-1) * 2).str() << ' ' << this->moves_.at(((turn-1) * 2) + 1).str() << endl;

    if(this->blackToMove_) cout << this->turnNum() << ": " << this->lastMove().str() << endl;
}

//WARNING: if it's an invalid position, it will break!
void ChessBoard::addPiece(ChessPos pos, ChessPiece piece) {

   // std::cout << "Adding piece " << piece.pieceChar() << " at " << pos.str() << std::endl;

    if(piece.type() == kPieceKing){
        if(piece.player() == kPlayerWhite) {
            if(this->whiteKingPos_.pos == -1) this->whiteKingPos_ = pos;
            else return;
        }
        else {
            if(this->blackKingPos_.pos == -1) this->blackKingPos_ = pos;
            else return;
        }

        //std::cout << this->whiteKingPos_.str() << '\t' << this->blackKingPos_.str() << std::endl;
    }

    this->pieces_[pos.pos] = piece;

    this->piecePositions_[this->pieceNum_] = pos;
    this->pieceMap_[pos.pos] = this->pieceNum_;
    this->pieceNum_++;
}
//WARNING: if it's an invalid position, it will break!
ChessPiece ChessBoard::removePiece(ChessPos pos) {

    //std::cout << "Removing piece at " << pos.str() << std::endl;

    ChessPiece removed = this->piece(pos);

    //std::cout << this->whiteKingPos_.str() << '\t' << this->blackKingPos_.str() << std::endl;
    if(this->whiteKingPos_ == pos) this->whiteKingPos_ = ChessPos();
    else if(this->blackKingPos_ == pos) this->blackKingPos_ = ChessPos();
    //std::cout << this->whiteKingPos_.str() << '\t' << this->blackKingPos_.str() << std::endl;

    this->pieces_[pos.pos] = ChessPiece();

    char pieceInd = this->pieceMap_[pos.pos];
    this->piecePositions_[pieceInd] = this->piecePositions_[this->pieceNum_-1];
    this->pieceMap_[this->piecePositions_[pieceInd].pos] = pieceInd;
    this->pieceNum_--;

    return removed;
}
//WARNING: if either of the positions are invalid, it will not work!
void ChessBoard::movePiece(ChessPos oldPos, ChessPos newPos) {
    //std::cout << "Moving piece at " << oldPos.str() << " to " << newPos.str() << std::endl;
    //std::cout << this->whiteKingPos_.str() << '\t' << this->blackKingPos_.str() << std::endl;

    if(oldPos == this->whiteKingPos_) this->whiteKingPos_ = newPos;
    else if (oldPos == this->blackKingPos_) this->blackKingPos_ = newPos;

    //std::cout << this->whiteKingPos_.str() << '\t' << this->blackKingPos_.str() << std::endl;

    this->pieces_[newPos.pos] = this->pieces_[oldPos.pos];
    this->pieces_[oldPos.pos] = ChessPiece();

    char pieceInd = this->pieceMap_[oldPos.pos];
    this->piecePositions_[pieceInd] = newPos;
    this->pieceMap_[newPos.pos] = pieceInd;
}

void ChessBoard::doMove(ChessMove move, bool update) {
    if(move.isCastling()){
        if(move.isCastlingKingside()) this->doMove(ChessMove(this->piece(ChessPos('h',move.oldPos.rank())),ChessPos('h',move.oldPos.rank()),ChessPos('f',move.oldPos.rank())), false);
        else this->doMove(ChessMove(this->piece(ChessPos('a',move.oldPos.rank())),ChessPos('a',move.oldPos.rank()),ChessPos('c',move.oldPos.rank())), false);
    }

    if(move.isEnPassant()){
        ChessPos removePos = move.newPos;
        if(move.piece.player() == kPlayerWhite) removePos.pos -= 8;
        else removePos.pos += 8;

        this->removePiece(removePos);
    } else if(move.isCapturing()) this->removePiece(move.newPos);

    this->movePiece(move.oldPos, move.newPos);

    if(move.isPromoting()) {
        this->pieces_[move.newPos.pos].data &= 0b11111000;
        this->pieces_[move.newPos.pos].data |= move.promotionType();
    } else {
        this->pieces_[move.newPos.pos].data |= 0b10000;
    }

    if(update){
        this->blackToMove_ = !this->blackToMove_;
        this->moves_.push_back(move);
        this->addNewKey(move);
    }


}
void ChessBoard::undoMove(ChessMove move, bool update) {
    if(move.isCastling()){
        if(move.isCastlingKingside()) this->undoMove(ChessMove(ChessPiece(kPieceRook, move.piece.player()),ChessPos('h',move.oldPos.rank()),ChessPos('f',move.oldPos.rank())), false);
        else this->undoMove(ChessMove(ChessPiece(kPieceRook, move.piece.player()),ChessPos('a',move.oldPos.rank()),ChessPos('c',move.oldPos.rank())), false);
    }

    this->removePiece(move.newPos);
    this->addPiece(move.oldPos, move.piece);

    if(move.isEnPassant()){
        ChessPos removePos = move.newPos;
        if(move.piece.player() == kPlayerWhite) removePos.pos -= 8;
        else removePos.pos += 8;

        this->addPiece(removePos, move.captured);
    } else if (move.isCapturing()) this->addPiece(move.newPos, move.captured);

    if(update){
        this->moves_.pop_back();
        this->boardHistory_.pop_back();
        this->boardData_.pop_back();
        this->blackToMove_ = !this->blackToMove_;
    }
}
void ChessBoard::undoLastMove() {
    this->undoMove(this->lastMove());
}