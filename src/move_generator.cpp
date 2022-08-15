#include <iostream>
#include <vector>
#include <cstring>
#include <unordered_map>

#include "board.h"
#include "piece_list.h"
#include "move_generator.h"
#include "ray.h"

static const char kPieceListInd[2][7] = {0, 0b0001, 0b0010, 0b0011, 0b0100, 0b0101, 0b0110, 0, 0b1001, 0b1010, 0b1011, 0b1100, 0b1101, 0b1110};

unordered_map<std::size_t, std::vector<ChessMove>> MoveGenerator::knownBoards_ = unordered_map<std::size_t, std::vector<ChessMove>>();

bool MoveGenerator::willMoveCapture(ChessMove &move) const {
    if(this->board_->hasPieceAtPos(move.newPos) && this->board_->piece(move.newPos).player() == this->opponent_) {
        move.captured = this->board_->piece(move.newPos);
        return true;
    }
    return false;
}
bool MoveGenerator::enPassantCheck(ChessMove &move) const {
    if(this->doEnPassantCheck_ && move.newPos.file() == this->board_->enPassantFile()){
        ChessPos capturePos = move.newPos;
        if(move.piece.player() == kPlayerWhite) capturePos.pos -= 8;
        else capturePos.pos += 8;

        if(
            this->board_->piece(capturePos).type() == kPiecePawn &&
            this->board_->piece(capturePos).player() == this->opponent_
        ){

            //Check for annoying edge case where the en passant capture puts your king in check
            vector<ChessPos> pieces;
            this->addPiecesInDir(pieces, this->kingPos_, kRayDirE);
            if(
                pieces.size() > 2 &&
                pieces[0] == move.oldPos &&
                pieces[1] == capturePos &&
                this->board_->piece(pieces[2]).player() == this->opponent_ &&
                (this->board_->piece(pieces[2]).type() == kPieceRook || this->board_->piece(pieces[2]).type() == kPieceQueen)
            ) return false;

            pieces.clear();
            this->addPiecesInDir(pieces, this->kingPos_, kRayDirW);
            if(
                pieces.size() > 2 &&
                pieces[0] == move.oldPos &&
                pieces[1] == capturePos &&
                this->board_->piece(pieces[2]).player() == this->opponent_ &&
                (this->board_->piece(pieces[2]).type() == kPieceRook || this->board_->piece(pieces[2]).type() == kPieceQueen)
            ) return false;

            move.captured = this->board_->piece(capturePos);
            move.moveData = kMoveEnPassant;

            return true;
        }
    }

    return false;
    
}

void MoveGenerator::addPiecesInDir(vector<ChessPos> &positions, ChessPos start, int dir) const{
    //std::cout << start.str() << '\t' << dir << '\t' << kRays[start.pos][dir].size() << std::endl;
    for(ChessPos &pos : kRays[start.pos][dir]){
        if(this->board_->hasPieceAtPos(pos)) positions.push_back(pos);
    }
}

void MoveGenerator::genPawnAttacks() {
    char dirL, dirR;

    const ChessPieceList &list = this->board_->pieceList(kPieceListInd[this->opponent_][kPiecePawn]);

    if(this->player_ == kPlayerWhite){
        dirL = kRayDirNE;
        dirR = kRayDirNW;
    } else {
        dirL = kRayDirSW;
        dirR = kRayDirSE;
    }

    for(int i = 0; i < list.size(); i++){
        const ChessPos &start = list[i];

        if(kRays[start.pos][dirL].size()) this->attacked_[kRays[start.pos][dirL][0].pos] = true;
        if(kRays[start.pos][dirR].size()) this->attacked_[kRays[start.pos][dirR][0].pos] = true;
    }
}
void MoveGenerator::genKnightAttacks() {
    const ChessPieceList &list = this->board_->pieceList(kPieceListInd[this->opponent_][kPieceKnight]);

    for(int i = 0; i < list.size(); i++){
        const ChessPos &start = list[i];
        for(ChessPos &attacked : kKnightPositionTable[start.pos]) this->attacked_[attacked.pos] = true;
    }
}
void MoveGenerator::genKingAttacks() {

    int opponentKingPosInt = this->board_->kingPos(this->opponent_).pos;

    for(int dir = 0; dir < 8; dir++){
        if(kRays[opponentKingPosInt][dir].size()) this->attacked_[kRays[opponentKingPosInt][dir][0].pos] = true;
    }
}
void MoveGenerator::genSlidingAttacks() {
    const ChessPieceList &rookList = this->board_->pieceList(kPieceListInd[this->opponent_][kPieceRook]);
    const ChessPieceList &bishopList = this->board_->pieceList(kPieceListInd[this->opponent_][kPieceBishop]);
    const ChessPieceList &queenList = this->board_->pieceList(kPieceListInd[this->opponent_][kPieceQueen]);

    bool isPinned;
    int posInDir;
    ChessMove move;

    for(int i = 0; i < rookList.size(); i++){

        const ChessPos &start = rookList[i];

        for(int dir = 0; dir < 4; dir++){
            
            for(posInDir = 0; posInDir < kRays[start.pos][dir].size() && (!this->board_->hasPieceAtPos(kRays[start.pos][dir][posInDir]) || kRays[start.pos][dir][posInDir] == this->kingPos_); posInDir++){
                this->attacked_[kRays[start.pos][dir][posInDir].pos] = true;
            }
            
            if(posInDir < kRays[start.pos][dir].size()) this->attacked_[kRays[start.pos][dir][posInDir].pos] = true;
        }

    }

    for(int i = 0; i < bishopList.size(); i++){

        const ChessPos &start = bishopList[i];

        for(int dir = 4; dir < 8; dir++){
            
            for(posInDir = 0; posInDir < kRays[start.pos][dir].size() && (!this->board_->hasPieceAtPos(kRays[start.pos][dir][posInDir]) || kRays[start.pos][dir][posInDir] == this->kingPos_); posInDir++){
                this->attacked_[kRays[start.pos][dir][posInDir].pos] = true;
            }
            
            if(posInDir < kRays[start.pos][dir].size()) this->attacked_[kRays[start.pos][dir][posInDir].pos] = true;
        }

    }

    for(int i = 0; i < queenList.size(); i++){

        const ChessPos &start = queenList[i];

        for(int dir = 0; dir < 8; dir++){
            
            for(posInDir = 0; posInDir < kRays[start.pos][dir].size() && (!this->board_->hasPieceAtPos(kRays[start.pos][dir][posInDir]) || kRays[start.pos][dir][posInDir] == this->kingPos_); posInDir++){
                this->attacked_[kRays[start.pos][dir][posInDir].pos] = true;
            }
            
            if(posInDir < kRays[start.pos][dir].size()) this->attacked_[kRays[start.pos][dir][posInDir].pos] = true;
        }
    }
}
//TODO: add code that checks for if a piece is pinned, and therefore is unable to move in any direction outside of the 
/*void MoveGenerator::setAttackedForPiece(ChessPiece piece, ChessPos start) {

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
            while(pos < kRays[start.pos][dir].size() && (!this->board_->hasPieceAtPos(kRays[start.pos][dir][pos]) || kRays[start.pos][dir][pos] == this->kingPos_)) {
                this->attacked_[kRays[start.pos][dir][pos].pos] = true;
                pos++;
            }
            if(pos < kRays[start.pos][dir].size()) this->attacked_[kRays[start.pos][dir][pos].pos] = true;
        }

        break;

        case kPieceBishop:

        for(int dir = 4; dir < 8; dir++){
            pos = 0;
            while(pos < kRays[start.pos][dir].size() && (!this->board_->hasPieceAtPos(kRays[start.pos][dir][pos]) || kRays[start.pos][dir][pos] == this->kingPos_)) {
                this->attacked_[kRays[start.pos][dir][pos].pos] = true;
                pos++;
            }
            if(pos < kRays[start.pos][dir].size()) this->attacked_[kRays[start.pos][dir][pos].pos] = true;
        }

        break;

        case kPieceQueen:

        for(int dir = 0; dir < 8; dir++){
            pos = 0;
            while(pos < kRays[start.pos][dir].size() && (!this->board_->hasPieceAtPos(kRays[start.pos][dir][pos]) || kRays[start.pos][dir][pos] == this->kingPos_)) {
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
*/

void MoveGenerator::genKingMoves(vector<ChessMove> &moves) const {

    ChessMove move = ChessMove(this->board_->piece(this->kingPos_),this->kingPos_,this->kingPos_);
    //First check eight directions
    for(int dir = 0; dir < 8; dir++){
        if(kRays[this->kingPos_.pos][dir].size()){
            move.newPos = kRays[this->kingPos_.pos][dir][0];
            //std::cout << move.newPos.str() << '\t' << this->attacked_[move.newPos.pos] << std::endl;
            if(!this->attacked_[move.newPos.pos] && (!this->board_->hasPieceAtPos(move.newPos) || this->willMoveCapture(move))){
                moves.push_back(move);
                move.captured = ChessPiece();
            }
        }
    }

    //Then check if you can castle
    if(!this->board_->piece(this->kingPos_).hasMoved){
        move.moveData = kMoveIsCastling;
        //Queenside
        if(
            this->board_->hasPieceAtPos(ChessPos('a',this->kingPos_.rank())) &&
            this->board_->piece(ChessPos('a',this->kingPos_.rank())) == ChessPiece(kPieceRook, this->player_) &&
            !this->board_->hasPieceAtPos(ChessPos('b',this->kingPos_.rank())) &&
            !this->board_->hasPieceAtPos(ChessPos('c',this->kingPos_.rank())) &&
            !this->board_->hasPieceAtPos(ChessPos('d',this->kingPos_.rank())) &&
            !this->attacked_[ChessPos('b',this->kingPos_.rank()).pos]
        ) {
            move.newPos = ChessPos('b',this->kingPos_.rank());
            moves.push_back(move);
        }

        //Kingside
        if(
            this->board_->hasPieceAtPos(ChessPos('h',this->kingPos_.rank())) &&
            this->board_->piece(ChessPos('h',this->kingPos_.rank())) == ChessPiece(kPieceRook, this->player_) &&
            !this->board_->hasPieceAtPos(ChessPos('f',this->kingPos_.rank())) &&
            !this->board_->hasPieceAtPos(ChessPos('g',this->kingPos_.rank())) &&
            !this->attacked_[ChessPos('g',this->kingPos_.rank()).pos]
        ) {
            move.newPos = ChessPos('g',this->kingPos_.rank());
            moves.push_back(move);
        }
    }
}
void MoveGenerator::genKnightMoves(vector<ChessMove> &moves) const {

    const ChessPieceList &list = this->board_->pieceList(kPieceListInd[this->player_][kPieceKnight]);

    for(int i = 0; i < list.size(); i++){
        const ChessPos &start = list[i];

        //If a knight is pinned, it cannot move
        if(this->pinned_[start.pos][8]) continue;

        for(ChessPos &newPos : kKnightPositionTable[start.pos]){
            ChessMove move(this->board_->piece(start), start, newPos);
            if(!this->board_->hasPieceAtPos(newPos) || this->willMoveCapture(move)) this->addMove(moves,move);
        }
    }
}
void MoveGenerator::genPawnMoves(vector<ChessMove> &moves) const {

    char dirL, dirR, dirF, promoteRank;
    bool isPinned, doTwoMoveCheck;
    ChessMove move;

    if(this->player_ == kPlayerWhite){
        dirL = kRayDirNE;
        dirR = kRayDirNW;
        dirF = kRayDirN;
        promoteRank = 7;
    } else {
        dirL = kRayDirSW;
        dirR = kRayDirSE;
        dirF = kRayDirS;
        promoteRank = 0;
    }

    const ChessPieceList &list = this->board_->pieceList(kPieceListInd[this->player_][kPiecePawn]);

    for(int i = 0; i < list.size(); i++){
        const ChessPos &start = list[i];
        move = ChessMove(this->board_->piece(start), start, start);
        isPinned = this->pinned_[start.pos][8];

        if(kRays[start.pos][dirL].size() && (!isPinned || this->pinned_[start.pos][dirL])){
            move.newPos = kRays[start.pos][dirL][0];
            if(this->willMoveCapture(move) || this->enPassantCheck(move)){
                if(move.newPos.rank() == promoteRank){
                    move.moveData = kMovePromotingToBishop;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToRook;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToKnight;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToQueen;
                }

                this->addMove(moves,move);
                move.moveData = kMoveNone;
            }
        }
        if(kRays[start.pos][dirR].size() && (!isPinned || this->pinned_[start.pos][dirR])){
            move.newPos = kRays[start.pos][dirR][0];
            if(this->willMoveCapture(move) || this->enPassantCheck(move)){
                if(move.newPos.rank() == promoteRank){
                    move.moveData = kMovePromotingToBishop;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToRook;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToKnight;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToQueen;
                }

                this->addMove(moves,move);
                move.moveData = kMoveNone;
            }
        }

        move.captured = ChessPiece();

        doTwoMoveCheck = false;
        if(kRays[start.pos][dirF].size() && (!isPinned || this->pinned_[start.pos][dirF])){
            move.newPos = kRays[start.pos][dirF][0];
            if(!this->board_->hasPieceAtPos(move.newPos)){
                doTwoMoveCheck = true;
                if(move.newPos.rank() == promoteRank){
                    move.moveData = kMovePromotingToBishop;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToRook;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToKnight;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToQueen;
                }
                this->addMove(moves,move);
                move.moveData = false;
            }
        }

        if(doTwoMoveCheck && kRays[start.pos][dirF].size() > 1 && !move.piece.hasMoved){
            move.newPos = kRays[start.pos][dirF][1];
            if(!this->board_->hasPieceAtPos(move.newPos)){
                if(move.newPos.rank() == promoteRank){
                    move.moveData = kMovePromotingToBishop;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToRook;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToKnight;
                    this->addMove(moves,move);
                    move.moveData = kMovePromotingToQueen;
                } else {
                    move.moveData = kMoveEnPassantEligible;
                }

                this->addMove(moves,move);
            }
        }
    }

}
void MoveGenerator::genSlidingMoves(vector<ChessMove> &moves) const {
    const ChessPieceList &rookList = this->board_->pieceList(kPieceListInd[this->player_][kPieceRook]);
    const ChessPieceList &bishopList = this->board_->pieceList(kPieceListInd[this->player_][kPieceBishop]);
    const ChessPieceList &queenList = this->board_->pieceList(kPieceListInd[this->player_][kPieceQueen]);

    bool isPinned;
    int posInDir;
    ChessMove move;

    for(int i = 0; i < rookList.size(); i++){

        const ChessPos &start = rookList[i];
        isPinned = this->pinned_[start.pos][8];
        move = ChessMove(this->board_->piece(start),start,start);

        for(int dir = 0; dir < 4; dir++){
            if(!isPinned || this->pinned_[start.pos][dir]){
                //std::cout << start.str() << std::endl;
                for(posInDir = 0; posInDir < kRays[start.pos][dir].size() && !this->board_->hasPieceAtPos(kRays[start.pos][dir][posInDir]); posInDir++) {
                    move.newPos = kRays[start.pos][dir][posInDir];
                    this->addMove(moves,move);
                }
                
                if(posInDir < kRays[start.pos][dir].size()) {
                    move.newPos = kRays[start.pos][dir][posInDir];
                    if(this->willMoveCapture(move)) {
                        this->addMove(moves,move);
                        move.captured = ChessPiece();
                    }
                }
            }
        }
    }

    for(int i = 0; i < bishopList.size(); i++){

        const ChessPos &start = bishopList[i];
        isPinned = this->pinned_[start.pos][8];
        move = ChessMove(this->board_->piece(start),start,start);

        for(int dir = 4; dir < 8; dir++){
            if(!isPinned || this->pinned_[start.pos][dir]){
                for(posInDir = 0; posInDir < kRays[start.pos][dir].size() && !this->board_->hasPieceAtPos(kRays[start.pos][dir][posInDir]); posInDir++) {
                    move.newPos = kRays[start.pos][dir][posInDir];
                    this->addMove(moves,move);
                }
                
                if(posInDir < kRays[start.pos][dir].size()) {
                    move.newPos = kRays[start.pos][dir][posInDir];
                    if(this->willMoveCapture(move)) {
                        this->addMove(moves,move);
                        move.captured = ChessPiece();
                    }
                }
            }
        }

    }

    for(int i = 0; i < queenList.size(); i++){

        const ChessPos &start = queenList[i];
        isPinned = this->pinned_[start.pos][8];
        move = ChessMove(this->board_->piece(start),start,start);

        for(int dir = 0; dir < 8; dir++){
            if(!isPinned || this->pinned_[start.pos][dir]){
                for(posInDir = 0; posInDir < kRays[start.pos][dir].size() && !this->board_->hasPieceAtPos(kRays[start.pos][dir][posInDir]); posInDir++) {
                    move.newPos = kRays[start.pos][dir][posInDir];
                    this->addMove(moves,move);
                }
                
                if(posInDir < kRays[start.pos][dir].size()) {
                    move.newPos = kRays[start.pos][dir][posInDir];
                    if(this->willMoveCapture(move)) {
                        this->addMove(moves,move);
                        move.captured = ChessPiece();
                    }
                }
            }
        }
    }
}

/*void MoveGenerator::genMovesForPiece(vector<ChessMove> &moves, ChessPiece piece, ChessPos start) const {

    ChessMove move(piece, start, start);
    int oldSize = moves.size();
    bool isPinned = this->pinned_[start.pos][8];
    int pos;

    //std::cout << start.str() << '\t' << piece.pieceChar() << std::endl;

    switch(piece.type())
    {
        case kPiecePawn:

        if(piece.player() == kPlayerWhite){
            
            if(kRays[start.pos][kRayDirNE].size() && (!isPinned || this->pinned_[start.pos][kRayDirNE])){
                move.newPos = kRays[start.pos][kRayDirNE][0];
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
                    move.moveData = kMoveNone;
                    move.captured = ChessPiece();
                }
                
            
            }  
            if(kRays[start.pos][kRayDirNW].size() && (!isPinned || this->pinned_[start.pos][kRayDirNW])){
                move.newPos = kRays[start.pos][kRayDirNW][0];
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
                    } else {
                        moves.push_back(move);
                    }
                    move.moveData = kMoveNone;
                    move.captured = ChessPiece();
                }
            }      
            //First check directly in front
            if((!isPinned || this->pinned_[start.pos][kRayDirN]) && kRays[start.pos][kRayDirN].size()){
                bool doNextCheck = false;
                move.newPos = kRays[start.pos][kRayDirN][0];
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
                if(doNextCheck && !move.piece.hasMoved() && kRays[start.pos][kRayDirN].size() > 1){
                    move.newPos = kRays[start.pos][kRayDirN][1];
                    move.moveData = kMoveEnPassantEligible;
                    if(!this->board_->hasPieceAtPos(move.newPos)) {
                        if(move.newPos.rank() == '8'){
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
                move.newPos = kRays[start.pos][kRayDirSE][0];
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
                    } else {
                        moves.push_back(move);
                    }
                    move.moveData = kMoveNone;
                    move.captured = ChessPiece();
                }
                
            }  
            if(kRays[start.pos][kRayDirSW].size() && (!isPinned || this->pinned_[start.pos][kRayDirSW])){
                move.newPos = kRays[start.pos][kRayDirSW][0];
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
                    } else {
                        moves.push_back(move);
                    }
                    move.moveData = kMoveNone;
                    move.captured = ChessPiece();
                }
            }      
   
            //First check directly in front
            if((!isPinned || this->pinned_[start.pos][kRayDirS]) && kRays[start.pos][kRayDirS].size()){
                bool doNextCheck = false;
                move.newPos = kRays[start.pos][kRayDirS][0];
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
                if(doNextCheck && !move.piece.hasMoved() && kRays[start.pos][kRayDirS].size() > 1){
                    move.newPos = kRays[start.pos][kRayDirS][1];
                    move.moveData = kMoveEnPassantEligible;
                    if(!this->board_->hasPieceAtPos(move.newPos)) {
                        if(move.newPos.rank() == '1'){
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

        break;

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
                //std::cout << move.newPos.str() << '\t' << this->attacked_[move.newPos.pos] << std::endl;
                if(!this->attacked_[move.newPos.pos] && (!this->board_->hasPieceAtPos(move.newPos) || this->willMoveCapture(move))){
                    //std::cout << "Adding move" << std::endl;
                    moves.push_back(move);
                }
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

    // if(piece.type() == kPieceKing){
    //     this->board_->printBoard();
    //     this->printAttacked();
    //     for(int i = oldSize; i < moves.size(); i++) std::cout << moves[i].str() << ", ";
    //     std::cout << std::endl;
    // }
}
*/

void MoveGenerator::setPinned() {

    memset(this->pinned_, false, 576);

    vector<ChessPos> piecesInDir;
    
    for(int dir = 0; dir < 4; dir++){
        piecesInDir.clear();
        this->addPiecesInDir(piecesInDir, this->kingPos_, dir);
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
        this->addPiecesInDir(piecesInDir, this->kingPos_, dir);
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

    // for(int i = 0; i < this->board_->pieceNum(); i++){
    //     ChessPos pos = this->board_->piecePositions()[i];
    //     ChessPiece piece = this->board_->piece(pos);
    //     if(piece.player() == this->opponent_) this->setAttackedForPiece(piece, pos);
    // }

    this->genPawnAttacks();
    this->genKingAttacks();
    this->genKnightAttacks();
    this->genSlidingAttacks();
}
void MoveGenerator::setForced() {

    memset(this->forced_, false, 64);

    this->cannotMove_ = false;
    this->hasForced_ = false;

    char dirL, dirR;

    if(this->player_ == kPlayerWhite) {
        dirL = kRayDirNW;
        dirR = kRayDirNE;
    } else {
        dirL = kRayDirSE;
        dirR = kRayDirSW;
    }

    //First check for pawns

    if(
        kRays[this->kingPos_.pos][dirL].size() && 
        this->board_->piece(kRays[this->kingPos_.pos][dirL][0]).type() == kPiecePawn && 
        this->board_->piece(kRays[this->kingPos_.pos][dirL][0]).player() == this->opponent_
    ){
        this->hasForced_ = true;
        this->forced_[kRays[this->kingPos_.pos][dirL][0].pos] = true;
    }

    if(
        kRays[this->kingPos_.pos][dirR].size() && 
        this->board_->piece(kRays[this->kingPos_.pos][dirR][0]).type() == kPiecePawn && 
        this->board_->piece(kRays[this->kingPos_.pos][dirR][0]).player() == this->opponent_
    ){
        if(this->hasForced_){
            this->cannotMove_ = true;
            return;
        } else {
            this->hasForced_ = true;
            this->forced_[kRays[this->kingPos_.pos][dirR][0].pos] = true;
        }
        
    }

    //Then, check for knights

    for(ChessPos &pos : kKnightPositionTable[this->kingPos_.pos]){
        if(
            this->board_->hasPieceAtPos(pos) &&
            this->board_->piece(pos).type() == kPieceKnight &&
            this->board_->piece(pos).player() == this->opponent_
        ) {
            if(this->hasForced_){
                this->cannotMove_ = true;
                return;
            } else {
                this->hasForced_ = true;
                this->forced_[pos.pos] = true;
            }
        }
    }

    //Then, check for rooks and queens

    vector<ChessPos> piecesInDir;

    for(int dir = 0; dir < 4; dir++){
        for(int ind = 0; ind < kRays[this->kingPos_.pos][dir].size(); ind++){
            ChessPos &pos = kRays[this->kingPos_.pos][dir][ind];
            if(this->board_->hasPieceAtPos(pos)){
                if(
                    this->board_->piece(pos).player() == this->opponent_ &&
                    (
                        this->board_->piece(pos).type() == kPieceQueen ||
                        this->board_->piece(pos).type() == kPieceRook
                    )
                ){
                    if(this->hasForced_){
                        this->cannotMove_ = true;
                        return;
                    } else {
                        this->hasForced_ = true;
                        for(int indToAdd = 0; indToAdd <= ind; indToAdd++ ) this->forced_[kRays[this->kingPos_.pos][dir][indToAdd].pos] = true;
                    }
                }
                break;
            }
        }
    }

    //Finally, do the same for bishops and queens

    for(int dir = 4; dir < 8; dir++){
        for(int ind = 0; ind < kRays[this->kingPos_.pos][dir].size(); ind++){
            ChessPos &pos = kRays[this->kingPos_.pos][dir][ind];
            if(this->board_->hasPieceAtPos(pos)){
                if(
                    this->board_->piece(pos).player() == this->opponent_ &&
                    (
                        this->board_->piece(pos).type() == kPieceQueen ||
                        this->board_->piece(pos).type() == kPieceBishop
                    )
                ){
                    if(this->hasForced_){
                        this->cannotMove_ = true;
                        return;
                    } else {
                        this->hasForced_ = true;
                        for(int indToAdd = 0; indToAdd <= ind; indToAdd++ ) this->forced_[kRays[this->kingPos_.pos][dir][indToAdd].pos] = true;
                    }
                }
                break;
            }
        }
    }

}

MoveGenerator::MoveGenerator() {
    return;
}

bool MoveGenerator::stalemate() {
    if(this->getMoves().empty() && !this->inCheck()) return true;
    if(this->board_->movesSinceLastCapture() >= 100) return true;

    return false;
}
bool MoveGenerator::stalemate(ChessBoard &board) {
    this->setBoard(board);
    return this->stalemate();
}

bool MoveGenerator::fiftyMoveRuleStalemate() const {
    return this->board_->movesSinceLastCapture() >= 100;
}
bool MoveGenerator::fiftyMoveRuleStalemate(ChessBoard &board) const {
    return board.movesSinceLastCapture() >= 100;
}

bool MoveGenerator::inCheck() const {
    return this->hasForced_;
}
bool MoveGenerator::inCheck(ChessBoard &board) {
    this->setBoard(board);
    return this->hasForced_;
}

bool MoveGenerator::hasLost() const {
    return (this->inCheck() && this->getMoves().empty());
}
bool MoveGenerator::hasLost(ChessBoard &board) {
    this->setBoard(board);
    return this->hasLost();
}

vector<ChessMove> MoveGenerator::getMoves() const {
    //std::cout << "Generating moves for board:" << std::endl;
    //std::cout << this->board_->pieceNum() << std::endl;
    //this->board_->printBoard();

    vector<ChessMove> moves;

    // for(int i = 0; i < this->board_->pieceNum(); i++){
    //     ChessPos pos = this->board_->piecePositions()[i];
    //     ChessPiece piece = this->board_->piece(pos);
    //     if(piece.player() == this->player_) this->genMovesForPiece(moves, piece, pos);
    // }

    // if(this->hasForced_){
    //     //std::cout << "In check!" << std::endl;
    //     vector<ChessMove> filteredMoves;
    //     for(ChessMove move : moves){
    //         if(move.piece.type() == kPieceKing) filteredMoves.push_back(move);
    //         else if(!this->cannotMove_ && this->forced_[move.newPos.pos]) filteredMoves.push_back(move);
    //     }

    //     moves = filteredMoves;
    // }

    // // std::cout << "Moves generated: ";
    // // for(ChessMove move : moves) std::cout << move.str() << ", ";
    // // std::cout << std::endl;

    //If in check, generate the attack table

    this->genKingMoves(moves);

    if(this->cannotMove_) return moves;

    this->genPawnMoves(moves);
    this->genKnightMoves(moves);
    this->genSlidingMoves(moves);

    return moves;
}
vector<ChessMove> MoveGenerator::getMoves(ChessBoard &board) {
    this->setBoard(board);
    return this->getMoves();
}

/*const vector<ChessMove> &MoveGenerator::getMoves() const {

    if(!MoveGenerator::knownBoards_.count(this->board_->key())){

        //std::cout << "Generating moves for board:" << std::endl;
        //std::cout << this->board_->pieceNum() << std::endl;
        //this->board_->printBoard();

        vector<ChessMove> moves;

        for(int i = 0; i < this->board_->pieceNum(); i++){
            ChessPos pos = this->board_->piecePositions()[i];
            ChessPiece piece = this->board_->piece(pos);
            if(piece.player() == this->player_) this->genMovesForPiece(moves, piece, pos);
        }

        if(this->hasForced_){
            //std::cout << "In check!" << std::endl;
            vector<ChessMove> filteredMoves;
            for(ChessMove move : moves){
                if(move.piece.type() == kPieceKing) filteredMoves.push_back(move);
                else if(!this->cannotMove_ && this->forced_[move.newPos.pos]) filteredMoves.push_back(move);
            }

            moves = filteredMoves;
        }

        // std::cout << "Moves generated: ";
        // for(ChessMove move : moves) std::cout << move.str() << ", ";
        // std::cout << std::endl;

        MoveGenerator::knownBoards_.emplace(this->board_->key(),moves);

    }

    return MoveGenerator::knownBoards_.at(this->board_->key());
    
}
const vector<ChessMove> &MoveGenerator::getMoves(ChessBoard &board) {
    if(!MoveGenerator::knownBoards_.count(board.key())) {
        this->setBoard(board);
        return this->getMoves();
    }
    else return MoveGenerator::knownBoards_.at(board.key());
}*/

void MoveGenerator::setBoard(ChessBoard &board) {

    this->board_ = &board;
    this->player_ = board.player();
    this->opponent_ = board.opponent();
    this->kingPos_ = board.kingPos(this->player_);
    this->doEnPassantCheck_ = board.enPassantFile() >= 0;

    //std::cout << this->kingPos_.str() << std::endl;

    this->setAttacked();
    this->setPinned();
    this->setForced();
}


void MoveGenerator::printAttacked() const {

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

void MoveGenerator::printForced() const {

    using std::cout;
    using std::endl;

    int pos;

    for(int rank = 7; rank >= 0; rank--){
        cout << rank + 1 << ' ';
        for(int file = 0; file < 8; file++){
            pos = rank * 8 + file;
            if(this->forced_[pos]) cout << 'X';
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