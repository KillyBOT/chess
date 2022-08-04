#include <iostream>
#include <unordered_set>

#include "board.h"

const array<PieceType, 4> kPossiblePromotions = {kPieceRook, kPieceBishop, kPieceKnight, kPieceQueen};

ChessBoard::ChessBoard(bool fill, bool recordMoves){

    using std::pair;

    if(fill){
        this->pieces_.insert(pair<ChessPos,ChessPiece>(ChessPos('a',1), ChessPiece(kPieceRook, kPlayerWhite)));
        this->pieces_.insert(pair<ChessPos,ChessPiece>(ChessPos('b',1), ChessPiece(kPieceKnight, kPlayerWhite)));
        this->pieces_.insert(pair<ChessPos,ChessPiece>(ChessPos('c',1), ChessPiece(kPieceBishop, kPlayerWhite)));
        this->pieces_.insert(pair<ChessPos,ChessPiece>(ChessPos('d',1), ChessPiece(kPieceQueen, kPlayerWhite)));
        this->pieces_.insert(pair<ChessPos,ChessPiece>(ChessPos('e',1), ChessPiece(kPieceKing, kPlayerWhite)));
        this->pieces_.insert(pair<ChessPos,ChessPiece>(ChessPos('f',1), ChessPiece(kPieceBishop, kPlayerWhite)));
        this->pieces_.insert(pair<ChessPos,ChessPiece>(ChessPos('g',1), ChessPiece(kPieceKnight, kPlayerWhite)));
        this->pieces_.insert(pair<ChessPos,ChessPiece>(ChessPos('h',1), ChessPiece(kPieceRook, kPlayerWhite)));

        this->pieces_.insert(pair<ChessPos,ChessPiece>(ChessPos('a',8), ChessPiece(kPieceRook, kPlayerBlack)));
        this->pieces_.insert(pair<ChessPos,ChessPiece>(ChessPos('b',8), ChessPiece(kPieceKnight, kPlayerBlack)));
        this->pieces_.insert(pair<ChessPos,ChessPiece>(ChessPos('c',8), ChessPiece(kPieceBishop, kPlayerBlack)));
        this->pieces_.insert(pair<ChessPos,ChessPiece>(ChessPos('d',8), ChessPiece(kPieceQueen, kPlayerBlack)));
        this->pieces_.insert(pair<ChessPos,ChessPiece>(ChessPos('e',8), ChessPiece(kPieceKing, kPlayerBlack)));
        this->pieces_.insert(pair<ChessPos,ChessPiece>(ChessPos('f',8), ChessPiece(kPieceBishop, kPlayerBlack)));
        this->pieces_.insert(pair<ChessPos,ChessPiece>(ChessPos('g',8), ChessPiece(kPieceKnight, kPlayerBlack)));
        this->pieces_.insert(pair<ChessPos,ChessPiece>(ChessPos('h',8), ChessPiece(kPieceRook, kPlayerBlack)));

        for(char c = 'a'; c <= 'h'; c++){ //Yoo like C++ that's so crazy bro
            this->pieces_.insert(pair<ChessPos,ChessPiece>(ChessPos(c,2), ChessPiece(kPiecePawn, kPlayerWhite)));
            this->pieces_.insert(pair<ChessPos,ChessPiece>(ChessPos(c,7), ChessPiece(kPiecePawn, kPlayerBlack)));
        }
    }

    this->recordMoves_ = recordMoves;
    this->movesSinceLastCapture_ = 0;
    this->moveNum_ = 0;

    this->updateScore();
    this->updateHash();
    this->updateState();
}
ChessBoard::ChessBoard(const ChessBoard& oldBoard) {

    using std::pair;

    for(pair<ChessPos, ChessPiece> piece : oldBoard.pieces_) this->pieces_.insert(piece);

    this->recordMoves_ = oldBoard.recordMoves_;
    if(this->recordMoves_){
        for(ChessMove move : oldBoard.moves_) this->moves_.push_back(move);
        for(string str : oldBoard.moveStrings_) this->moveStrings_.push_back(str);
    }
    this->moveNum_ = oldBoard.moveNum_;
    this->score_ = oldBoard.score_;
    this->movesSinceLastCapture_ = oldBoard.movesSinceLastCapture_;
    this->hash_ = oldBoard.hash_;
    this->state_ = oldBoard.state_;
    this->lastMove_ = oldBoard.lastMove_;
}

bool ChessBoard::operator==(const ChessBoard& other) const{
    if(this->pieces_.size() > other.pieces_.size()){
        for(auto it : this->pieces_){
            if(!other.hasPieceAtPos(it.first) || !(other.pieces_.at(it.first) == it.second)) return false;
        }
    } else {
        for(auto it : other.pieces_){
            if(!this->hasPieceAtPos(it.first) || !(this->pieces_.at(it.first) == it.second)) return false;
        }
    }
    return true;
}

int ChessBoard::currentTurnNum() const{
    return (this->moveNum_ >> 1) + 1;
}
Player ChessBoard::currentPlayer() const{
    return (this->moveNum_ & 1) ? kPlayerBlack : kPlayerWhite;
}

ChessMove ChessBoard::lastMove() const {
    return this->lastMove_;
}

bool ChessBoard::hasPieceAtPos(ChessPos pos) const{
    return this->pieces_.count(pos);
}

void ChessBoard::updateScore(){
    this->score_[kPlayerWhite] = 0;
    this->score_[kPlayerBlack] = 0;
    for(auto iter : this->pieces_){
        this->score_[iter.second.player] += kPieceValue[iter.second.pieceType];
    }
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
                x |= this->pieces_[pos].pieceType;
                x = ((x >> 16) ^ x) * 0x45d9f3b;
                x = ((x >> 16) ^ x) * 0x45d9f3b;
                x = (x >> 16) ^ x;
                this->hash_ ^= x + 0x9e3779b9 + (this->hash_ << 6) + (this->hash_ >> 2);
            }
        }
    }
}

void ChessBoard::updateState(){
    if(this->hasWon(kPlayerWhite)) this->state_ = kStateWhiteWin;
    else if(this->hasWon(kPlayerBlack)) this->state_ = kStateBlackWin;
    else if(this->inCheck(kPlayerWhite)) this->state_ = kStateWhiteInCheck;
    else if(this->inCheck(kPlayerBlack)) this->state_ = kStateBlackInCheck;
    else if(this->stalemate()) this->state_ = kStateStalemate;
    else this->state_ = kStateNone;
}

int ChessBoard::playerScore(Player player) const{
    return this->score_[player];
}

bool ChessBoard::inCheck(Player player) const{

    Player other = kPlayerBlack;
    if(player == kPlayerBlack) other = kPlayerWhite;
    else other = kPlayerBlack;

    for(auto it : this->pieces_){
        if(it.second.player == other){
            for(ChessMove move : this->getMovesForPiece(it.second, it.first)){
                if(hasPieceAtPos(move.newPos) && this->pieces_.at(move.newPos).player == player && this->pieces_.at(move.newPos).pieceType == kPieceKing) return true;
            }
        }
    }

    return false;

}
bool ChessBoard::hasWon(Player player) const{
    Player other = (player == kPlayerWhite) ? kPlayerBlack : kPlayerWhite;
    if(!this->getMovesForPlayer(other).size()) return true;
    return false;
}
bool ChessBoard::stalemate() const{
    //TODO: add more situations
    return this->movesSinceLastCapture_ >= 100;
}
std::size_t ChessBoard::hash() const {
    return this->hash_;
}
BoardState ChessBoard::state() const {
    return this->state_;
}

void ChessBoard::printBoard() const{

    using std::cout;
    using std::endl;

    ChessPos pos;
    cout << "Turn " << this->currentTurnNum() << endl;
    cout << this->score_[kPlayerWhite] << " | " << this->score_[kPlayerBlack] << endl;
    if(this->state_ == kStateWhiteWin) cout << "White has won!" << endl;
    else if(this->state_ == kStateBlackWin) cout << "Black has won!" << endl;
    else if(this->state_ == kStateStalemate) cout << "Stalemate!" << endl;
    else if(this->state_ == kStateWhiteInCheck) cout << "White in check!" << endl;
    else if(this->state_ == kStateBlackInCheck) cout << "Black in check!" << endl;

    for(int row = 8; row > 0; row--){ //Do 8..0 instead of 7..-1 because I need to print row
        pos.col = 'a';
        cout << row << ' ';
        for(int col = 'a'; col <= 'h'; col++){
            pos = ChessPos(col, row);
            if(this->pieces_.count(pos)){
                cout << this->pieces_.at(pos).pieceChar();
            } else {
                cout << ((row + col) % 2 ? '#' : ' ');
            }
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
    cout << (this->currentPlayer() == kPlayerWhite ? "White" : "Black") << "\'s turn" << endl;
    cout << endl;

    // for(ChessMove move : getPossibleMoves()){
    //     cout << move.basicStr() << endl;
    // }
}

void ChessBoard::addPiece(ChessPos pos, ChessPiece piece){
    this->pieces_.insert(pair<ChessPos,ChessPiece>(pos,piece));
    this->updateScore();
}

bool ChessBoard::willMoveCapture(ChessMove &move) const{
    if(hasPieceAtPos(move.newPos) && this->pieces_.at(move.pos).player != this->pieces_.at(move.newPos).player){
        move.capture = this->pieces_.at(move.newPos);
        return true;
    }
    return false;
}

void ChessBoard::addPosInDir(vector<ChessPos> &positions, ChessPos pos, char dCol, char dRow) const {
    pos.col += dCol;
    pos.row += dRow;

    while(pos.isInBounds() && !hasPieceAtPos(pos)){
        positions.push_back(pos);
        pos.col += dCol;
        pos.row += dRow;
    }

    if(pos.isInBounds())  positions.push_back(pos);
}


void ChessBoard::addMovesInDir(vector<ChessMove> &moves, ChessPiece piece, ChessPos startPos, char dCol, char dRow) const{
    ChessMove newMove = ChessMove(piece, startPos, ChessPos(startPos.col + dCol, startPos.row + dRow));

    while(newMove.isInBounds() && !hasPieceAtPos(newMove.newPos)){
        moves.push_back(newMove);
        newMove.newPos.row += dRow;
        newMove.newPos.col += dCol;
    }

    if(newMove.isInBounds() && willMoveCapture(newMove)) moves.push_back(newMove);
}

vector<ChessPos> ChessBoard::getThreatenedByPiece(ChessPos pos) const{
    vector<ChessPos> positions;

    if(!this->pieces_.count(pos)){
        return positions;
    }
    
    ChessPiece piece = this->pieces_.at(pos);

    char dRow, dRow1, dCol, dCol1, tmp;

    ChessMove newMove;
    bool doCheck = false;
    ChessPos newPos;

    switch(piece.pieceType){
        //Turns out that finding out what moves a pawn can do is really damn annoying
        case kPiecePawn:
        default:

            dRow = 1;
            if(piece.player == kPlayerBlack) dRow = -1; //If black, go other direction

            //First, check if you can capture anything
            //TODO: check for en passant (holy hell)

            newPos = ChessPos(pos.col - 1, pos.row + dRow);
            if(newPos.isInBounds()) positions.push_back(newPos);
            newPos.col += 2;
            if(newPos.isInBounds()) positions.push_back(newPos);
            
        break;

        //The rook, bishop, and queen all use basically the same algorithm found in addMovesInDir

        case kPieceRook:

        dRow = 1;
        dCol = 0;

        for(int x = 0; x < 4; x++){
            addPosInDir(positions, pos, dCol, dRow);
            
            tmp = dRow;
            dRow = -dCol;
            dCol = tmp;
        }

        break;

        case kPieceBishop:
        dRow = 1;
        dCol = 1;
        for(int x = 0; x < 4; x++){
            addPosInDir(positions, pos, dCol, dRow);
            
            tmp = dRow;
            dRow = -dCol;
            dCol = tmp;
        }
        break;

        case kPieceQueen:

        dRow = 1;
        dCol = 1;
        dRow1 = 1;
        dCol1 = 0;

        for(int x = 0; x < 4; x++){
            addPosInDir(positions, pos, dCol, dRow);
            addPosInDir(positions, pos, dCol1, dRow1);
            
            tmp = dRow;
            dRow = -dCol;
            dCol = tmp;

            tmp = dRow1;
            dRow1 = -dCol1;
            dCol1 = tmp;
        }

        break;

        //Knight and king are similar to addMovesInDir, but instead of looping we just do it once
        case kPieceKnight:
        dRow = 1;
        dCol = 2;

        for(int x = 0; x < 4; x++){
            newPos = ChessPos(pos.col + dCol, pos.row + dRow);
            //std::cout << newMove.newPos.str() << std::endl;
            if(newPos.isInBounds()) positions.push_back(newPos);
            newPos = ChessPos(pos.col + dRow, pos.row + dCol);
            //std::cout << newMove.newPos.str() << std::endl;
            //if(this->pieces_.count(newMove.newPos)) std::cout << this->pieces_[newMove.newPos].pieceChar() <<std::endl;
            if(newPos.isInBounds()) positions.push_back(newPos);
            
            tmp = dRow;
            dRow = -dCol;
            dCol = tmp;
        }

        break;

        case kPieceKing:

        dRow = 1;
        dCol = 1;
        dRow1 = 1;
        dCol1 = 0;

        for(int x = 0; x < 4; x++){
            newPos = ChessPos(pos.col + dCol,pos.row + dRow);
            if(newPos.isInBounds()) positions.push_back(newPos);
            newPos = ChessPos(pos.col + dCol1,pos.row + dRow1);
            if(newPos.isInBounds()) positions.push_back(newPos);
            
            tmp = dRow;
            dRow = -dCol;
            dCol = tmp;
            tmp = dRow1;
            dRow1 = -dCol1;
            dCol1 = tmp;
        }

        break;
    }

    return positions;
}

vector<ChessMove> ChessBoard::getMovesForPiece(ChessPiece piece, ChessPos pos) const{
    vector<ChessMove> moves;

    char dRow, dRow1, dCol, dCol1, tmp;

    ChessMove newMove;
    bool doCheck = false;

    vector<ChessMove> movesToAdd; //Goddamn pawns making us have to declare a whole new vector

    switch(piece.pieceType){
        //Turns out that finding out what moves a pawn can do is really damn annoying
        case kPiecePawn:
        default:

            dRow = 1;
            if(piece.player == kPlayerBlack) dRow = -1; //If black, go other direction

            //First, check if you can capture anything
            //TODO: check for en passant (holy hell)

            for(dCol = - 1; dCol < 2; dCol += 2){
                
                newMove = ChessMove(piece, pos, ChessPos(pos.col + dCol, pos.row + dRow));

                if(newMove.isInBounds() && willMoveCapture(newMove)){
                    movesToAdd.push_back(newMove);
                }
            }

            newMove = ChessMove(piece, pos, ChessPos(pos.col, pos.row + dRow));
            if(newMove.isInBounds() && !hasPieceAtPos(newMove.newPos)) {
                movesToAdd.push_back(newMove);
                doCheck = true;
            }

            //Then, see if you can move two spaces
            if(doCheck && !piece.hasMoved){
                newMove = ChessMove(piece, pos, ChessPos(pos.col, pos.row + dRow*2));
                if(newMove.isInBounds() && !hasPieceAtPos(newMove.newPos)) movesToAdd.push_back(newMove);
            }

            for(ChessMove move : movesToAdd){
                if((piece.player == kPlayerWhite && move.newPos.row == 8) || (piece.player == kPlayerBlack && move.newPos.row == 1)){
                    for(PieceType promotionType : kPossiblePromotions){
                        piece.pieceType = promotionType;
                        moves.push_back(ChessMove(piece, move.pos, move.newPos));
                    }

                } else {
                    moves.push_back(move);
                }
            }
            
        break;

        //The rook, bishop, and queen all use basically the same algorithm found in addMovesInDir

        case kPieceRook:

        dRow = 1;
        dCol = 0;

        for(int x = 0; x < 4; x++){
            addMovesInDir(moves, piece, pos, dCol, dRow);
            
            tmp = dRow;
            dRow = -dCol;
            dCol = tmp;
        }

        break;

        case kPieceBishop:
        dRow = 1;
        dCol = 1;
        for(int x = 0; x < 4; x++){
            addMovesInDir(moves, piece, pos, dCol, dRow);
            
            tmp = dRow;
            dRow = -dCol;
            dCol = tmp;
        }
        break;

        case kPieceQueen:

        dRow = 1;
        dCol = 1;
        dRow1 = 1;
        dCol1 = 0;

        for(int x = 0; x < 4; x++){
            addMovesInDir(moves, piece, pos, dCol, dRow);
            addMovesInDir(moves, piece, pos, dCol1, dRow1);
            
            tmp = dRow;
            dRow = -dCol;
            dCol = tmp;

            tmp = dRow1;
            dRow1 = -dCol1;
            dCol1 = tmp;
        }

        break;

        //Knight and king are similar to addMovesInDir, but instead of looping we just do it once
        case kPieceKnight:
        dRow = 1;
        dCol = 2;

        for(int x = 0; x < 4; x++){
            newMove = ChessMove(piece, pos, ChessPos(pos.col + dCol, pos.row + dRow));
            //std::cout << newMove.newPos.str() << std::endl;
            if(newMove.isInBounds() && (!hasPieceAtPos(newMove.newPos) || willMoveCapture(newMove))) moves.push_back(newMove);
            newMove = ChessMove(piece, pos, ChessPos(pos.col + dRow, pos.row + dCol));
            //std::cout << newMove.newPos.str() << std::endl;
            //if(this->pieces_.count(newMove.newPos)) std::cout << this->pieces_[newMove.newPos].pieceChar() <<std::endl;
            if(newMove.isInBounds() && (!hasPieceAtPos(newMove.newPos) || willMoveCapture(newMove))) moves.push_back(newMove);
            
            tmp = dRow;
            dRow = -dCol;
            dCol = tmp;
        }

        break;

        case kPieceKing:

        dRow = 1;
        dCol = 1;
        dRow1 = 1;
        dCol1 = 0;

        for(int x = 0; x < 4; x++){
            newMove = ChessMove(piece, pos, ChessPos(pos.col + dCol,pos.row + dRow));
            if(newMove.isInBounds() && (!hasPieceAtPos(newMove.newPos) || willMoveCapture(newMove))) moves.push_back(newMove);
            newMove = ChessMove(piece, pos, ChessPos(pos.col + dCol1,pos.row + dRow1));
            if(newMove.isInBounds() && (!hasPieceAtPos(newMove.newPos) || willMoveCapture(newMove))) moves.push_back(newMove);
            
            tmp = dRow;
            dRow = -dCol;
            dCol = tmp;
            tmp = dRow1;
            dRow1 = -dCol1;
            dCol1 = tmp;
        }

        //Check if you can castle
        if(!piece.hasMoved){
            char row = (piece.player == kPlayerWhite) ? 1 : 8;

            //Queenside
            if(
                hasPieceAtPos(ChessPos('a',row)) 
                && !this->pieces_.at(ChessPos('a',row)).hasMoved 
                && !hasPieceAtPos(ChessPos('b',row)) 
                && !hasPieceAtPos(ChessPos('c',row)) 
                && !hasPieceAtPos(ChessPos('d',row))
            ) moves.push_back(ChessMove(piece, pos, ChessPos('b',row)));
            //Kingside
            if(
                hasPieceAtPos(ChessPos('h',row)) 
                && !this->pieces_.at(ChessPos('h',row)).hasMoved 
                && !hasPieceAtPos(ChessPos('g',row)) 
                && !hasPieceAtPos(ChessPos('f',row)) 
            ) moves.push_back(ChessMove(piece, pos, ChessPos('g',row)));
        }

        break;
    }

    return moves;
}

// void ChessBoard::resetThreatened() {
//     this->threatened_.empty();
//     for(auto pieceData : this->pieces_){
//         addPieceThreatening(pieceData.first);
//     }
// }

// void ChessBoard::addPieceThreatening(ChessPos pos) {
//     vector<ChessPos> threatened = getThreatenedByPiece(pos);

//     for(ChessPos threatenedPos : threatened){
//         if(!this->threatened_.count(threatenedPos)){
//             this->threatened_.insert(pair<ChessPos,vector<ChessPos>>(threatenedPos,vector<ChessPos>()));
//         }

//     }
// }

vector<ChessMove> ChessBoard::getMovesForPlayer(Player player) const{
    vector<ChessMove> moves, filteredMoves;

    for(auto pieceData : this->pieces_){
        if(pieceData.second.player == player){
            vector<ChessMove> movesToAdd = this->getMovesForPiece(pieceData.second, pieceData.first);
            moves.insert(moves.end(), movesToAdd.begin(), movesToAdd.end());
        }
    }

    for(ChessMove move : moves){
        ChessBoard newBoard = ChessBoard(*this);
        newBoard.movePiece(move);
        if(!newBoard.inCheck(player)) filteredMoves.push_back(move);
    }

    return filteredMoves;
}

vector<ChessMove> ChessBoard::getPossibleMoves() const{
    return this->getMovesForPlayer(this->currentPlayer());
}

void ChessBoard::movePiece(ChessMove move){
    //Check if the move resets movesSinceLastCapture
    if(hasPieceAtPos(move.newPos)){
        this->movesSinceLastCapture_ = 0;
    } else {
        this->movesSinceLastCapture_++;
    }

    this->pieces_[move.newPos] = move.piece;
    this->pieces_.erase(move.pos);

    //Also move rook if there's castling
    if(!this->pieces_[move.newPos].hasMoved && this->pieces_[move.newPos].pieceType == kPieceKing && (move.newPos.col == 'b' || move.newPos.col == 'g')){
        ChessMove rookMove;
        if(move.newPos.col == 'b') rookMove = ChessMove(this->pieces_[ChessPos('a',move.pos.row)],ChessPos('a',move.pos.row),ChessPos('c',move.pos.row));
        else rookMove = rookMove = ChessMove(this->pieces_[ChessPos('h',move.pos.row)],ChessPos('h',move.pos.row),ChessPos('f',move.pos.row));

        this->pieces_[rookMove.newPos] = rookMove.piece;
        this->pieces_.erase(rookMove.pos);
        this->pieces_[rookMove.newPos].hasMoved = true;
    }
    this->pieces_[move.newPos].hasMoved = true; //I set this later so that the previous check works

    this->moveNum_++;
    if(this->recordMoves_){
        this->moves_.push_back(move);
    }

    this->lastMove_ = move;
}

//This already assumes that the move is valid, I don't think it will ever be the case that you'll call this function on an invalid move
void ChessBoard::doMove(ChessMove move){

    //We'll assume that the move has to make th
    //this->inCheck_[this->pieces_[move.pos].player] = false;

    //In the getMovesForPlayer function, it should already only return moves that don't put the current player in check

    //Separate this so that we don't get stuck in endless loops when doing updates
    movePiece(move);

    //See if this move puts any of the opponent's pieces in check
    // vector<ChessMove> moves = getMovesForPiece(this->pieces_[move.newPos],move.newPos);
    // for(ChessMove move : moves){
    //     if(this->pieces_.count(move.newPos) && this->pieces_[move.newPos].pieceType == kPieceKing && this->pieces_[move.newPos].player != this->pieces_[move.pos].player){
    //         this->inCheck_[this->pieces_[move.newPos].player] = true;
    //         break;
    //     }
    // }

    this->updateScore();
    this->updateHash();
    this->updateState();

    //TODO: make function that can write move in chess notation
    //std::cout << move.pos.col << (int)move.pos.row << ' ' << move.newPos.col << (int)move.newPos.row << std::endl;
    
}

std::size_t ChessBoardHash::operator()(ChessBoard const& board) const {
    return board.hash();
}
