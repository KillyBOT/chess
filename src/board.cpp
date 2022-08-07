#include <iostream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "board.h"

ChessBoard::ChessBoard(bool fill, bool recordMoves){

    using std::pair;

    // for(char col = 'a'; col <= 'h'; col++){
    //     for(char row = 1; row <= 8; row++){
    //         this->attacked_.emplace(ChessPos(col,row),unordered_set<ChessPos,ChessPosHash>());
    //     }
    // }

    if(fill){
        this->addPiece(ChessPos('a',1), ChessPiece(kPieceRook, kPlayerWhite));
        this->addPiece(ChessPos('b',1), ChessPiece(kPieceKnight, kPlayerWhite));
        this->addPiece(ChessPos('c',1), ChessPiece(kPieceBishop, kPlayerWhite));
        this->addPiece(ChessPos('d',1), ChessPiece(kPieceQueen, kPlayerWhite));
        this->addPiece(ChessPos('e',1), ChessPiece(kPieceKing, kPlayerWhite));
        this->addPiece(ChessPos('f',1), ChessPiece(kPieceBishop, kPlayerWhite));
        this->addPiece(ChessPos('g',1), ChessPiece(kPieceKnight, kPlayerWhite));
        this->addPiece(ChessPos('h',1), ChessPiece(kPieceRook, kPlayerWhite));

        this->addPiece(ChessPos('a',8), ChessPiece(kPieceRook, kPlayerBlack));
        this->addPiece(ChessPos('b',8), ChessPiece(kPieceKnight, kPlayerBlack));
        this->addPiece(ChessPos('c',8), ChessPiece(kPieceBishop, kPlayerBlack));
        this->addPiece(ChessPos('d',8), ChessPiece(kPieceQueen, kPlayerBlack));
        this->addPiece(ChessPos('e',8), ChessPiece(kPieceKing, kPlayerBlack));
        this->addPiece(ChessPos('f',8), ChessPiece(kPieceBishop, kPlayerBlack));
        this->addPiece(ChessPos('g',8), ChessPiece(kPieceKnight, kPlayerBlack));
        this->addPiece(ChessPos('h',8), ChessPiece(kPieceRook, kPlayerBlack));

        for(char c = 'a'; c <= 'h'; c++){ //Yoo like C++ that's so crazy bro
            this->addPiece(ChessPos(c,2), ChessPiece(kPiecePawn, kPlayerWhite));
            this->addPiece(ChessPos(c,7), ChessPiece(kPiecePawn, kPlayerBlack));
        }
    }

    this->updateHash();

}
ChessBoard::ChessBoard(const ChessBoard& oldBoard) {
    this->pieces_ = oldBoard.pieces_;
    this->moves_ = oldBoard.moves_;
    this->hash_ = oldBoard.hash_;
}

bool ChessBoard::operator==(const ChessBoard& other) const{
    return this->pieces_ == other.pieces_;
}

inline int ChessBoard::turnNum() const{
    return (this->moves_.size() >> 1) + 1;
}
inline Player ChessBoard::player() const {
    return (this->moves_.size() & 1) ? kPlayerBlack : kPlayerWhite;
}
inline Player ChessBoard::opponent() const {
    return(this->player() == kPlayerWhite) ? kPlayerBlack : kPlayerWhite;
}
inline const ChessMove &ChessBoard::lastMove() const {
    return this->moves_.back();
}
inline bool ChessBoard::hasPiece(ChessPos pos) const {
    return this->pieces_.count(pos);
}
inline const ChessPiece &ChessBoard::piece(ChessPos pos) const {
    return this->pieces_.at(pos);
}
inline const unordered_map<ChessPos, ChessPiece, ChessPosHash> &ChessBoard::pieces() const {
    return this->pieces_;
}
inline const vector<ChessMove> &ChessBoard::moves() const {
    return this->moves_;
}

int ChessBoard::playerScore(Player player) const{
    int whiteScore = 0;
    int blackScore = 0;
    for(auto iter : this->pieces_){
        if(iter.second.player == kPlayerWhite) whiteScore += kPieceValue[iter.second.pieceType];
        else blackScore += kPieceValue[iter.second.pieceType];
    }

    if(player == kPlayerWhite) return whiteScore;
    else return blackScore;
}
std::size_t ChessBoard::hash() const {
    return this->hash_;
}

void ChessBoard::updateHash(){
    this->hash_ = this->pieces_.size();
    std::size_t x;
    ChessPos pos;
    for(char col = 'a'; col <= 'h'; col++){
        for(char row = 1; row <= 8; row++){
            pos = ChessPos(col, row);
            if(this->pieces_.count(pos)){
                x = col;
                x <<= 8;
                x |= row;
                x <<= 8;
                x |= this->pieces_.at(pos).pieceType;
                x = ((x >> 16) ^ x) * 0x45d9f3b;
                x = ((x >> 16) ^ x) * 0x45d9f3b;
                x = (x >> 16) ^ x;
                this->hash_ ^= x + 0x9e3779b9 + (this->hash_ << 6) + (this->hash_ >> 2);
            }
        }
    }
}

void ChessBoard::printBoard() const{

    using std::cout;
    using std::endl;

    ChessPos pos = ChessPos('a',8);
    cout << this->playerScore(kPlayerWhite) << " | " << this->playerScore(kPlayerBlack) << endl;
    // if(this->hasWon(kPlayerWhite)) cout << "White has won!" << endl;
    // else if(this->hasWon(kPlayerBlack)) cout << "Black has won!" << endl;
    // else if(this->stalemate()) cout << "Stalemate!" << endl;
    // else if(this->inCheck(kPlayerWhite)) cout << "White in check!" << endl;
    // else if(this->inCheck(kPlayerBlack)) cout << "Black in check!" << endl;

    for(int row = 8; row > 0; row--){ //Do 8..0 instead of 7..-1 because I need to print row
        pos.col = 'a';
        cout << (int)pos.row << ' ';
        for(int col = 0; col < 8; col++){
            if(this->pieces_.count(pos)){
                cout << this->pieces_.at(pos).pieceChar();
            } else {
                cout << ((row + col) % 2 ? '#' : ' ');
            }
            pos.col++;
        }
        cout << endl;
        pos.row -= 1;
    }
    cout << "  ";
    for(char col = 'a'; col <= 'h'; col++){
        cout << col;
    }
    cout << endl;
    cout << (this->player() == kPlayerWhite ? "White" : "Black") << "\'s turn" << endl;
    cout << this->turnNum() << ": ";
    if(this->moves_.size()) cout << this->lastMove().str();
    cout << endl << endl;


    // for(ChessMove move : getPossibleMoves()){
    //     cout << move.basicStr() << endl;
    // }
}
/*void ChessBoard::printAttacked() const {
    using std::cout;
    using std::endl;

    ChessPos pos = ChessPos('a',8);
    char printChar;

    // for(auto iter : this->attacked_) {
    //     cout << iter.first.str() << ", ";
    // }
    // cout << endl;

    bool squareAttackedByWhite, squareAttackedByBlack;

    for(int row = 8; row > 0; row--){ //Do 8..0 instead of 7..-1 because I need to print row
        pos.col = 'a';
        cout << (int)pos.row << ' ';
        for(int col = 0; col < 8; col++){
            //cout << pos.str() << ' ' << this->attacked_.count(pos) << endl;
            squareAttackedByWhite = this->attackedByWhite_.count(pos);
            squareAttackedByBlack = this->attackedByBlack_.count(pos);

            if(squareAttackedByWhite && squareAttackedByBlack){
                printChar = 'X';
            } else if (squareAttackedByWhite) {
                printChar = 'W';
            } else if (squareAttackedByBlack) {
                printChar = 'B';
            } else {
                printChar = ((row + col) % 2 ? '#' : ' ');
            }
            cout << printChar;
            pos.col += 1;
        }
        cout << endl;
        pos.row -= 1;
    }
    cout << "  ";
    for(char col = 'a'; col <= 'h'; col++){
        cout << col;
    }
    cout << endl;
} */

/*void ChessBoard::printAttackedDict() const {
    using std::cout;
    using std::endl;
    for(auto iter : this->attacked_){
        if(this->hasThreats(iter.first)){
            cout << iter.first.str() << ": ";
            for(ChessPos attackedBy : iter.second) cout << attackedBy.str() << ", ";
            cout << std::endl;
        }
    }
}*/

void ChessBoard::printMoves() const {
    using std::cout;
    using std::endl;
    for(int turn = 1; turn < this->turnNum(); turn++) cout << turn << ": " << this->moves_.at((turn-1) * 2).str() << ' ' << this->moves_.at(((turn-1) * 2) + 1).str() << endl;

    cout << this->turnNum() << ": " << endl;
    if(this->lastMove().piece.player == kPlayerWhite) cout << this->lastMove().str() << endl;
}

void ChessBoard::addPiece(ChessPos pos, ChessPiece piece){
    //std::cout << "Adding piece " << piece.pieceChar() << " at location " << pos.str() << std::endl;
    this->pieces_.insert(pair<ChessPos,ChessPiece>(pos,piece));

    // unordered_set<ChessPos,ChessPosHash> attacks = this->attacked_.at(pos);

    // //std::cout << "Updating attacked table" << std::endl;
    // for(const ChessPos &threat : attacks){
    //     //std::cout << "Updating " << attackedBy.str_int() << std::endl;
    //     this->pieces_.at(pos).affecting.insert(threat);
    //     this->pieces_.at(threat).affectedBy.insert(pos);
    //     this->updatePieceAttacking(threat);
    //     //std::cout << "Finished updating " << attackedBy.str_int() << std::endl;
    // }
    // //std::cout << "Updating complete" << std::endl;
    // this->addPieceAttacking(pos);

    //this->printBoard();
    //this->printAttacked();
}
void ChessBoard::removePiece(ChessPos pos){
    //std::cout << "Removing piece at " << pos.str() << std::endl;
    //this->removePieceAttacking(pos);
    // for(const ChessPos &affected : this->pieces_.at(pos).affecting){
    //     this->updatePieceAttacking(affected);   
    // }
    this->pieces_.erase(pos);
}

//Both of these already assume that the muve is valid
void ChessBoard::doMove(ChessMove move, bool updateHash){

    //Actually do the move
    if(move.capture.pieceType != kPieceNone) this->removePiece(move.newPos);
    this->removePiece(move.pos);
    this->addPiece(move.newPos, move.piece);

    //Also move rook if there's castling
    if(move.isCastling){
        ChessMove rookMove;
        if(move.newPos.col == 'b') rookMove = ChessMove(this->pieces_[ChessPos('a',move.pos.row)],ChessPos('a',move.pos.row),ChessPos('c',move.pos.row));
        else rookMove = ChessMove(this->pieces_[ChessPos('h',move.pos.row)],ChessPos('h',move.pos.row),ChessPos('f',move.pos.row));

        this->doMove(rookMove, false);
        this->moves_.pop_back();
    }
    this->pieces_[move.newPos].moveNum++; //I set this later so that the previous check works
    this->moves_.push_back(move);

    // ChessPosSet attacking = this->getAttackedByPiece(move.newPos);
    // if(attacking.count(this->whiteKingPos_)) this->whiteInCheck_ = true;
    // else if(attacking.count(this->blackInCheck_)) this->blackInCheck_ = true;

    // ChessPosSet attacks = this->attacked_.at(move.pos);
    // for(const ChessPos &attackedBy : attacks){
    //     this->updatePieceAttacking(attackedBy);
    // }

    if(updateHash) this->updateHash();

}
void ChessBoard::undoMove(ChessMove move, bool updateHash){

    if(move.isPromoting){
        move.piece.pieceType = kPiecePawn;
    }

    //TODO: add check for castling
    if(move.isCastling){
        if(move.newPos.col == 'b') this->undoMove(ChessMove(this->pieces_.at(ChessPos('c',move.newPos.row)),ChessPos('c',move.newPos.row),ChessPos('a',move.newPos.row)), false);
        else this->undoMove(ChessMove(this->pieces_.at(ChessPos('c',move.newPos.row)),ChessPos('c',move.newPos.row),ChessPos('a',move.newPos.row)), false);
        this->moves_.push_back(move);
    }

    this->doMove(ChessMove(this->pieces_.at(move.newPos), move.newPos,move.pos), false);
    this->pieces_.at(move.pos).moveNum -= 2;

    if(move.capture.pieceType != kPieceNone){
        this->addPiece(move.newPos, move.capture);
    }

    this->moves_.pop_back();

    if(updateHash) this->updateHash();
}

std::size_t ChessBoardHash::operator()(ChessBoard const& board) const {
    return board.hash();
}
