#include <iostream>
#include <unordered_set>

#include "board.h"

const array<PieceType, 4> kPossiblePromotions = {kPieceRook, kPieceBishop, kPieceKnight, kPieceQueen};

ChessBoard::ChessBoard(bool fill){

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

    this->updateScore();
}
ChessBoard::ChessBoard(const ChessBoard& oldBoard) {

    using std::pair;

    for(pair<ChessPos, ChessPiece> piece : oldBoard.pieces_) this->pieces_.insert(piece);
    for(ChessMove move : oldBoard.moves_) this->moves_.push_back(move);
    for(string str : oldBoard.moveStrings_) this->moveStrings_.push_back(str);
    this->score_ = oldBoard.score_;
    this->inCheck_ = oldBoard.inCheck_;
}

bool ChessBoard::operator==(const ChessBoard& other) const{
    if(this->pieces_.size() > other.pieces_.size()){
        for(auto it : this->pieces_){
            if(!other.pieces_.count(it.first) || !(other.pieces_.at(it.first) == it.second)) return false;
        }
    }
    return true;
}

int ChessBoard::currentTurnNum(){
    return (this->moves_.size() >> 1) + 1;
}
Player ChessBoard::currentPlayer(){
    return (this->moves_.size() & 1) ? kPlayerBlack : kPlayerWhite;
}

void ChessBoard::updateScore(){
    this->score_[kPlayerWhite] = 0;
    this->score_[kPlayerBlack] = 0;
    for(auto iter : this->pieces_){
        this->score_[iter.second.player] += kPieceValue[iter.second.pieceType];
    }
}

int ChessBoard::playerScore(Player player){
    return this->score_[player];
}

bool ChessBoard::inCheck(Player player){

    Player other = kPlayerBlack;
    if(player == kPlayerBlack) other = kPlayerWhite;
    else other = kPlayerBlack;

    for(auto it : this->pieces_){
        if(it.second.player == other){
            for(ChessMove move : this->getMovesForPiece(it.second, it.first)){
                if(this->pieces_.count(move.newPos) && this->pieces_[move.newPos].player == player && this->pieces_[move.newPos].pieceType == kPieceKing) return true;
            }
        }
    }

    return false;
}
bool ChessBoard::hasWon(Player player){
    if(!this->getMovesForPlayer(player).size()) return true;
    return false;
}

void ChessBoard::printBoard(){

    using std::cout;
    using std::endl;

    ChessPos pos;
    cout << "Turn " << this->currentTurnNum() << endl;
    cout << this->score_[kPlayerWhite] << " | " << this->score_[kPlayerBlack] << endl;
    if(this->inCheck(kPlayerWhite)) cout << "White in check!" << endl;
    else if(this->inCheck(kPlayerBlack)) cout << "Black in check!" << endl;

    for(int row = 8; row > 0; row--){ //Do 8..0 instead of 7..-1 because I need to print row
        pos.col = 'a';
        cout << row << ' ';
        for(int col = 'a'; col <= 'h'; col++){
            pos = ChessPos(col, row);
            if(this->pieces_.count(pos)){
                cout << this->pieces_[pos].pieceChar();
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
}

void ChessBoard::addPiece(ChessPos pos, ChessPiece piece){
    this->pieces_.insert(pair<ChessPos,ChessPiece>(pos,piece));
    this->updateScore();
}

bool ChessBoard::willMoveCapture(ChessMove move){
    return (this->pieces_[move.pos].player != this->pieces_[move.newPos].player);
}

void ChessBoard::addMovesInDir(vector<ChessMove> &moves, ChessPiece piece, ChessPos startPos, char dCol, char dRow){
    ChessPos newPos = ChessPos(startPos.col + dCol, startPos.row + dRow);

    while(newPos.isInBounds() && !this->pieces_.count(newPos)){
        moves.push_back(ChessMove(startPos, newPos, piece));
        newPos.row += dRow;
        newPos.col += dCol;
    }

    ChessMove newMove = ChessMove(startPos, newPos, piece);
    if(newPos.isInBounds() && willMoveCapture(newMove)) moves.push_back(newMove);
}

vector<ChessMove> ChessBoard::getMovesForPiece(ChessPiece piece, ChessPos pos){
    vector<ChessMove> moves;

    char dRow, dRow1, dCol, dCol1, tmp;

    ChessMove newMove;
    ChessPiece promotion = ChessPiece(piece);
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
                
                newMove = ChessMove(pos, ChessPos(pos.col + dCol, pos.row + dRow), promotion);

                if(newMove.isInBounds() && this->pieces_.count(newMove.newPos) && willMoveCapture(newMove)){
                    movesToAdd.push_back(newMove);
                }
            }

            newMove = ChessMove(pos, ChessPos(pos.col, pos.row + dRow), promotion);
            if(newMove.isInBounds() && !this->pieces_.count(newMove.newPos)) {
                movesToAdd.push_back(newMove);
                doCheck = true;
            }

            //Then, see if you can move two spaces
            if(doCheck && ((piece.player == kPlayerWhite && pos.row == 2) || (piece.player == kPlayerBlack && pos.row == 7))){
                newMove = ChessMove(pos, ChessPos(pos.col, pos.row + dRow*2), promotion);
                if(newMove.isInBounds() && !this->pieces_.count(newMove.newPos)) movesToAdd.push_back(newMove);
            }

            for(ChessMove move : movesToAdd){
                if((piece.player == kPlayerWhite && move.newPos.row == 8) || (piece.player == kPlayerBlack && move.newPos.row == 1)){
                    for(PieceType promotionType : kPossiblePromotions){
                        promotion.pieceType = promotionType;
                        moves.push_back(ChessMove(move.pos, move.newPos, promotion));
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
            newMove = ChessMove(pos, ChessPos(pos.col + dCol, pos.row + dRow), promotion);
            //std::cout << newMove.newPos.str() << std::endl;
            if(newMove.isInBounds() && (!this->pieces_.count(newMove.newPos) || willMoveCapture(newMove))) moves.push_back(newMove);
            newMove = ChessMove(pos, ChessPos(pos.col + dRow, pos.row + dCol), promotion);
            //std::cout << newMove.newPos.str() << std::endl;
            //if(this->pieces_.count(newMove.newPos)) std::cout << this->pieces_[newMove.newPos].pieceChar() <<std::endl;
            if(newMove.isInBounds() && (!this->pieces_.count(newMove.newPos) || willMoveCapture(newMove))) moves.push_back(newMove);
            
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
            newMove = ChessMove(pos, ChessPos(pos.col + dCol,pos.row + dRow), promotion);
            if(newMove.isInBounds() && (!this->pieces_.count(newMove.newPos) || willMoveCapture(newMove))) moves.push_back(newMove);
            newMove = ChessMove(pos, ChessPos(pos.col + dCol1,pos.row + dRow1), promotion);
            if(newMove.isInBounds() && (!this->pieces_.count(newMove.newPos) || willMoveCapture(newMove))) moves.push_back(newMove);
            
            tmp = dRow;
            dRow = -dCol;
            dCol = tmp;
            tmp = dRow1;
            dRow1 = -dCol1;
            dCol1 = tmp;
        }

        break;
    }

    return moves;
}

vector<ChessMove> ChessBoard::getMovesForPlayer(Player player){
    vector<ChessMove> moves, filteredMoves;

    for(auto pieceData : this->pieces_){
        if(pieceData.second.player == player){
            vector<ChessMove> movesToAdd = this->getMovesForPiece(pieceData.second, pieceData.first);
            moves.insert(moves.end(), movesToAdd.begin(), movesToAdd.end());
        }
    }

    for(ChessMove move : moves){
        ChessBoard newBoard = ChessBoard(*this);
        newBoard.doMove(move);
        if(!newBoard.inCheck(player)) filteredMoves.push_back(move);
    }

    return filteredMoves;
}

vector<ChessMove> ChessBoard::getPossibleMoves(){
    return this->getMovesForPlayer(this->currentPlayer());
}

//This already assumes that the move is valid, I don't think it will ever be the case that you'll call this function on an invalid move
void ChessBoard::doMove(ChessMove move){

    //We'll assume that the move has to make th
    //this->inCheck_[this->pieces_[move.pos].player] = false;

    //In the getMovesForPlayer function, it should already only return moves that don't put the current player in check
    this->pieces_[move.newPos] = move.promotion;
    this->pieces_.erase(move.pos);

    this->moves_.push_back(move);

    //See if this move puts any of the opponent's pieces in check
    // vector<ChessMove> moves = getMovesForPiece(this->pieces_[move.newPos],move.newPos);
    // for(ChessMove move : moves){
    //     if(this->pieces_.count(move.newPos) && this->pieces_[move.newPos].pieceType == kPieceKing && this->pieces_[move.newPos].player != this->pieces_[move.pos].player){
    //         this->inCheck_[this->pieces_[move.newPos].player] = true;
    //         break;
    //     }
    // }

    this->updateScore();

    //TODO: make function that can write move in chess notation
    //std::cout << move.pos.col << (int)move.pos.row << ' ' << move.newPos.col << (int)move.newPos.row << std::endl;
    
}

std::size_t ChessBoardHash::operator()(ChessBoard const& board) const {
    std::size_t seed = board.pieces_.size();
    std::size_t x;
    ChessPos pos;
    for(char col = 'a'; col <= 'h'; col++){
        for(char row = 1; row <= 8; row++){
            pos = ChessPos(col, row);
            if(board.pieces_.count(pos)){
                x = ((std::size_t)col << 16) | ((std::size_t)row << 8) | board.pieces_.at(pos).pieceType;
                x = ((x >> 16) ^ x) * 0x45d9f3b;
                x = ((x >> 16) ^ x) * 0x45d9f3b;
                x = (x >> 16) ^ x;
                seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
        }
    }
    return seed;
}
