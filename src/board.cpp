#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstring>
#include <cstdlib>

#include "board.h"

using std::vector;
using std::size_t;

size_t kZobristPieceNums[64][16];
size_t kZobristBlackToMoveNum;
size_t kZobristCastlingNums[16];
size_t kZobristEnPassantNums[9];

void init_zobrist_nums() {
    srand(43252003); //The first 8 digits of the number of possible rubik's cube configurations

    for(int pos = 0; pos < 64; pos++){
        for(int player = 0; player < 12; player++){
            kZobristPieceNums[pos][player] = 0;
            kZobristPieceNums[pos][player + kPieceKing + 1] = 0;
            for(int type = kPiecePawn; type <= kPieceKing; type++){
                kZobristPieceNums[pos][player+type] = rand();
            }
        }
    }

    kZobristBlackToMoveNum = rand();
    for(int i = 0; i < 16; i++) kZobristCastlingNums[i] = rand();
    for(int i = 0; i < 9; i++) kZobristEnPassantNums[i] = rand();
}

ChessBoard::ChessBoard(bool initBoard){

    using std::string;

    this->blackToMove_ = false;
    this->whiteKingPos_ = ChessPos();
    this->blackKingPos_ = ChessPos();

    memset(this->pieces_, 0, 64);
    for(int i = 0; i < 16; i++) this->pieceLists_[i] = ChessPieceList();

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
    memcpy(this->pieceLists_, board.pieceLists_, 16 * sizeof(ChessPieceList));

    this->boardData_ = board.boardData_;
    this->boardHistory_ = board.boardHistory_;
    this->moves_ = board.moves_;

    this->blackToMove_ = board.blackToMove_;
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
            newKey ^= kZobristPieceNums[ChessPos('h',move.oldPos.rank()).pos][kPieceRook | (move.piece.player() << 3)];
            newKey ^= kZobristPieceNums[ChessPos('f',move.oldPos.rank()).pos][kPieceRook | (move.piece.player() << 3)];
        } else {
            newKey ^= kZobristPieceNums[ChessPos('a',move.oldPos.rank()).pos][kPieceRook | (move.piece.player() << 3)];
            newKey ^= kZobristPieceNums[ChessPos('c',move.oldPos.rank()).pos][kPieceRook | (move.piece.player() << 3)];
        }
    }

    //If capturing, remove the captured piece
    if(move.isEnPassant()){
        ChessPos capturePos = move.newPos;
        if(move.piece.player() == kPlayerWhite) capturePos.pos -= 8;
        else capturePos.pos += 8;

        newKey ^= kZobristPieceNums[capturePos.pos][6*move.captured.player()];
    } else if (move.isCapturing()) newKey ^= kZobristPieceNums[move.newPos.pos][move.captured.data];

    //Remove the piece at oldPos and add the piece at newPos
    newKey ^= kZobristPieceNums[move.oldPos.pos][move.piece.data];
    newKey ^= kZobristPieceNums[move.newPos.pos][move.piece.data];

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

    for(int i = 0; i < 64; i++){
        key ^= kZobristPieceNums[i][this->piece(ChessPos(i)).data];
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
void ChessBoard::printPieces() const {
    using std::cout;
    using std::endl;

    cout << "White king: ";
    cout << this->whiteKingPos_.str() << endl;
    cout << "White pawns: ";
    this->pieceLists_[kPiecePawn].printList();
    cout << "White rooks: ";
    this->pieceLists_[kPieceRook].printList();
    cout << "White knights: ";
    this->pieceLists_[kPieceKnight].printList();
    cout << "White bishops: ";
    this->pieceLists_[kPieceBishop].printList();
    cout << "White queens: ";
    this->pieceLists_[kPieceQueen].printList();

    cout << "Black king: ";
    cout << this->blackKingPos_.str() << endl;
    cout << "Black pawns: ";
    this->pieceLists_[kPiecePawn + 0b1000].printList();
    cout << "Black rooks: ";
    this->pieceLists_[kPieceRook + 0b1000].printList();
    cout << "Black knights: ";
    this->pieceLists_[kPieceKnight + 0b1000].printList();
    cout << "Black bishops: ";
    this->pieceLists_[kPieceBishop + 0b1000].printList();
    cout << "Black queens: ";
    this->pieceLists_[kPieceQueen + 0b1000].printList();
    
}

//WARNING: if it's an invalid position, it will break!
void ChessBoard::addPiece(ChessPos pos, ChessPiece piece) {

   // std::cout << "Adding piece " << piece.pieceChar() << " at " << pos.str() << std::endl;

    if(piece.type() == kPieceKing){
        // std::cout << "Adding king:" << std::endl;
        // this->printBoard();
        if(piece.player() == kPlayerWhite) this->whiteKingPos_ = pos;
        else this->blackKingPos_ = pos;
        //std::cout << this->whiteKingPos_.str() << '\t' << this->blackKingPos_.str() << std::endl;
    } else {
        this->pieceLists_[piece.data].addPiece(pos);
    }

    this->pieces_[pos.pos] = piece;
}
//WARNING: if it's an invalid position, it will break!
ChessPiece ChessBoard::removePiece(ChessPos pos) {

    //std::cout << "Removing piece at " << pos.str() << std::endl;

    ChessPiece removed = this->piece(pos);

    //std::cout << this->whiteKingPos_.str() << '\t' << this->blackKingPos_.str() << std::endl;
    // if(pos == this->whiteKingPos_ || pos == this->blackKingPos_) {
    //     std::cout << "Removing king:" << std::endl;
    //     this->printBoard();
    // }
    if(this->whiteKingPos_ == pos) this->whiteKingPos_ = ChessPos();
    else if(this->blackKingPos_ == pos) this->blackKingPos_ = ChessPos();
    else this->pieceLists_[removed.data].removePiece(pos);
    
    //std::cout << this->whiteKingPos_.str() << '\t' << this->blackKingPos_.str() << std::endl;

    this->pieces_[pos.pos] = ChessPiece();

    return removed;
}
//WARNING: if either of the positions are invalid, it will not work!
void ChessBoard::movePiece(ChessPos oldPos, ChessPos newPos) {
    //std::cout << "Moving piece at " << oldPos.str() << " to " << newPos.str() << std::endl;
    //std::cout << this->whiteKingPos_.str() << '\t' << this->blackKingPos_.str() << std::endl;

    if(oldPos == this->whiteKingPos_) this->whiteKingPos_ = newPos;
    else if (oldPos == this->blackKingPos_) this->blackKingPos_ = newPos;
    else this->pieceLists_[this->piece(oldPos).data].movePiece(oldPos, newPos);

    //std::cout << this->whiteKingPos_.str() << '\t' << this->blackKingPos_.str() << std::endl;

    this->pieces_[newPos.pos] = this->pieces_[oldPos.pos];
    this->pieces_[oldPos.pos] = ChessPiece();
}

void ChessBoard::doMove(ChessMove move, bool update) {

    if(move.isCastling()){
        if(move.isCastlingKingside()) this->doMove(ChessMove(this->piece(ChessPos('h',move.oldPos.rank())),ChessPos('h',move.oldPos.rank()),ChessPos('f',move.oldPos.rank())), false);
        else this->doMove(ChessMove(this->piece(ChessPos('a',move.oldPos.rank())),ChessPos('a',move.oldPos.rank()),ChessPos('c',move.oldPos.rank())), false);
    }

    if(move.isEnPassant()){
        //this->printBoard();
        ChessPos removePos = move.newPos;
        if(move.piece.player() == kPlayerWhite) removePos.pos -= 8;
        else removePos.pos += 8;

        this->removePiece(removePos);
    } else if(move.isCapturing()) this->removePiece(move.newPos);

    this->movePiece(move.oldPos, move.newPos);

    if(move.isPromoting()) {
        this->pieces_[move.newPos.pos].data &= 0b11111000;
        this->pieces_[move.newPos.pos].data |= move.promotionType();
    }
    
    this->pieces_[move.newPos.pos].hasMoved = true;

    if(update){
        this->blackToMove_ = !this->blackToMove_;
        this->moves_.push_back(move);
        this->addNewKey(move);
    }

    //if(move.isEnPassant()) this->printBoard();


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