#ifndef CHESS_H
#define CHESS_H

#include "Error.h"
#include "raylib.h"

enum ChessPieceType {
	CHESS_PIECE_NONE = 0,
	CHESS_PIECE_PAWN = 1,
	CHESS_PIECE_BISHOP = 2,
	CHESS_PIECE_KNIGHT = 3,
	CHESS_PIECE_ROOK = 4,
	CHESS_PIECE_QUEEN = 5,
	CHESS_PIECE_KING = 6,
	CHESS_PIECE_MAX = CHESS_PIECE_KING,
};

/**
 * Gets the human readable name (eg. "Bishop")
 */
const char* ChessPieceType_MGetName(enum ChessPieceType);
const char* ChessPieceType_MGetShortName(enum ChessPieceType);

enum ChessSide {
	CHESS_WHITE,
	CHESS_BLACK,
};

//TODO: Bitfield here is premature optimization. Check performance.
struct ChessPiece {
	enum ChessPieceType type : 7;
	enum ChessSide side : 1;
};

#define DEFAULT_CHESS_BOARD_WIDTH 8
#define DEFAULT_CHESS_BOARD_HEIGHT 8

struct ChessBoard {
	int width, height;
	/**
	 * Each cell of the chess board,
	 * where pieces[0][0] is A1
	 * and pieces[1][0] is A2
	 */
	struct ChessPiece* pieces;
};

void ChessBoard_MInitialize(struct ChessBoard*, int w, int h);

struct ChessPiece* ChessBoard_MGetPiece(struct ChessBoard*, int r, int c);

/**
 * Sets the default starting layout. Requires the board
 * to be 8x8
 */
enum Error ChessBoard_MInitializeDefaultStartingPieces(struct ChessBoard*);
void ChessBoard_MDestruct(struct ChessBoard*);

/**
	* Gets the human-readable name (eg. C4) from a cell index
	*/
const char* ChessBoard_SGetCellName(int r, int c);

struct ChessLayout {
	Rectangle board;
	int cellWidth;
	int cellGap;
};

void ChessLayout_MRecalculate(struct ChessLayout*, struct ChessBoard*);

struct Chess {
	struct ChessBoard board;
	struct ChessLayout layout;
	enum ChessSide turn;
};

void Chess_MInitializeDefault(struct Chess*);

void Chess_MDraw(struct Chess*);

void Chess_MDestruct(struct Chess*);

#endif // CHESS_H
