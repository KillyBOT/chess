#include <iostream>
#include <vector>
#include <cstring>

#include "move_generator.h"
#include "ray.h"

bool MoveGenerator::willMoveCapture(ChessMove &move) const {
    if(this->board_->hasPieceAtPos(move.newPos) && this->board_->piece(move.newPos).player() == this->opponent_) {
        move.captured = this->board_->piece(move.newPos);
        return true;
    }
    return false;
}
bool MoveGenerator::enPassantCheck(ChessMove &move) const {
    if(this->doEnPassantCheck_ && move.newPos.file() - 'a' + 1 == this->board_->enPassantFile()){
        ChessPos capturePos = move.newPos;
        if(move.piece.player() == kPlayerWhite) capturePos.pos -= 8;
        else capturePos.pos += 8;

        if(
            this->board_->piece(capturePos).type() == kPiecePawn &&
            this->board_->piece(capturePos).player() == this->opponent_
        ){
            move.captured = this->board_->piece(capturePos.pos);
            move.moveData = kMoveEnPassant;

            //Check for annoying edge case where the en passant capture puts your king in check
            vector<ChessPos> pieces;
            this->addPiecesInDir(pieces, this->board_->kingPos(this->player_), kRayDirE);
            if(
                pieces.size() < 3 ||
                pieces[0] != move.oldPos ||
                pieces[1] != capturePos ||
                this->board_->piece(pieces[2]).player() != this->opponent_ ||
                this->board_->piece(pieces[2]).type() != kPieceRook ||
                (this->board_->piece(pieces[2]).type() != kPieceRook && this->board_->piece(pieces[2]).type() != kPieceQueen)
            ) return false;

            pieces.clear();
            this->addPiecesInDir(pieces, this->board_->kingPos(this->player_), kRayDirW);
            if(
                pieces.size() < 3 ||
                pieces[0] != move.oldPos ||
                pieces[1] != capturePos ||
                this->board_->piece(pieces[2]).player() != this->opponent_ ||
                (this->board_->piece(pieces[2]).type() != kPieceRook && this->board_->piece(pieces[2]).type() != kPieceQueen)
            ) return false;

            return true;
        }
    }

    return false;
    
}

void MoveGenerator::addPiecesInDir(vector<ChessPos> &positions, ChessPos start, int dir) const{
    for(ChessPos &pos : kRays[start.pos][dir]){
        if(this->board_->hasPieceAtPos(ChessPos(pos))) positions.push_back(ChessPos(pos));
    }
}

//TODO: add code that checks for if a piece is pinned, and therefore is unable to move in any direction outside of the 
void MoveGenerator::setAttackedForPiece(ChessPiece piece, ChessPos start) {

    int pos;

    switch(piece.type()){
        case kPiecePawn:
        if(piece.player() == kPlayerWhite){
            if(kRays[start.pos][kRayDirNE].size()) this->attacked_[kRays[start.pos][kRayDirNE][0].pos] = true;
            if(kRays[start.pos][kRayDirNW].size()) this->attacked_[kRays[start.pos][kRayDirNW][0].pos] = true;
        } else {
            if(kRays[start.pos][kRayDirSE].size()) this->attacked_[kRays[start.pos][kRayDirSE][0].pos] = true;
            if(kRays[start.pos][kRayDirSW].size()) this->attacked_[kRays[start.pos][kRayDirSW][0].pos] = true;
        }

        break;

        case kPieceKnight:
        for(ChessPos &pos : kKnightPositionTable[start.pos]) this->attacked_[pos.pos] = true;

        break;

        case kPieceRook:
        for(int dir = 0; dir < 4; dir++){
            pos = 0;
            while(pos < kRays[start.pos][dir].size() && !this->board_->hasPieceAtPos(kRays[start.pos][dir][pos])) {
                this->attacked_[kRays[start.pos][dir][pos].pos] = true;
                pos++;
            }
            if(pos < kRays[start.pos][dir].size()) this->attacked_[kRays[start.pos][dir][pos].pos] = true;
        }

        break;

        case kPieceBishop:

        for(int dir = 4; dir < 8; dir++){
            pos = 0;
            while(pos < kRays[start.pos][dir].size() && !this->board_->hasPieceAtPos(kRays[start.pos][dir][pos])) {
                this->attacked_[kRays[start.pos][dir][pos].pos] = true;
                pos++;
            }
            if(pos < kRays[start.pos][dir].size()) this->attacked_[kRays[start.pos][dir][pos].pos] = true;
        }

        break;

        case kPieceQueen:

        for(int dir = 0; dir < 8; dir++){
            pos = 0;
            while(pos < kRays[start.pos][dir].size() && !this->board_->hasPieceAtPos(kRays[start.pos][dir][pos])) {
                this->attacked_[kRays[start.pos][dir][pos].pos] = true;
                pos++;
            }
            if(pos < kRays[start.pos][dir].size()) this->attacked_[kRays[start.pos][dir][pos].pos] = true;
        }

        break;

        case kPieceKing:
        for(int dir = 0; dir < 8; dir++){
            if(kRays[start.pos][dir].size()) this->attacked_[kRays[start.pos][dir][0].pos] = true;
        }
        
        break;
    }
}

void MoveGenerator::genMovesForPiece(vector<ChessMove> &moves, ChessPiece piece, ChessPos start) const {

    ChessMove move(piece, start, start);
    int oldSize = moves.size();
    bool isPinned = this->pinned_[start.pos][8];
    int pos;

    switch(piece.type())
    {
        case kPiecePawn:

        if(piece.player() == kPlayerWhite){
            
            if(kRays[start.pos][kRayDirNE].size() && (!isPinned || this->pinned_[start.pos][kRayDirNE])){
                move.newPos = ChessPos(kRays[start.pos][kRayDirNE][0]);
                if(this->willMoveCapture(move) || this->enPassantCheck(move)) {
                    if(move.newPos.rank() == '8'){
                        move.moveData = kMovePromotingToBishop;
                        moves.push_back(move);
                        move.moveData = kMovePromotingToRook;
                        moves.push_back(move);
                        move.moveData = kMovePromotingToKnight;
                        moves.push_back(move);
                        move.moveData = kMovePromotingToQueen;
                        moves.push_back(move);
                        move.moveData = kMoveNone;
                    } else {
                        moves.push_back(move);
                    }
                    move.captured = ChessPiece();
                }
                
            }  
            if(kRays[start.pos][kRayDirNW].size() && (!isPinned || this->pinned_[start.pos][kRayDirNW])){
                move.newPos = ChessPos(kRays[start.pos][kRayDirNW][0]);
                if(this->willMoveCapture(move) || this->enPassantCheck(move)) {
                    
                    if(move.newPos.rank() == '8'){
                        move.moveData = kMovePromotingToBishop;
                        moves.push_back(move);
                        move.moveData = kMovePromotingToRook;
                        moves.push_back(move);
                        move.moveData = kMovePromotingToKnight;
                        moves.push_back(move);
                        move.moveData = kMovePromotingToQueen;
                        moves.push_back(move);
                        move.moveData = kMoveNone;
                    } else {
                        moves.push_back(move);
                    }
                    move.captured = ChessPiece();
                }
            }      
            //First check directly in front
            if(!isPinned || this->pinned_[start.pos][kRayDirN]){
                bool doNextCheck = false;
                move.newPos = ChessPos(kRays[start.pos][kRayDirN][0]);
                if(!this->board_->hasPieceAtPos(move.newPos)){
                    if(move.newPos.rank() == '8'){
                        move.moveData = kMovePromotingToBishop;
                        moves.push_back(move);
                        move.moveData = kMovePromotingToRook;
                        moves.push_back(move);
                        move.moveData = kMovePromotingToKnight;
                        moves.push_back(move);
                        move.moveData = kMovePromotingToQueen;
                        moves.push_back(move);
                        move.moveData = kMoveNone;
                    } else {
                        moves.push_back(move);
                        doNextCheck = true;
                    }
                }

                //Then, check two squares forward
                if(doNextCheck && kRays[start.pos][kRayDirN].size() > 1){
                    move.newPos = ChessPos(kRays[start.pos][kRayDirN][1]);
                    if(!this->board_->hasPieceAtPos(move.newPos)) {
                        if(!move.piece.hasMoved()){
                            move.moveData = kMoveEnPassantEligible;
                        }
                        else if(move.newPos.rank() == '8'){
                            move.moveData = kMovePromotingToBishop;
                            moves.push_back(move);
                            move.moveData = kMovePromotingToRook;
                            moves.push_back(move);
                            move.moveData = kMovePromotingToKnight;
                            moves.push_back(move);
                            move.moveData = kMovePromotingToQueen;
                        }
                        moves.push_back(move);
                        move.moveData = kMoveNone;
                    }
                }
            }

        } else { //kPlayerBlack
            if(kRays[start.pos][kRayDirSE].size() && (!isPinned || this->pinned_[start.pos][kRayDirSE])){
                move.newPos = ChessPos(kRays[start.pos][kRayDirSE][0]);
                if(this->willMoveCapture(move) || this->enPassantCheck(move)) {
                    if(move.newPos.rank() == '1'){
                        move.moveData = kMovePromotingToBishop;
                        moves.push_back(move);
                        move.moveData = kMovePromotingToRook;
                        moves.push_back(move);
                        move.moveData = kMovePromotingToKnight;
                        moves.push_back(move);
                        move.moveData = kMovePromotingToQueen;
                        moves.push_back(move);
                        move.moveData = kMoveNone;
                    } else {
                        moves.push_back(move);
                    }
                    move.captured = ChessPiece();
                }
                
            }  
            if(kRays[start.pos][kRayDirSW].size() && (!isPinned || this->pinned_[start.pos][kRayDirSW])){
                move.newPos = ChessPos(kRays[start.pos][kRayDirSW][0]);
                if(this->willMoveCapture(move) || this->enPassantCheck(move)) {
                    
                    if(move.newPos.rank() == '1'){
                        move.moveData = kMovePromotingToBishop;
                        moves.push_back(move);
                        move.moveData = kMovePromotingToRook;
                        moves.push_back(move);
                        move.moveData = kMovePromotingToKnight;
                        moves.push_back(move);
                        move.moveData = kMovePromotingToQueen;
                        moves.push_back(move);
                        move.moveData = kMoveNone;
                    } else {
                        moves.push_back(move);
                    }
                    move.captured = ChessPiece();
                }
            }      
   
            //First check directly in front
            if(!isPinned || this->pinned_[start.pos][kRayDirS]){
                bool doNextCheck = false;
                move.newPos = ChessPos(kRays[start.pos][kRayDirS][0]);
                if(!this->board_->hasPieceAtPos(move.newPos)){
                    if(move.newPos.rank() == '1'){
                        move.moveData = kMovePromotingToBishop;
                        moves.push_back(move);
                        move.moveData = kMovePromotingToRook;
                        moves.push_back(move);
                        move.moveData = kMovePromotingToKnight;
                        moves.push_back(move);
                        move.moveData = kMovePromotingToQueen;
                        moves.push_back(move);
                        move.moveData = kMoveNone;
                    } else {
                        moves.push_back(move);
                        doNextCheck = true;
                    }
                }

                //Then, check two squares forward
                if(doNextCheck && kRays[start.pos][kRayDirS].size() > 1){
                    move.newPos = ChessPos(kRays[start.pos][kRayDirS][1]);
                    if(!this->board_->hasPieceAtPos(move.newPos)) {
                        if(!move.piece.hasMoved()){
                            move.moveData = kMoveEnPassantEligible;
                        }
                        else if(move.newPos.rank() == '1'){
                            move.moveData = kMovePromotingToBishop;
                            moves.push_back(move);
                            move.moveData = kMovePromotingToRook;
                            moves.push_back(move);
                            move.moveData = kMovePromotingToKnight;
                            moves.push_back(move);
                            move.moveData = kMovePromotingToQueen;
                        }
                        moves.push_back(move);
                        move.moveData = kMoveNone;
                    }
                }
            }
        }

        break;

        case kPieceKnight:

        //If knights are pinned there isn't anywhere that they can go
        if(isPinned) return;

        for(ChessPos pos : kKnightPositionTable[start.pos]){
            move.newPos = pos;
            move.captured = ChessPiece();
            move.moveData = kMoveNone;
            if(!this->board_->hasPieceAtPos(move.newPos) || this->willMoveCapture(move)) moves.push_back(move);
        }

        case kPieceRook:

        for(int dir = 0; dir < 4; dir++){
            if(!isPinned || this->pinned_[start.pos][dir]){
                pos = 0;
                move.captured = ChessPiece();
                move.moveData = kMoveNone;
                while(pos < kRays[start.pos][dir].size() && !this->board_->hasPieceAtPos(kRays[start.pos][dir][pos])) {
                    move.newPos = kRays[start.pos][dir][pos];
                    moves.push_back(move);
                    pos++;
                }
                
                if(pos < kRays[start.pos][dir].size()) {
                    move.newPos = kRays[start.pos][dir][pos];
                    if(!this->board_->hasPieceAtPos(move.newPos) || this->willMoveCapture(move)) moves.push_back(move);
                }
            }
            
        }

        break;

        case kPieceBishop:

        for(int dir = 4; dir < 8; dir++){
            if(!isPinned || this->pinned_[start.pos][dir]){
                pos = 0;
                move.captured = ChessPiece();
                move.moveData = kMoveNone;
                while(pos < kRays[start.pos][dir].size() && !this->board_->hasPieceAtPos(kRays[start.pos][dir][pos])) {
                    move.newPos = kRays[start.pos][dir][pos];
                    moves.push_back(move);
                    pos++;
                }
                
                if(pos < kRays[start.pos][dir].size()) {
                    move.newPos = kRays[start.pos][dir][pos];
                    if(!this->board_->hasPieceAtPos(move.newPos) || this->willMoveCapture(move)) moves.push_back(move);
                }
            }
        }

        break;

        case kPieceQueen:

        for(int dir = 0; dir < 8; dir++){
            if(!isPinned || this->pinned_[start.pos][dir]){
                pos = 0;
                move.captured = ChessPiece();
                move.moveData = kMoveNone;
                while(pos < kRays[start.pos][dir].size() && !this->board_->hasPieceAtPos(kRays[start.pos][dir][pos])) {
                    move.newPos = kRays[start.pos][dir][pos];
                    moves.push_back(move);
                    pos++;
                }
                
                if(pos < kRays[start.pos][dir].size()) {
                    move.newPos = kRays[start.pos][dir][pos];
                    if(!this->board_->hasPieceAtPos(move.newPos) || this->willMoveCapture(move)) moves.push_back(move);
                }
            }
        }

        break;

        case kPieceKing:

        for(int dir = 0; dir < 8; dir++){
            if(kRays[start.pos][dir].size()){
                move.captured = ChessPiece();
                move.moveData = kMoveNone;
                move.newPos = kRays[start.pos][dir][0];
                if(!this->attacked_[move.newPos.pos] && (!this->board_->hasPieceAtPos(move.newPos) || this->willMoveCapture(move)))moves.push_back(move);
            }
        }

        if(!piece.hasMoved()){
            move.moveData = kMoveIsCastling;
            //Queenside
            if(
                this->board_->hasPieceAtPos(ChessPos('a',start.rank())) &&
                this->board_->piece(ChessPos('a',start.rank())) == ChessPiece(kPieceRook, this->player_) &&
                !this->board_->hasPieceAtPos(ChessPos('b',start.rank())) &&
                !this->board_->hasPieceAtPos(ChessPos('c',start.rank())) &&
                !this->board_->hasPieceAtPos(ChessPos('d',start.rank())) &&
                !this->attacked_[ChessPos('b',start.rank()).pos]
            ) {
                move.newPos = ChessPos('b',start.rank());
                moves.push_back(move);
            }

            //Kingside
            if(
                this->board_->hasPieceAtPos(ChessPos('h',start.rank())) &&
                this->board_->piece(ChessPos('h',start.rank())) == ChessPiece(kPieceRook, this->player_) &&
                !this->board_->hasPieceAtPos(ChessPos('f',start.rank())) &&
                !this->board_->hasPieceAtPos(ChessPos('g',start.rank())) &&
                !this->attacked_[ChessPos('g',start.rank()).pos]
            ) {
                move.newPos = ChessPos('g',start.rank());
                moves.push_back(move);
            }
        }
        
        break;
    }
}

void MoveGenerator::setPinned() {
    vector<ChessPos> piecesInDir;
    
    ChessPos kingPos = this->board_->kingPos(this->player_);
    for(int dir = 0; dir < 4; dir++){
        piecesInDir.clear();
        this->addPiecesInDir(piecesInDir, kingPos, dir);
        if(
            piecesInDir.size() > 1 &&
            this->board_->piece(piecesInDir[0]).player() == this->player_ &&
            this->board_->piece(piecesInDir[1]).player() == this->opponent_ &&
            (this->board_->piece(piecesInDir[1]).type() == kPieceRook || this->board_->piece(piecesInDir[1]).type() == kPieceQueen)
        ) {
            this->pinned_[piecesInDir[0].pos][dir] = true;
            this->pinned_[piecesInDir[0].pos][opposite_dir(dir)] = true;
            this->pinned_[piecesInDir[0].pos][8] = true;
        }
    }
    for(int dir = 4; dir < 8; dir++){
        piecesInDir.clear();
        this->addPiecesInDir(piecesInDir, kingPos, dir);
        if(
            piecesInDir.size() > 1 &&
            this->board_->piece(piecesInDir[0]).player() == this->player_ &&
            this->board_->piece(piecesInDir[1]).player() == this->opponent_ &&
            (this->board_->piece(piecesInDir[1]).type() == kPieceBishop || this->board_->piece(piecesInDir[1]).type() == kPieceQueen)
        ) {
            this->pinned_[piecesInDir[0].pos][dir] = true;
            this->pinned_[piecesInDir[0].pos][opposite_dir(dir)] = true;
            this->pinned_[piecesInDir[0].pos][8] = true;
        }
    }
}
void MoveGenerator::setAttacked() {
    memset(this->attacked_, false, 64);

    const ChessPos *positions = this->board_->piecePositions();

    for(int i = 0; i < this->board_->pieceNum(); i++){
        ChessPos pos = positions[i];
        ChessPiece piece = this->board_->piece(pos);
        if(piece.player() == this->opponent_) this->setAttackedForPiece(piece, pos);
    }
}


//TODO: Finish this
void MoveGenerator::setForced() {

    bool foundThreat = false;
}

MoveGenerator::MoveGenerator() {
    return;
}

bool MoveGenerator::stalemate() const {
    if(this->board_->movesSinceLastCapture() >= 50) return true;

    return false;
}
bool MoveGenerator::stalemate(ChessBoard &board) {
    this->setBoard(board);
    return this->stalemate();
}

bool MoveGenerator::inCheck() const {
    return this->attacked_[this->board_->kingPos(this->player_).pos];
}
bool MoveGenerator::inCheck(ChessBoard &board) {
    this->setBoard(board);
    return this->inCheck();
}

// bool MoveGenerator::hasLost() const;
// bool MoveGenerator::hasLost(ChessBoard &board);

vector<ChessMove> MoveGenerator::getMoves() {
    vector<ChessMove> moves;

    for(int i = 0; i < this->board_->pieceNum(); i++){
        ChessPos pos = this->board_->piecePositions()[i];
        ChessPiece piece = this->board_->piece(pos);
        if(piece.player() == this->player_) this->genMovesForPiece(moves, piece, pos);
    }

    return moves;
}
vector<ChessMove> MoveGenerator::getMoves(ChessBoard &board) {
    this->setBoard(board);

    return this->getMoves();
}

void MoveGenerator::setBoard(ChessBoard &board) {

    memset(this->attacked_, false, 64);
    memset(this->pinned_, false, 576);
    memset(this->forced_, false, 65);

    this->board_ = &board;
    this->player_ = board.player();
    this->opponent_ = board.opponent();
    this->doEnPassantCheck_ = board.enPassantFile();

    this->setAttacked();
    this->setPinned();
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