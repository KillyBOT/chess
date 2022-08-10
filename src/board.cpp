#include <iostream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cstdlib>

#include "board.h"

static const Byte whiteKingsideMask = 0b1110;
static const Byte blackKingsideMask = 0b1101;
static const Byte whiteQueensideMask = 0b1011;
static const Byte blackQueensideMask = 0b0111;

ChessBoard::ChessBoard(bool fill){

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

    //this->updateHash();
    this->resetZobrist();
    this->addBoardToSeen();
}
ChessBoard::ChessBoard(const ChessBoard& oldBoard) {
    this->pieces_ = oldBoard.pieces_;
    this->moves_ = oldBoard.moves_;
    this->seenBoards_ = oldBoard.seenBoards_;
    this->boardHashHistory_ = oldBoard.boardHashHistory_;
    this->gameData_ = oldBoard.gameData_;

    this->zobristBlackToMove_ = oldBoard.zobristBlackToMove_;
    this->zobristPieces_ = oldBoard.zobristPieces_;
    this->zobristCastlingRights_ = oldBoard.zobristCastlingRights_;
    this->zobristEnPassantFile_ = oldBoard.zobristEnPassantFile_;
}

bool ChessBoard::operator==(const ChessBoard& other) const{
    return this->zobristKey() == other.zobristKey();
}
bool ChessBoard::operator!=(const ChessBoard &other) const {
    return this->zobristKey() != other.zobristKey();
}


int ChessBoard::turnNum() const{
    return (this->moves_.size() >> 1) + 1;
}
Player ChessBoard::player() const {
    return (this->moves_.size() & 1) ? kPlayerBlack : kPlayerWhite;
}
Player ChessBoard::opponent() const {
    return(this->player() == kPlayerWhite) ? kPlayerBlack : kPlayerWhite;
}
ChessMove ChessBoard::lastMove() const {
    return this->moves_.back();
}
bool ChessBoard::hasPiece(ChessPos pos) const {
    return this->pieces_.count(pos);
}
const ChessPiece &ChessBoard::piece(ChessPos pos) const {
    return this->pieces_.at(pos);
}
const unordered_map<ChessPos, ChessPiece, ChessPosHash> &ChessBoard::pieces() const {
    return this->pieces_;
}
const vector<ChessMove> &ChessBoard::moves() const {
    return this->moves_;
}
const unordered_map<size_t, int> &ChessBoard::seenBoards() const {
    return this->seenBoards_;
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
const size_t &ChessBoard::zobristKey() const {
    return this->boardHashHistory_.back();
}
/*std::size_t ChessBoard::hash() const {
    return this->hash_;
}*/

/*void ChessBoard::updateHash(){
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
}*/
void ChessBoard::resetZobrist() {

    this->gameData_.push_back(0b1111);
    this->boardHashHistory_.push_back(0);

    srand(43252003); //The first 8 digits of God's number (the number of possible permutations of a rubik's cube)
    for(int i = 0; i < 64; i++){
        for(int j = 0; j < 12; j++){
            this->zobristPieces_[i][j] = rand();
        }
    }
    this->zobristBlackToMove_ = rand();
    
    for(int i = 0; i < 16; i++) this->zobristCastlingRights_[i] = rand();
    for(int i = 0; i < 8; i++) this->zobristEnPassantFile_[i] = rand();

    for(auto iter : this->pieces_){
        this->boardHashHistory_.back() ^= this->zobristPieces_[(iter.first.col - 'a') * 8 + (iter.first.row - 1)][iter.second.pieceType + iter.second.player * 6];
    }

    this->boardHashHistory_.back() ^= this->zobristCastlingRights_[this->gameData_.back() & 0b1111];
}
void ChessBoard::updateZobrist(ChessMove move) {
    Byte newGameData = this->gameData_.back();
    size_t newKey = this->boardHashHistory_.back();

    //Get rid of the piece at the old position
    if(move.isPromoting) newKey ^= this->zobristPieces_[(move.pos.col - 'a') * 8 + (move.pos.row - 1)][move.piece.player * 6];
    else newKey ^= this->zobristPieces_[(move.pos.col - 'a') * 8 + (move.pos.row - 1)][move.piece.pieceType + move.piece.player * 6];

    //If you're capturing or doing en passant, also get rid of the piece you're capturing
    if(move.isEnPassant){
        if(move.piece.player == kPlayerWhite){
            move.newPos.row--;
            newKey ^= this->zobristPieces_[(move.newPos.col - 'a') * 8 + (move.newPos.row - 1)][move.capture.player * 6]; //For both of these, it should always be a pawn, so I shouldn't have to worry
            move.newPos.row++;
        } else {
            move.newPos.row++;
            newKey ^= this->zobristPieces_[(move.newPos.col - 'a') * 8 + (move.newPos.row - 1)][move.capture.player * 6];
            move.newPos.row--;
        }
    }
    else if(move.capture.pieceType != kPieceNone) newKey ^= this->zobristPieces_[(move.newPos.col - 'a') * 8 + (move.newPos.row - 1)][move.capture.pieceType + move.capture.player * 6];
    
    //Add the piece that's being moved
    newKey ^= this->zobristPieces_[(move.newPos.col - 'a') * 8 + (move.newPos.row - 1)][move.piece.pieceType + move.piece.player * 6];

    //If you're castling, update the hash accordingly
    if(move.isCastling){
        newKey ^= this->zobristCastlingRights_[newGameData & 0b1111];
        if(move.piece.player == kPlayerWhite){
            if(move.castlingSide) newGameData &= whiteKingsideMask;
            else newGameData &= whiteQueensideMask;
        } else {
            if(move.castlingSide) newGameData &= blackKingsideMask;
            else newGameData &= blackQueensideMask;
        }
        newKey ^= this->zobristCastlingRights_[newGameData & 0b1111];
    }
    //Also update the en passant file
    if(this->gameData_.back() >> 7 || move.isEnPassantEligible){
        newKey ^= this->zobristEnPassantFile_[(newGameData >> 4) & 0b111];
        newGameData &= 0b1111;
    }
    if(move.isEnPassantEligible){
        newGameData |= 0b10000000;
        newGameData &= 0b10001111;
        newGameData |= (move.pos.col - 'a') << 4;
        newKey ^= this->zobristEnPassantFile_[(newGameData >> 4) & 0b111];
    }
    //Finally, XOR by the blackToMove, since you're switching to the other 
    newKey ^= this->zobristBlackToMove_;

    this->boardHashHistory_.push_back(newKey);
    this->gameData_.push_back(newGameData);
}
void ChessBoard::addBoardToSeen() {
    if(!this->seenBoards_.count(this->zobristKey())) this->seenBoards_.emplace(this->zobristKey(), 1);
    else this->seenBoards_.at(this->zobristKey())++;
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
    if(!this->moves_.empty()){
        if(this->lastMove().piece.player == kPlayerWhite) cout << this->turnNum() << ": " << this->lastMove().str() << endl;
        else cout << this->turnNum()-1 << ": " << this->moves_[this->moves_.size()-2].str() << ' ' << this->moves_[this->moves_.size()-1].str() << endl;
    }

    cout << endl;
}

void ChessBoard::printMoves() const {
    using std::cout;
    using std::endl;
    for(int turn = 1; turn < this->turnNum(); turn++) cout << turn << ": " << this->moves_.at((turn-1) * 2).str() << ' ' << this->moves_.at(((turn-1) * 2) + 1).str() << endl;

    if(this->lastMove().piece.player == kPlayerWhite) cout << this->turnNum() << ": " << this->lastMove().str() << endl;
}

void ChessBoard::addPiece(ChessPos pos, ChessPiece piece){
    //std::cout << "Adding piece " << piece.pieceChar() << " at location " << pos.str() << std::endl;
    this->pieces_.insert(pair<ChessPos,ChessPiece>(pos,piece));
}
void ChessBoard::removePiece(ChessPos pos){
    //std::cout << "Removing piece at " << pos.str() << std::endl;
    this->pieces_.erase(pos);
}

//Both of these already assume that the muve is valid
void ChessBoard::doMove(ChessMove move, bool updateHash){

    //std::cout << this->zobristKey_ << std::endl;
    //this->printBoard();

    //Actually do the move
    if(move.isEnPassant){
        if(move.piece.player == kPlayerWhite){
            move.newPos.row--;
            this->removePiece(move.newPos);
            move.newPos.row++;
        } else {
            move.newPos.row++;
            this->removePiece(move.newPos);
            move.newPos.row--;
        }
    }
    else if(move.capture.pieceType != kPieceNone) this->removePiece(move.newPos);
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

    // ChessPosSet attacking = this->getAttackedByPiece(move.newPos);
    // if(attacking.count(this->whiteKingPos_)) this->whiteInCheck_ = true;
    // else if(attacking.count(this->blackInCheck_)) this->blackInCheck_ = true;

    // ChessPosSet attacks = this->attacked_.at(move.pos);
    // for(const ChessPos &attackedBy : attacks){
    //     this->updatePieceAttacking(attackedBy);
    // }
    if(updateHash) {
        this->moves_.push_back(move);
        this->updateZobrist(move);
        this->addBoardToSeen();
    }
    //for(size_t key : this->boardHashHistory_) std::cout << "test " << key << std::endl;
    //std::cout << std::endl;
    //std::cout << this->zobristKey_ << std::endl;
    //this->printBoard();

}
void ChessBoard::undoMove(ChessMove move, bool updateHash){

    if(move.isPromoting){
        move.piece.pieceType = kPiecePawn;
    }

    if(move.isCastling){
        if(move.newPos.col == 'b') this->undoMove(ChessMove(this->pieces_.at(ChessPos('c',move.newPos.row)),ChessPos('c',move.newPos.row),ChessPos('a',move.newPos.row)), false);
        else this->undoMove(ChessMove(this->pieces_.at(ChessPos('c',move.newPos.row)),ChessPos('c',move.newPos.row),ChessPos('a',move.newPos.row)), false);
    }

    if(move.isEnPassant){
        if(move.piece.player == kPlayerWhite){
            move.newPos.row++;
            this->addPiece(move.newPos, move.capture);
            move.newPos.row--;
        } else {
            move.newPos.row--;
            this->addPiece(move.newPos, move.capture);
            move.newPos.row++;
        }
        this->removePiece(move.newPos);
        move.piece.moveNum--;
        this->addPiece(move.pos,move.piece);
        
    }
    else {
        this->doMove(ChessMove(this->pieces_.at(move.newPos), move.newPos,move.pos), false);
        this->pieces_.at(move.pos).moveNum -= 2;

        if(move.capture.pieceType != kPieceNone){
            this->addPiece(move.newPos, move.capture);
        }
    }

    if(updateHash) {
        this->seenBoards_.at(this->zobristKey())--;
        this->moves_.pop_back();
        this->gameData_.pop_back();
        this->boardHashHistory_.pop_back();

    }
}
void ChessBoard::undoLastMove(bool updateHash){
    this->undoMove(this->moves_.back(), updateHash);
}

std::size_t ChessBoardHash::operator()(ChessBoard const& board) const {
    //return board.hash();
    return board.zobristKey();
}
