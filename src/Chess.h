#ifndef CHESS_H
#define CHESS_H

#include "Error.h"
#include "raylib.h"
#include <stdint.h>

struct IPoint {
	int x, y;
};

typedef uint8_t ChessPieceType;
#define CHESS_PIECE_NONE 0
#define CHESS_PIECE_PAWN 1
#define CHESS_PIECE_BISHOP 2
#define CHESS_PIECE_KNIGHT 3
#define CHESS_PIECE_ROOK 4
#define CHESS_PIECE_QUEEN 5
#define CHESS_PIECE_KING 6
#define CHESS_PIECE_MAX CHESS_PIECE_KING

/**
 * Gets the human readable name (eg. "Bishop")
 */
const char* ChessPieceType_MGetName(ChessPieceType);
const char* ChessPieceType_MGetShortName(ChessPieceType);

typedef uint8_t ChessSide;
#define CHESS_WHITE 0
#define CHESS_BLACK 1u

//TODO: Bitfield here is premature optimization. Check performance.
struct ChessPiece {
	ChessPieceType type : 7;
	ChessSide side : 1;
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
Error ChessBoard_MInitializeDefaultStartingPieces(struct ChessBoard*);
void ChessBoard_MDestruct(struct ChessBoard*);

/**
	* Gets the human-readable name (eg. C4) from a cell index
	*/
const char* ChessBoard_SGetCellName(int r, int c);

struct ChessLayout {
	int rows, cols;
	Rectangle board;
	int cellWidth;
	int cellGap;
};

void ChessLayout_MRecalculate(struct ChessLayout*, struct ChessBoard*);
Rectangle ChessLayout_MGetCellRect(struct ChessLayout*, int r, int c);
struct IPoint ChessLayout_MGetCellFromPoint(struct ChessLayout*, int x, int y);

struct Chess {
	struct ChessBoard board;
	struct ChessLayout layout;
	ChessSide side;
	ChessSide turn;

	struct IPoint hoveredCell;
};

struct IPoint Chess_MGetLocalCellFromGrid(struct Chess*, int r, int c);
struct IPoint Chess_MGetGridCellFromLocal(struct Chess*, int r, int c);

void Chess_MInitializeDefault(struct Chess*);

void Chess_MUpdate(struct Chess*);

void Chess_MDestruct(struct Chess*);

#endif // CHESS_H
