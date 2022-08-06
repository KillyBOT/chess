#include <iostream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "board.h"

static const array<PieceType, 4> kPossiblePromotions = {kPieceRook, kPieceBishop, kPieceKnight, kPieceQueen};
static const array<array<Player, 2>, 2> players = {kPlayerWhite,kPlayerBlack, kPlayerBlack, kPlayerWhite};

void rotate_dir(char &col, char &row){
    char tmp = col;
    col = -row;
    row = tmp;
}
Player opposite_player(Player player){
    if(player == kPlayerWhite) return kPlayerBlack;
    else if(player == kPlayerBlack) return kPlayerWhite;

    //you shouldn't ever see this

    return player;
}

ChessBoard::ChessBoard(bool fill, bool recordMoves){

    using std::pair;

    this->whiteKingPos_ = ChessPos('a',-1);
    this->blackKingPos_ = ChessPos('a',-1);

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

    //this->printAttackedBy(kPlayerWhite);
    //this->printAttackedBy(kPlayerBlack);

    //this->resetAttacked();
    this->updateState();
    this->updateHash();
    //this->setPinned();


}
ChessBoard::ChessBoard(const ChessBoard& oldBoard) {

    this->pieces_ = oldBoard.pieces_;
    this->pinned_ = oldBoard.pinned_;
    this->moves_ = oldBoard.moves_;
    this->whiteKingPos_ = oldBoard.whiteKingPos_;
    this->blackKingPos_ = oldBoard.blackKingPos_;
    this->attackedByWhite_ = oldBoard.attackedByWhite_;
    this->attackedByBlack_ = oldBoard.attackedByBlack_;
    this->whiteInCheck_ = oldBoard.whiteInCheck_;
    this->blackInCheck_ = oldBoard.blackInCheck_;
    this->hash_ = oldBoard.hash_;
    this->state_ = oldBoard.state_;
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
    return (this->moves_.size() >> 1) + 1;
}
Player ChessBoard::currentPlayer() const{
    return (this->moves_.size() & 1) ? kPlayerBlack : kPlayerWhite;
}
ChessMove ChessBoard::lastMove() const {
    return this->moves_.back();
}
ChessPos ChessBoard::kingPos(Player player) const {
    return (player == kPlayerWhite) ? this->whiteKingPos_ : this->blackKingPos_;
}
bool ChessBoard::hasPieceAtPos(ChessPos pos) const{
    if(!pos.isInBounds()) return false;
    return this->pieces_.count(pos);
}
ChessPiece ChessBoard::pieceAtPos(ChessPos pos) const {
    if(pos.isInBounds() && this->pieces_.count(pos)) return this->pieces_.at(pos);
    return ChessPiece();
}
Player ChessBoard::playerAtPos(ChessPos pos) const{
    if(pos.isInBounds() && this->hasPieceAtPos(pos)) return this->pieces_.at(pos).player;
    return kPlayerNone;
}
// bool ChessBoard::hasThreats(ChessPos pos) const {
//     return !this->attacked_.at(pos).empty();
// }

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
bool ChessBoard::inCheck(Player player) const{

    // if(player == kPlayerWhite) return !this->attacked_.at(this->whiteKingPos_).empty();
    // else return !this->attacked_.at(this->blackKingPos_).empty();

    // Player other = kPlayerBlack;
    // if(player == kPlayerBlack) other = kPlayerWhite;
    // else other = kPlayerBlack;

    // for(auto it : this->pieces_){
    //     if(it.second.player == other){
    //         for(ChessMove move : this->getMovesForPiece(it.first)){
    //             if(hasPieceAtPos(move.newPos) && this->pieces_.at(move.newPos).player == player && this->pieces_.at(move.newPos).pieceType == kPieceKing) return true;
    //         }
    //     }
    // }

    // return false;

    if(player == kPlayerWhite) return this->whiteInCheck_;
    else if(player == kPlayerBlack) return this->blackInCheck_;

    return false;

}
bool ChessBoard::hasWon(Player player) const{
    if(this->getMovesForPlayer(opposite_player(player)).empty()) return true;
    return false;
}
bool ChessBoard::stalemate() const{
    //TODO: add more situations

    bool noPiecesCaptured = true;

    if(this->moves_.size() > 100){
        for(int i = 0; i < 100; i++){
            if(this->moves_[this->moves_.size()-1-i].capture.pieceType != kPieceNone){
                noPiecesCaptured = false;
                break;
            }
        }
        if(noPiecesCaptured){
            return true;
        }
    }

    return false;
}
vector<ChessPos> ChessBoard::pinned() const {
    vector<ChessPos> pinned;

    for(ChessPos pinnedPiece : this->pinned_) pinned.push_back(pinnedPiece);

    return pinned;
}
std::size_t ChessBoard::hash() const {
    return this->hash_;
}
BoardState ChessBoard::state() const {
    if(this->inCheck(kPlayerWhite)){
        if(this->getMovesForPlayer(kPlayerWhite).empty()) return kStateBlackWin;
        else return kStateWhiteInCheck;
    }
    else if(this->inCheck(kPlayerBlack)) {
        if(this->getMovesForPlayer(kPlayerBlack).empty()) return kStateWhiteWin;
        else kStateBlackInCheck;
    }
    else if(this->stalemate()) return kStateStalemate;
    return kStateNone;
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
void ChessBoard::updateState(){
    //std::cout << "Updating state..." << std::endl;
    if(this->inCheck(kPlayerWhite)){
        if(this->getMovesForPlayer(kPlayerWhite).empty()) this->state_ = kStateBlackWin;
        else this->state_ = kStateWhiteInCheck;
    }
    else if(this->inCheck(kPlayerBlack)) {
        if(this->getMovesForPlayer(kPlayerBlack).empty()) this->state_ = kStateWhiteWin;
        else this->state_ = kStateBlackInCheck;
    }
    else if(this->stalemate()) this->state_ = kStateStalemate;
    else this->state_ = kStateNone;
}
void ChessBoard::setKingPos(){
    for(auto iter : this->pieces_){
        if(iter.second.pieceType == kPieceKing){
            if(iter.second.player == kPlayerWhite) this->whiteKingPos_ = iter.first;
            else this->blackKingPos_ = iter.first;
        }
    }
}

void ChessBoard::resetAttacked() {
    // for(char col = 'a'; col <= 'h'; col++){
    //     for(char row = 1; row <= 8; row++){
    //         this->attacked_.at(ChessPos(col,row)).clear();
    //     }
    // }
    this->attackedByWhite_.clear();
    this->attackedByBlack_.clear();

    for(auto pieceData : this->pieces_){
        this->addPieceAttacking(pieceData.first);
    }
}
void ChessBoard::resetAttackedForPlayer(Player player){
    if(player == kPlayerWhite) this->attackedByWhite_.clear();
    else this->attackedByBlack_.clear();
    for(auto pieceData : this->pieces_){
        if(pieceData.second.player == player) this->addPieceAttacking(pieceData.first);
    }
    
}
void ChessBoard::setPinned() {

    this->pinned_.clear();

    char dCol, dRow, dCol1, dRow1, tmp;
    vector<pair<ChessPos,ChessPiece>> piecePositions;
    array<pair<ChessPos,Player>, 2> kings = {pair<ChessPos,Player>(this->whiteKingPos_,kPlayerBlack), pair<ChessPos,Player>(this->blackKingPos_,kPlayerWhite)};
    ChessPiece piece;

    dCol = 1;
    dRow = 0;
    dCol1 = 1;
    dRow1 = 1;
    for(int i = 0; i < 4; i++){

        //First check in the vertical/horizontal direction for rooks or queens
        for(auto king : kings){
            piecePositions = this->piecesInDir(king.first,dCol,dRow);
            if(
                piecePositions.size() > 1
                && piecePositions[0].second.player != king.second
                && piecePositions[1].second.player == king.second 
                && (piecePositions[1].second.pieceType == kPieceQueen || piecePositions[1].second.pieceType == kPieceRook)
            ) this->pinned_.insert(piecePositions[0].first);

            //Then, check the diagonal direction for bishops or queens
            piecePositions = this->piecesInDir(king.first,dCol1,dRow1);
            if(
                piecePositions.size() > 1
                && piecePositions[0].second.player != king.second
                && piecePositions[1].second.player == king.second 
                && (piecePositions[1].second.pieceType == kPieceQueen || piecePositions[1].second.pieceType == kPieceBishop)
            ) this->pinned_.insert(piecePositions[0].first);
        }

        rotate_dir(dCol, dRow);
        rotate_dir(dCol1, dRow1);
    }

}

void ChessBoard::printBoard() const{

    using std::cout;
    using std::endl;

    ChessPos pos = ChessPos('a',8);
    cout << "Turn " << this->currentTurnNum() << endl;
    cout << this->playerScore(kPlayerWhite) << " | " << this->playerScore(kPlayerBlack) << endl;
    if(this->hasWon(kPlayerWhite)) cout << "White has won!" << endl;
    else if(this->hasWon(kPlayerBlack)) cout << "Black has won!" << endl;
    else if(this->stalemate()) cout << "Stalemate!" << endl;
    else if(this->inCheck(kPlayerWhite)) cout << "White in check!" << endl;
    else if(this->inCheck(kPlayerBlack)) cout << "Black in check!" << endl;

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
    cout << (this->currentPlayer() == kPlayerWhite ? "White" : "Black") << "\'s turn" << endl;
    cout << this->currentTurnNum() << ": ";
    if(this->moves_.size()) cout << this->lastMove().str();
    cout << endl << endl;


    // for(ChessMove move : getPossibleMoves()){
    //     cout << move.basicStr() << endl;
    // }
}
void ChessBoard::printAttacked() const {
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
}
// void ChessBoard::printAttackedDict() const {
//     using std::cout;
//     using std::endl;
//     for(auto iter : this->attacked_){
//         if(this->hasThreats(iter.first)){
//             cout << iter.first.str() << ": ";
//             for(ChessPos attackedBy : iter.second) cout << attackedBy.str() << ", ";
//             cout << std::endl;
//         }
//     }
// }
void ChessBoard::printMoves() const {
    using std::cout;
    using std::endl;
    for(int turn = 1; turn < this->currentTurnNum(); turn++) cout << turn << ": " << this->moves_.at((turn-1) * 2).str() << ' ' << this->moves_.at(((turn-1) * 2) + 1).str() << endl;

    cout << this->currentTurnNum() << ": " << endl;
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

    if(piece.pieceType == kPieceKing){
        if(piece.player == kPlayerWhite) this->whiteKingPos_ = pos;
        else this->blackKingPos_ = pos;
    }

    this->setPinned();
    this->resetAttacked();
    this->whiteInCheck_ = false;
    this->blackInCheck_ = false;
    if(this->attackedByBlack_.count(this->whiteKingPos_)) whiteInCheck_ = true;
    if(this->attackedByWhite_.count(this->blackKingPos_)) blackInCheck_ = true;

    this->updateState();

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

vector<pair<ChessPos,ChessPiece>> ChessBoard::piecesInDir(ChessPos start, char dCol, char dRow) const{
    vector<pair<ChessPos,ChessPiece>> pieces;

    for(ChessPos pos : positions_in_ray(start, dCol, dRow)){
        if(this->hasPieceAtPos(pos)) pieces.push_back(pair<ChessPos,ChessPiece>(pos,this->pieces_.at(pos)));
    }

    return pieces;
}
ChessPosSet ChessBoard::untilFirstInDir(ChessPos start, char dCol, char dRow, bool countFirst) const {
    ChessPosSet positions;
    start.col += dCol;
    start.row += dRow;

    while(start.isInBounds() && !this->hasPieceAtPos(start)){
        positions.insert(start);
        start.col += dCol;
        start.row += dRow;
    }

    if(start.isInBounds() && countFirst) positions.insert(start);

    return positions;
}  
bool ChessBoard::willMoveCapture(ChessMove &move) const{
    if(hasPieceAtPos(move.newPos) && this->pieces_.at(move.pos).player != this->pieces_.at(move.newPos).player){
        move.capture = this->pieces_.at(move.newPos);
        return true;
    }
    return false;
}
void ChessBoard::addPosInDir(ChessPosSet &positions, ChessPos pos, char dCol, char dRow) const {
    ChessPos newPos = ChessPos(pos);
    newPos.col += dCol;
    newPos.row += dRow;
    while(newPos.isInBounds() && !this->hasPieceAtPos(newPos)){
        positions.insert(newPos);
        newPos.col += dCol;
        newPos.row += dRow;
    }

    //if(newPos.isInBounds() && this->pieces_.at(newPos).player != this->pieces_.at(pos).player)  positions.insert(newPos);
    if(newPos.isInBounds()) positions.insert(newPos);
}
//Add all the moves in the direction specified by dCol and dRow
void ChessBoard::addMovesInDir(vector<ChessMove> &moves, ChessPiece piece, ChessPos startPos, char dCol, char dRow) const{
    ChessMove newMove = ChessMove(piece, startPos, ChessPos(startPos.col + dCol, startPos.row + dRow));

    while(newMove.isInBounds() && !hasPieceAtPos(newMove.newPos)){
        moves.push_back(newMove);
        newMove.newPos.row += dRow;
        newMove.newPos.col += dCol;
    }

    if(newMove.isInBounds() && willMoveCapture(newMove)) moves.push_back(newMove);
}

//Get all spots attacked by a piece at pos
ChessPosSet ChessBoard::getAttackedByPiece(ChessPos pos) const{
    ChessPosSet positions;

    if(!this->pieces_.count(pos)){
        return positions;
    }

    ChessPiece piece = this->pieces_.at(pos);
    
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
            if(newPos.isInBounds()) positions.insert(newPos);
            newPos.col += 2;
            if(newPos.isInBounds()) positions.insert(newPos);
            
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
            if(newPos.isInBounds() && (!this->hasPieceAtPos(newPos) || this->pieces_.at(newPos).player != piece.player)) positions.insert(newPos);
            newPos = ChessPos(pos.col + dRow, pos.row + dCol);
            //std::cout << newMove.newPos.str() << std::endl;
            //if(this->pieces_.count(newMove.newPos)) std::cout << this->pieces_[newMove.newPos].pieceChar() <<std::endl;
            if(newPos.isInBounds() && (!this->hasPieceAtPos(newPos) || this->pieces_.at(newPos).player != piece.player))  positions.insert(newPos);
            
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
            if(newPos.isInBounds() && (!this->hasPieceAtPos(newPos) || this->pieces_.at(newPos).player != piece.player)) positions.insert(newPos);
            newPos = ChessPos(pos.col + dCol1, pos.row + dRow1);
            if(newPos.isInBounds() && (!this->hasPieceAtPos(newPos) || this->pieces_.at(newPos).player != piece.player)) positions.insert(newPos);
            
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

    return positions;
}
vector<ChessMove> ChessBoard::getMovesForPiece(ChessPos pos) const{
    vector<ChessMove> moves;

    //If pinned, can't do any moves
    if(this->pinned_.count(pos)) return moves;
    
    char dRow, dRow1, dCol, dCol1, tmp;

    ChessMove newMove;
    bool doCheck = false;
    ChessPiece piece = this->pieces_.at(pos);

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
            if(doCheck && !piece.moveNum){
                newMove = ChessMove(piece, pos, ChessPos(pos.col, pos.row + dRow*2));
                if(newMove.isInBounds() && !hasPieceAtPos(newMove.newPos)) movesToAdd.push_back(newMove);
            }

            for(ChessMove move : movesToAdd){
                if((piece.player == kPlayerWhite && move.newPos.row == 8) || (piece.player == kPlayerBlack && move.newPos.row == 1)){
                    for(PieceType promotionType : kPossiblePromotions){
                        newMove = ChessMove(piece, move.pos, move.newPos);
                        newMove.isPromoting = true;     
                        newMove.piece.pieceType = promotionType;               
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
            
            rotate_dir(dCol, dRow);

        }

        break;

        case kPieceBishop:
        dRow = 1;
        dCol = 1;
        for(int x = 0; x < 4; x++){
            addMovesInDir(moves, piece, pos, dCol, dRow);
            
            rotate_dir(dCol, dRow);

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
            if(newMove.isInBounds() && (!hasPieceAtPos(newMove.newPos) || willMoveCapture(newMove))) moves.push_back(newMove);
            newMove = ChessMove(piece, pos, ChessPos(pos.col + dRow, pos.row + dCol));
            //std::cout << newMove.newPos.str() << std::endl;
            //if(this->pieces_.count(newMove.newPos)) std::cout << this->pieces_[newMove.newPos].pieceChar() <<std::endl;
            if(newMove.isInBounds() && (!hasPieceAtPos(newMove.newPos) || willMoveCapture(newMove))) moves.push_back(newMove);
            
            rotate_dir(dCol, dRow);
        }

        break;

        case kPieceKing:

        dRow = 1;
        dCol = 1;
        dRow1 = 1;
        dCol1 = 0;

        ChessBoard newBoard = *this;
        newBoard.removePiece(pos);
        newBoard.resetAttacked();

        for(int x = 0; x < 4; x++){
            newMove = ChessMove(piece, pos, ChessPos(pos.col + dCol,pos.row + dRow));
            if(
                newMove.isInBounds() 
                && (!this->hasPieceAtPos(newMove.newPos) || this->willMoveCapture(newMove))
                && (
                    (piece.player == kPlayerWhite && !newBoard.attackedByBlack_.count(newMove.newPos)) 
                    || (piece.player == kPlayerBlack && !newBoard.attackedByWhite_.count(newMove.newPos))
                )
            ) moves.push_back(newMove);
            newMove = ChessMove(piece, pos, ChessPos(pos.col + dCol1,pos.row + dRow1));
            if(
                newMove.isInBounds() 
                && (!this->hasPieceAtPos(newMove.newPos) || this->willMoveCapture(newMove))
                && (
                    (piece.player == kPlayerWhite && !newBoard.attackedByBlack_.count(newMove.newPos)) 
                    || (piece.player == kPlayerBlack && !newBoard.attackedByWhite_.count(newMove.newPos))
                )
            ) moves.push_back(newMove);

            rotate_dir(dCol, dRow);
            rotate_dir(dCol1, dRow1);
        }

        //Check if you can castle
        if(piece.moveNum == 0 && pos.col == 'e'){
            char row = (piece.player == kPlayerWhite) ? 1 : 8;

            //Queenside
            if(
                hasPieceAtPos(ChessPos('a',row)) 
                && !this->pieces_.at(ChessPos('a',row)).moveNum 
                && !hasPieceAtPos(ChessPos('b',row)) 
                && !hasPieceAtPos(ChessPos('c',row)) 
                && !hasPieceAtPos(ChessPos('d',row))
            ) {
                newMove = ChessMove(piece, pos, ChessPos('b',row));
                newMove.isCastling = true;
                moves.push_back(newMove);
            }
            //Kingside
            if(
                hasPieceAtPos(ChessPos('h',row)) 
                && !this->pieces_.at(ChessPos('h',row)).moveNum 
                && !hasPieceAtPos(ChessPos('g',row)) 
                && !hasPieceAtPos(ChessPos('f',row)) 
            ){ 
                newMove = ChessMove(piece, pos, ChessPos('g',row));
                newMove.isCastling = true;
                moves.push_back(newMove);
            }
        }

        break;
    }

    return moves;
}
ChessPosSet ChessBoard::attackedByPlayer(Player player) const {
    ChessPosSet attacked;

    for(auto iter : this->pieces_){
        if(iter.second.player == player){
            for(ChessPos pos : this->getAttackedByPiece(iter.first)) attacked.insert(pos);
        }
    }

    return attacked;
}
//If nothing is returned, then assume that there is more than one threat, and therefore no move could block both pieces
unordered_set<ChessPos,ChessPosHash> ChessBoard::forcedNewPositions(Player player) const {

    ChessPosSet forced;

    char dCol, dRow, dCol1, dRow1, tmp;
    bool foundAttacker;
    vector<pair<ChessPos,ChessPiece>> piecePositions;
    ChessPos kingPos = this->kingPos(player);
    Player other = opposite_player(player);
    ChessPiece piece;

    dCol = 1;
    dRow = 0;
    dCol1 = 1;
    dRow1 = 1;
    foundAttacker = false;

    //First, do pawn checks because they're really annoying
    ChessPos pawnPos = kingPos;
    pawnPos.col--;
    pawnPos.row++;
    if(player == kPlayerBlack) pawnPos.row -= 2;
    
    piece = this->pieceAtPos(pawnPos);
    if(piece.player == other && piece.pieceType == kPiecePawn){
        foundAttacker = true;
        forced.insert(pawnPos);
    }

    pawnPos.col += 2;
    piece = this->pieceAtPos(pawnPos);
    if(piece.player == other && piece.pieceType == kPiecePawn){
        if(foundAttacker){
            forced.clear();
            return forced;
        } else {
            foundAttacker = true;
            forced.insert(pawnPos);
        }
    }

    for(int i = 0; i < 4; i++){

        piecePositions = this->piecesInDir(kingPos,dCol,dRow);
        if(
            !piecePositions.empty()
            && piecePositions[0].second.player == other
            && (piecePositions[0].second.pieceType == kPieceQueen || piecePositions[0].second.pieceType == kPieceRook)
        ) {
            if(!foundAttacker){
                foundAttacker = true;
                forced = untilFirstInDir(kingPos, dCol, dRow);
            } else { //If you've already found a threat, then don't return anything
                forced.clear();
                return forced;
            }
        }


        piecePositions = this->piecesInDir(kingPos,dCol1,dRow1);
        if(
            !piecePositions.empty()
            && piecePositions[0].second.player == other
            && (piecePositions[0].second.pieceType == kPieceQueen || piecePositions[0].second.pieceType == kPieceBishop)
        ) {
            if(!foundAttacker){
                foundAttacker = true;
                forced = untilFirstInDir(kingPos, dCol1, dRow1);
            } else { //If you've already found a threat, then don't return anything
                forced.clear();
                return forced;
            }
        }

        rotate_dir(dCol, dRow);
        rotate_dir(dCol1, dRow1);
    }

    return forced;
}

void ChessBoard::addPieceAttacking(ChessPos pos){
    //std::cout << "Adding attacks from position " << pos.str() << std::endl;
    //for(ChessPos attackedPos : this->getAttackedByPiece(pos)) this->attacked_.at(attackedPos).emplace(pos);

    if(this->pieceAtPos(pos).player == kPlayerWhite){
        for(ChessPos attackedPos : this->getAttackedByPiece(pos)) this->attackedByWhite_.insert(attackedPos);
    } else {
        for(ChessPos attackedPos : this->getAttackedByPiece(pos)) this->attackedByBlack_.insert(attackedPos);
    }
}
/*void ChessBoard::removePieceAttacking(ChessPos pos) {
    //std::cout << "Removing attacks from position " << pos.str() << std::endl;
    ChessPos attacked = ChessPos('a',1);
    for(int col = 0; col < 8; col++){
        attacked.row = 1;
        for(int row = 0; row < 8; row++){
            this->attacked_.at(attacked).erase(pos);
            attacked.row++;
        }
        attacked.col++;
    }
}
void ChessBoard::updatePieceAttacking(ChessPos pos){

    //this->printAttackedBy(kPlayerWhite);
    //this->printAttackedBy(kPlayerBlack);

    // for(ChessPos attackedBy : this->attacked_.at(pos)){
    //     std::cout << attackedBy.str_int() << ", ";
    // }
    // std::cout << std::endl;
    //std::cout << pos.isInBounds() << std::endl;
    this->removePieceAttacking(pos);
    //std::cout << "Successfully removed" << std::endl;
    this->addPieceAttacking(pos);
    //std::cout << "Successfully added" << std::endl;

    //this->printAttackedBy(kPlayerWhite);
    //this->printAttackedBy(kPlayerBlack);
}
*/
vector<ChessMove> ChessBoard::getMovesForPlayer(Player player) const{
    vector<ChessMove> moves, filteredMoves;

    for(auto pieceData : this->pieces_){
        if(pieceData.second.player == player){
            vector<ChessMove> movesToAdd = this->getMovesForPiece(pieceData.first);
            moves.insert(moves.end(), movesToAdd.begin(), movesToAdd.end());
        }
    }

    if(player == kPlayerWhite && this->whiteInCheck_){
        ChessPosSet forced = this->forcedNewPositions(kPlayerWhite);
        for(ChessMove move : moves){
            if(forced.count(move.newPos) || move.piece.pieceType == kPieceKing) filteredMoves.push_back(move);
        }
    }  else if (player == kPlayerBlack && this->blackInCheck_){
        ChessPosSet forced = this->forcedNewPositions(kPlayerBlack);
        for(ChessMove move : moves){
            if(forced.count(move.newPos) || move.piece.pieceType == kPieceKing ) filteredMoves.push_back(move);
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
vector<ChessMove> ChessBoard::getPossibleMoves() const{
    return this->getMovesForPlayer(this->currentPlayer());
}

//Both of these already assume that the muve is valid
void ChessBoard::doMove(ChessMove move, bool updateHash){

    //Since it's impossible to do a move that keeps you in check, set current check status to false
    if(move.piece.player == kPlayerWhite) this->whiteInCheck_ = false;
    else if(move.piece.player == kPlayerBlack) this->blackInCheck_ = false;

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
