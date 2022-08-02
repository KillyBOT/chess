#include <iostream>

#include "board.h"

ChessBoard::ChessBoard(){

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
ChessBoard::ChessBoard(const ChessBoard& oldBoard){
    for (auto piece : oldBoard.pieces_){
        this->pieces_.insert(piece);
    }

    for(ChessMove move: oldBoard.moves_){
        this->moves_.push_back(move);
    }
    
    for(std::string str: oldBoard.moveStrings_){
        this->moveStrings_.push_back(str);
    }
}

int ChessBoard::currentTurnNum(){
    return (int)((this->moves_.size() / 2) + 1);
}
Player ChessBoard::currentPlayer(){
    return (this->moves_.size() % 2) ? kPlayerBlack : kPlayerWhite;
}

void ChessBoard::printBoard(){

    using std::cout;
    using std::endl;

    ChessPos pos;

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
}

bool ChessBoard::willMoveCapture(ChessMove move){
    return (this->pieces_[move.pos].player != this->pieces_[move.newPos].player);
}

void ChessBoard::addMovesInDir(vector<ChessMove> &moves, ChessPos startPos, char dCol, char dRow){
    ChessPos newPos = ChessPos(startPos.col, startPos.row);

    while(newPos.isInBounds() && !this->pieces_.count(newPos)){
        newPos.row += dRow;
        newPos.col += dCol;
        moves.push_back(ChessMove(startPos, newPos));
    }

    ChessMove newMove = ChessMove(startPos, newPos);
    if(newPos.isInBounds() && willMoveCapture(newMove)) moves.push_back(newMove);
}

vector<ChessMove> ChessBoard::getMovesForPiece(ChessPiece piece, ChessPos pos){
    vector<ChessMove> moves;

    char dRow, dRow1, dCol, dCol1, tmp;

    ChessMove newMove;

    switch(piece.pieceType){
        case kPiecePawn:
        default:
            dRow = 1;
            if(piece.player == kPlayerBlack) dRow = -1; //If black, go other direction

            //First, check if you can capture anything
            //TODO: check for en passant (holy hell)

            for(dCol = - 1; dCol < 2; dCol += 2){
                newMove = ChessMove(pos, ChessPos(pos.col + dCol, pos.row + dRow));

                if(newMove.isInBounds() && this->pieces_.count(newMove.newPos) && willMoveCapture(newMove)){
                    moves.push_back(newMove);
                }
            }

            //Then, see if you can move two spaces
            if((piece.player == kPlayerWhite && pos.row == 2) || (piece.player == kPlayerBlack && pos.row == 7)){
                newMove = ChessMove(pos, ChessPos(pos.col, pos.row + dRow + dRow));
                if(newMove.isInBounds() && !this->pieces_.count(newMove.newPos)) moves.push_back(newMove);
            }

            newMove = ChessMove(pos, ChessPos(pos.col, pos.row + dRow));
            if(newMove.isInBounds() && !this->pieces_.count(newMove.newPos)) moves.push_back(newMove);
            
        break;

        //The rook, bishop, and queen all use basically the same algorithm found in addMovesInDir

        case kPieceRook:
        dRow = 1;
        dCol = 0;

        for(int x = 0; x < 4; x++){
            addMovesInDir(moves, pos, dCol, dRow);
            
            tmp = dRow;
            dRow = -dCol;
            dCol = tmp;
        }
        break;

        case kPieceBishop:
        dRow = 1;
        dCol = 1;
        for(int x = 0; x < 4; x++){
            addMovesInDir(moves, pos, dCol, dRow);
            
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
            addMovesInDir(moves, pos, dCol, dRow);
            addMovesInDir(moves, pos, dCol1, dRow1);
            
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
            newMove = ChessMove(pos, ChessPos(pos.col + dCol, pos.row + dRow));
            //std::cout << newMove.newPos.str() << std::endl;
            if(newMove.isInBounds() && (!this->pieces_.count(newMove.newPos) || willMoveCapture(newMove))) moves.push_back(newMove);
            newMove = ChessMove(pos, ChessPos(pos.col + dRow, pos.row + dCol));
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
            newMove = ChessMove(pos, ChessPos(pos.col + dCol,pos.row + dRow));
            if(newMove.isInBounds() && (!this->pieces_.count(newMove.newPos) || willMoveCapture(newMove))) moves.push_back(newMove);
            newMove = ChessMove(pos, ChessPos(pos.col + dCol1,pos.row + dRow1));
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

vector<ChessMove> ChessBoard::getPossibleMoves(){

    vector<ChessMove> moves;
    Player currentPlayer = this->currentPlayer();

    for(auto pieceData : this->pieces_){
        if(pieceData.second.player == currentPlayer){
            vector<ChessMove> movesToAdd = this->getMovesForPiece(pieceData.second, pieceData.first);
            moves.insert(moves.end(), movesToAdd.begin(), movesToAdd.end());
        }
    }

    return moves;
}

//This already assumes that the move is valid, I don't think it will ever be the case that you'll call this function on an invalid move
void ChessBoard::doMove(ChessMove move){

    //TODO: Add piece to score

    this->pieces_[move.newPos] = this->pieces_[move.pos];
    this->pieces_.erase(move.pos);

    this->moves_.push_back(move);
    
}
