#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <algorithm>

#include "board.h"
#include "move_generator.h"

void rotate_dir(char &col, char &row){
    char tmp = col;
    col = -row;
    row = tmp;
}
bool compare_moves(const ChessMove &a, const ChessMove &b) {
    return a.score > b.score;
}

static const array<PieceType, 4> kPossiblePromotions = {kPieceRook, kPieceBishop, kPieceKnight, kPieceQueen};

void MoveGenerator::setKingPos(){
    for(auto iter : this->board_->pieces()){
        if(iter.second.player == this->player_ && iter.second.pieceType == kPieceKing){
            this->kingPos_ = iter.first;
            break;
        }
    }
}

vector<pair<ChessPos,ChessPiece>> MoveGenerator::piecesInDir(ChessPos start, char dCol, char dRow) const{
    vector<pair<ChessPos,ChessPiece>> pieces;

    for(ChessPos pos : positions_in_ray(start, dCol, dRow)){
        if(this->board_->hasPiece(pos)) pieces.push_back(pair<ChessPos,ChessPiece>(pos,this->board_->piece(pos)));
    }

    return pieces;
}
ChessPosSet MoveGenerator::untilFirstInDir(ChessPos start, char dCol, char dRow, bool countFirst) const {
    ChessPosSet positions;
    start.col += dCol;
    start.row += dRow;

    while(start.isInBounds() && !this->board_->hasPiece(start)){
        positions.insert(start);
        start.col += dCol;
        start.row += dRow;
    }

    if(start.isInBounds() && countFirst) positions.insert(start);

    return positions;
}  
//This function is pretty simple, but it also sets the move's capture value, so it's pretty useful
//It also checks for en passant (holy hell)
bool MoveGenerator::willMoveCapture(ChessMove &move) const{
    
    if(move.piece.pieceType == kPiecePawn){
        ChessPos pos = move.newPos;
        if(this->player_ == kPlayerWhite) pos.row--;
        else pos.row++;

        if(
            pos.isInBounds() &&
            this->board_->hasPiece(pos) &&
            this->board_->piece(pos).pieceType == kPiecePawn &&
            this->board_->piece(pos).moveNum == 1 &&
            this->board_->piece(pos).player == this->opponent_
        ) {
            move.capture = this->board_->piece(pos);
            move.isEnPassant = true;

            return enPassantCheck(move);
        }
    }

    if(this->board_->hasPiece(move.newPos) && this->board_->piece(move.newPos).player == this->opponent_){
        move.capture = this->board_->piece(move.newPos);
        return true;
    }
    return false;
}
void MoveGenerator::addAttacksInDir(ChessPos pos, char dCol, char dRow) {
    pos.col += dCol;
    pos.row += dRow;
    while(pos.isInBounds() && (!this->board_->hasPiece(pos) || this->board_->piece(pos).player == this->player_ && this->board_->piece(pos).pieceType == kPieceKing)){
        this->attacked_.insert(pos);
        pos.col += dCol;
        pos.row += dRow;
    }

    //if(newPos.isInBounds() && this->pieces_.at(newPos).player != this->pieces_.at(pos).player)  positions.insert(newPos);
    if(pos.isInBounds()) this->attacked_.insert(pos);
}
//Add all the moves in the direction specified by dCol and dRow
void MoveGenerator::addMovesInDir(vector<ChessMove> &moves, ChessPiece piece, ChessPos startPos, char dCol, char dRow) const{
    ChessMove newMove = ChessMove(piece, startPos, ChessPos(startPos.col + dCol, startPos.row + dRow));

    while(newMove.isInBounds() && !this->board_->hasPiece(newMove.newPos)){
        moves.push_back(newMove);
        newMove.newPos.row += dRow;
        newMove.newPos.col += dCol;
    }

    if(newMove.isInBounds() && willMoveCapture(newMove)) moves.push_back(newMove);
}
void MoveGenerator::setMoveScore(ChessMove &move) const {

    if(move.capture.pieceType != kPieceNone) move.score += 10 * kPieceValue[move.capture.pieceType] - kPieceValue[move.piece.pieceType];

    if(move.isPromoting) move.score += kPieceValue[move.piece.pieceType];

    if(this->attacked_.count(move.newPos)) move.score -= kPieceValue[move.piece.pieceType];
}

//Get all spots attacked by a piece at pos
void MoveGenerator::addPieceAttacks(ChessPos pos, ChessPiece piece){

    if(!this->board_->pieces().count(pos)) return;
    
    char dRow, dRow1, dCol, dCol1, tmp;

    ChessMove newMove;
    ChessPos newPos;

    switch(piece.pieceType){
        case kPiecePawn:
        default:

            dRow = 1;
            if(piece.player == kPlayerBlack) dRow = -1; //If black, go other direction

            //First, check if you can capture anything
            //TODO: check for en passant (holy hell)

            newPos = ChessPos(pos.col - 1, pos.row + dRow);
            if(newPos.isInBounds()) this->attacked_.insert(newPos);
            newPos.col += 2;
            if(newPos.isInBounds()) this->attacked_.insert(newPos);
            
        break;

        //The rook, bishop, and queen all use basically the same algorithm found in addMovesInDir

        case kPieceRook:

        dRow = 1;
        dCol = 0;

        for(int x = 0; x < 4; x++){
            this->addAttacksInDir(pos, dCol, dRow);
            
            tmp = dRow;
            dRow = -dCol;
            dCol = tmp;
        }

        break;

        case kPieceBishop:
        dRow = 1;
        dCol = 1;
        for(int x = 0; x < 4; x++){
            this->addAttacksInDir(pos, dCol, dRow);
            
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
            this->addAttacksInDir(pos, dCol, dRow);
            this->addAttacksInDir(pos, dCol1, dRow1);
            
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
            if(newPos.isInBounds()) this->attacked_.insert(newPos);
            newPos = ChessPos(pos.col + dRow, pos.row + dCol);
            //std::cout << newMove.newPos.str() << std::endl;
            //if(this->pieces_.count(newMove.newPos)) std::cout << this->pieces_[newMove.newPos].pieceChar() <<std::endl;
            if(newPos.isInBounds())  this->attacked_.insert(newPos);
            
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
            if(newPos.isInBounds()) this->attacked_.insert(newPos);
            newPos = ChessPos(pos.col + dCol1, pos.row + dRow1);
            if(newPos.isInBounds()) this->attacked_.insert(newPos);
            
            tmp = dRow;
            dRow = -dCol;
            dCol = tmp;
            tmp = dRow1;
            dRow1 = -dCol1;
            dCol1 = tmp;
        }

        break;
    }

    // std::cout << piece.pieceChar() << ' ';
    // for(ChessPos position : positions){
    //     std::cout << position.str() << ", ";
    // }
    // std::cout << std::endl;
}
vector<ChessMove> MoveGenerator::addPieceMoves(ChessPos pos, ChessPiece piece) const{
    vector<ChessMove> moves;

    //If pinned, can't do any moves
    if(this->pinned_.count(pos)) return moves;
    
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

                if(newMove.isInBounds() && willMoveCapture(newMove)) movesToAdd.push_back(newMove);
            }

            newMove = ChessMove(piece, pos, ChessPos(pos.col, pos.row + dRow));
            if(newMove.isInBounds() && !this->board_->hasPiece(newMove.newPos)) {
                movesToAdd.push_back(newMove);
                doCheck = true;
            }

            //Then, see if you can move two spaces
            if(doCheck && !piece.moveNum){
                newMove = ChessMove(piece, pos, ChessPos(pos.col, pos.row + dRow*2));
                newMove.isEnPassantEligible = true;
                if(newMove.isInBounds() && !this->board_->hasPiece(newMove.newPos)) movesToAdd.push_back(newMove);
            }

            for(ChessMove move : movesToAdd){
                if((piece.player == kPlayerWhite && move.newPos.row == 8) || (piece.player == kPlayerBlack && move.newPos.row == 1)){
                    for(PieceType promotionType : kPossiblePromotions){
                        newMove = ChessMove(piece, move.pos, move.newPos);
                        newMove.isPromoting = true;     
                        newMove.piece.pieceType = promotionType;               
                        moves.push_back(newMove);
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
            this->addMovesInDir(moves, piece, pos, dCol, dRow);
            
            rotate_dir(dCol, dRow);

        }

        break;

        case kPieceBishop:
        dRow = 1;
        dCol = 1;
        for(int x = 0; x < 4; x++){
            this->addMovesInDir(moves, piece, pos, dCol, dRow);
            
            rotate_dir(dCol, dRow);

        }
        break;

        case kPieceQueen:

        dRow = 1;
        dCol = 1;
        dRow1 = 1;
        dCol1 = 0;

        for(int x = 0; x < 4; x++){
            this->addMovesInDir(moves, piece, pos, dCol, dRow);
            this->addMovesInDir(moves, piece, pos, dCol1, dRow1);
            
            rotate_dir(dCol, dRow);
            rotate_dir(dCol1, dRow1);
        }

        break;

        //Knight and king are similar to addMovesInDir, but instead of looping we just do it once
        case kPieceKnight:
        dRow = 1;
        dCol = 2;

        for(int x = 0; x < 4; x++){
            newMove = ChessMove(piece, pos, ChessPos(pos.col + dCol, pos.row + dRow));
            //std::cout << newMove.newPos.str() << std::endl;
            if(newMove.isInBounds() && (!this->board_->hasPiece(newMove.newPos) || willMoveCapture(newMove))) moves.push_back(newMove);
            newMove = ChessMove(piece, pos, ChessPos(pos.col + dRow, pos.row + dCol));
            //std::cout << newMove.newPos.str() << std::endl;
            //if(this->pieces_.count(newMove.newPos)) std::cout << this->pieces_[newMove.newPos].pieceChar() <<std::endl;
            if(newMove.isInBounds() && (!this->board_->hasPiece(newMove.newPos) || willMoveCapture(newMove))) moves.push_back(newMove);
            
            rotate_dir(dCol, dRow);
        }

        break;

        case kPieceKing:

        dRow = 1;
        dCol = 1;
        dRow1 = 1;
        dCol1 = 0;

        for(int x = 0; x < 4; x++){
            newMove = ChessMove(piece, pos, ChessPos(pos.col + dCol,pos.row + dRow));
            if(
                newMove.isInBounds() 
                && (!this->board_->hasPiece(newMove.newPos) || this->willMoveCapture(newMove))
                && !this->attacked_.count(newMove.newPos)
            ) moves.push_back(newMove);
            newMove = ChessMove(piece, pos, ChessPos(pos.col + dCol1,pos.row + dRow1));
            if(
                newMove.isInBounds() 
                && (!this->board_->hasPiece(newMove.newPos) || this->willMoveCapture(newMove))
                && !this->attacked_.count(newMove.newPos)
            ) moves.push_back(newMove);

            rotate_dir(dCol, dRow);
            rotate_dir(dCol1, dRow1);
        }

        //Check if you can castle
        if(!piece.moveNum && pos.col == 'e'){
            char row = (piece.player == kPlayerWhite) ? 1 : 8;

            //Queenside
            if(
                this->board_->hasPiece(ChessPos('a',row)) 
                && !this->board_->piece(ChessPos('a',row)).moveNum 
                && !this->board_->hasPiece(ChessPos('b',row)) 
                && !this->board_->hasPiece(ChessPos('c',row)) 
                && !this->board_->hasPiece(ChessPos('d',row))
                && !this->attacked_.count(ChessPos('b',row))
            ) {
                newMove = ChessMove(piece, pos, ChessPos('b',row));
                newMove.isCastling = true;
                newMove.castlingSide = false;
                moves.push_back(newMove);
            }
            //Kingside
            if(
                this->board_->hasPiece(ChessPos('h',row)) 
                && !this->board_->piece(ChessPos('h',row)).moveNum 
                && !this->board_->hasPiece(ChessPos('g',row)) 
                && !this->board_->hasPiece(ChessPos('f',row)) 
                && !this->attacked_.count(ChessPos('g',row))
            ){ 
                newMove = ChessMove(piece, pos, ChessPos('g',row));
                newMove.isCastling = true;
                newMove.castlingSide = true;
                moves.push_back(newMove);
            }
        }

        break;
    }

    return moves;
}
//If nothing is returned, then assume that there is more than one threat, and therefore no move could block both pieces
ChessPosSet MoveGenerator::forcedPositions() const {

    ChessPosSet forced;
    ChessPosSet emptySet;

    char dCol, dRow, dCol1, dRow1, tmp;
    bool foundAttacker;
    vector<pair<ChessPos,ChessPiece>> piecePositions;
    ChessPiece piece;

    dCol = 1;
    dRow = 0;
    dCol1 = 1;
    dRow1 = 1;
    foundAttacker = false;

    //First, do pawn checks because they're really annoying
    ChessPos pawnPos = this->kingPos_;
    pawnPos.col--;
    pawnPos.row++;
    if(this->player_ == kPlayerBlack) pawnPos.row -= 2;
    
    if(pawnPos.isInBounds() && this->board_->hasPiece(pawnPos)){
        if(this->board_->piece(pawnPos).player == this->opponent_ && this->board_->piece(pawnPos).pieceType == kPiecePawn){
            foundAttacker = true;
            forced.insert(pawnPos);
        }
    }

    pawnPos.col += 2;
    if(pawnPos.isInBounds() && this->board_->hasPiece(pawnPos)){
        if(this->board_->piece(pawnPos).player == this->opponent_ && this->board_->piece(pawnPos).pieceType == kPiecePawn){
            if(!foundAttacker){
                foundAttacker = true;
                forced.insert(pawnPos);
            } else {
                return emptySet;
            }
        }
    }
    //Then, look for knights

    dCol = 1;
    dRow = 2;

    for(int i = 0; i < 4; i++){
        ChessPos knightPos = this->kingPos_;
        knightPos.row += dRow;
        knightPos.col += dCol;
        if(knightPos.isInBounds() && this->board_->hasPiece(knightPos) && this->board_->piece(knightPos).player == this->opponent_ && this->board_->piece(knightPos).pieceType == kPieceKnight){
            if(!foundAttacker){
                foundAttacker = true;
                forced.insert(knightPos);
            } else {
                return emptySet;
            }
        }
        knightPos.row -= dRow;
        knightPos.col -= dCol;

        knightPos.row += dCol;
        knightPos.col += dRow;
        if(knightPos.isInBounds() && this->board_->hasPiece(knightPos) && this->board_->piece(knightPos).player == this->opponent_ && this->board_->piece(knightPos).pieceType == kPieceKnight){
            if(!foundAttacker){
                foundAttacker = true;
                forced.insert(knightPos);
            } else {
                return emptySet;
            }
        }
    }

    dCol = 1;
    dRow = 0;

    //Finally, look for queens, rooks, and bishops
    for(int i = 0; i < 4; i++){

        //Is there a queen or rook vertically/horizontally?
        piecePositions = this->piecesInDir(this->kingPos_,dCol,dRow);
        if(
            !piecePositions.empty()
            && piecePositions[0].second.player == this->opponent_
            && (piecePositions[0].second.pieceType == kPieceQueen || piecePositions[0].second.pieceType == kPieceRook)
        ) {
            if(!foundAttacker){
                foundAttacker = true;
                forced = untilFirstInDir(this->kingPos_, dCol, dRow);
            } else { //If you've already found a threat, then don't return anything
                return emptySet;
            }
        }

        //Is there a queen or bishop diagonally?
        piecePositions = this->piecesInDir(this->kingPos_,dCol1,dRow1);
        if(
            !piecePositions.empty()
            && piecePositions[0].second.player == this->opponent_
            && (piecePositions[0].second.pieceType == kPieceQueen || piecePositions[0].second.pieceType == kPieceBishop)
        ) {
            if(!foundAttacker){
                foundAttacker = true;
                forced = untilFirstInDir(this->kingPos_, dCol1, dRow1);
            } else { //If you've already found a threat, then don't return anything
                return emptySet;
            }
        }

        rotate_dir(dCol, dRow);
        rotate_dir(dCol1, dRow1);
    }

    return forced;
}
bool MoveGenerator::enPassantCheck(ChessMove move) const {
    char dCol, dRow;
    vector<pair<ChessPos,ChessPiece>> piecePositions;

    dCol = 1;

    for(int dCol = -1; dCol < 2; dCol += 2){
        piecePositions = this->piecesInDir(this->kingPos_,dCol, dRow);

        if(
            piecePositions.size() > 2 &&
            piecePositions[0].first == move.pos &&
            piecePositions[1].second == move.capture &&
            piecePositions[2].second.player == this->opponent_ &&
            (piecePositions[2].second.pieceType == kPieceRook || piecePositions[2].second.pieceType == kPieceQueen)
        ) return false;
    }

    return true;
}

void MoveGenerator::setAttacked() {
    // for(char col = 'a'; col <= 'h'; col++){
    //     for(char row = 1; row <= 8; row++){
    //         this->attacked_.at(ChessPos(col,row)).clear();
    //     }
    // }
    this->attacked_.clear();

    for(auto pieceData : this->board_->pieces()){
        if(pieceData.second.player == this->opponent_) this->addPieceAttacks(pieceData.first, pieceData.second);
    }
}
void MoveGenerator::setPinned() {

    this->pinned_.clear();

    char dCol, dRow, dCol1, dRow1, tmp;
    vector<pair<ChessPos,ChessPiece>> piecePositions;
    ChessPiece piece;

    dCol = 1;
    dRow = 0;
    dCol1 = 1;
    dRow1 = 1;
    for(int i = 0; i < 4; i++){

        //First check in the vertical/horizontal direction for rooks or queens
        piecePositions = this->piecesInDir(this->kingPos_,dCol,dRow);
        if(
            piecePositions.size() > 1
            && piecePositions[0].second.player == this->player_
            && piecePositions[1].second.player == this->opponent_
            && (piecePositions[1].second.pieceType == kPieceQueen || piecePositions[1].second.pieceType == kPieceRook)
        ) this->pinned_.insert(piecePositions[0].first);

        //Then, check the diagonal direction for bishops or queens
        piecePositions = this->piecesInDir(this->kingPos_,dCol1,dRow1);
        if(
            piecePositions.size() > 1
            && piecePositions[0].second.player == this->player_
            && piecePositions[1].second.player == this->opponent_
            && (piecePositions[1].second.pieceType == kPieceQueen || piecePositions[1].second.pieceType == kPieceBishop)
        ) this->pinned_.insert(piecePositions[0].first);

        rotate_dir(dCol, dRow);
        rotate_dir(dCol1, dRow1);
    }

}

MoveGenerator::MoveGenerator(){}
MoveGenerator::MoveGenerator(ChessBoard &board){
    this->setBoard(board);
}

void MoveGenerator::setBoard(ChessBoard &board){
    this->board_= &board;
    this->player_ = this->board_->player();
    this->opponent_ = this->board_->opponent();

    this->setKingPos();
    this->setAttacked();
    this->setPinned();
}

bool MoveGenerator::inCheck() const{
    return this->attacked_.count(this->kingPos_);
}

bool MoveGenerator::inCheck(ChessBoard &board) {
    this->setBoard(board);

    return this->inCheck();
}

bool MoveGenerator::hasLost() const {
    return this->getMoves().empty() && !this->stalemate();
}

bool MoveGenerator::hasLost(ChessBoard &board) {
    this->setBoard(board);
    return this->hasLost();
}
bool MoveGenerator::stalemate() const{
    //TODO: add more situations

    bool noPiecesCaptured = true;
    vector<ChessMove> moves = this->board_->moves();

    if(this->board_->seenBoards().count(this->board_->zobristKey()) && this->board_->seenBoards().at(this->board_->zobristKey()) >= 3) return true;

    if(moves.size() > 100){
        for(int i = 0; i < 100; i++){
            if(moves[moves.size()-1-i].capture.pieceType != kPieceNone || moves[moves.size()-1-i].piece.pieceType == kPiecePawn) return false;
        }
        return true;
    }

    return false;
}
bool MoveGenerator::stalemate(ChessBoard &board) {
    this->setBoard(board);

    return this->stalemate();
}

const ChessPosSet &MoveGenerator::pinned() const {
    return this->pinned_;
}
const ChessPosSet &MoveGenerator::attacked() const {
    return this->attacked_;
}
ChessPosSet MoveGenerator::forced() const {
    return this->forcedPositions();
}

vector<ChessMove> MoveGenerator::getMoves() const{

    vector<ChessMove> moves, filteredMoves;

    if(this->stalemate()) return moves;

    for(auto iter : this->board_->pieces()){
        if(iter.second.player == this->player_){
            vector<ChessMove> movesToAdd = this->addPieceMoves(iter.first, iter.second);
            moves.insert(moves.end(), movesToAdd.begin(), movesToAdd.end());
        }
    }


    if(this->inCheck()){
        ChessPosSet forced = this->forcedPositions();
        for(ChessMove move : moves){
            if(move.piece.pieceType == kPieceKing || forced.count(move.newPos)) filteredMoves.push_back(move);
        }
    }
    else filteredMoves = moves;
    
    // ChessBoard newBoard = ChessBoard(*this);
    // for(ChessMove move : moves){
    //     //std::cout << move.str() << std::endl;
    //     newBoard.doMove(move, false);
    //     //newBoard.printBoard();
    //     if(!newBoard.inCheck(player)) filteredMoves.push_back(move);
    //     newBoard.undoMove(move, false);
    //     //newBoard.printBoard();
    // }

    return filteredMoves;
}
vector<ChessMove> MoveGenerator::getMoves(ChessBoard &board){
    this->setBoard(board);

    return this->getMoves();
}

vector<ChessMove> MoveGenerator::getMovesOrdered() const{

    using std::sort;

    vector<ChessMove> orderedMoves = this->getMoves();
    for(ChessMove &move : orderedMoves) this->setMoveScore(move);

    sort(orderedMoves.begin(), orderedMoves.end(), compare_moves);

    return orderedMoves;
}
vector<ChessMove> MoveGenerator::getMovesOrdered(ChessBoard &board){
    this->setBoard(board);

    return this->getMovesOrdered();
}


void MoveGenerator::printAttacked() const {
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

            if(this->attacked_.count(pos)) printChar = 'X';
            else printChar = ((row + col) % 2 ? '#' : ' ');
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
}