#include "Chess.h"
#include "Error.h"
#include "Log.h"
#include "Const.h"

#include <stdlib.h>

const char* ChessPieceType_MGetName(enum ChessPieceType type){
	const char* names[] = {
		0,
		"Pawn",
		"Bishop",
		"Knight",
		"Rook",
		"Queen",
		"King",
	};
	return names[type];
}

const char* ChessPieceType_MGetShortName(enum ChessPieceType type){
	if(type > CHESS_PIECE_MAX) {
		CHESS_LOG_FATAL("INVALID PIECE PASSED TO ChessPiece_MGetShortName: %d\n", type);
		exit(1);
	}
	const char* names[] = {
		0,
		"P",
		"B",
		"N",
		"R",
		"Q",
		"K",
	};
	return names[type];
}

void ChessBoard_MInitialize(struct ChessBoard* b, int w, int h) {
	b->width = w;
	b->height = h;

	b->pieces = calloc(w * h, sizeof(struct ChessPiece));
}

void ChessBoard_MDestruct(struct ChessBoard* b) {
	free(b->pieces);
}

struct ChessPiece* ChessBoard_MGetPiece(struct ChessBoard* b, int r, int c){
	return &b->pieces[c + b->width * r];
}


enum Error ChessBoard_MInitializeDefaultStartingPieces(struct ChessBoard* b) {
	enum ChessPieceType edgePieces[] = {
		CHESS_PIECE_ROOK,
		CHESS_PIECE_KNIGHT,
		CHESS_PIECE_BISHOP,
		CHESS_PIECE_QUEEN,
		CHESS_PIECE_KING,
		CHESS_PIECE_BISHOP,
		CHESS_PIECE_KNIGHT,
		CHESS_PIECE_ROOK,
	};

	if (b->width != 8 || b->height != 8) {
		return ERROR_INVALID_SIZE;
	}
	for (int r = 0; r < 8; ++r) {
		for(int c = 0; c < 8; ++c){
			struct ChessPiece* piece = ChessBoard_MGetPiece(b, r, c);
			if(r < 2){
				piece->side = CHESS_WHITE;
			}
			else if (r > 5){
				piece->side = CHESS_BLACK;
			}

			if (r == 0){
				piece->type = edgePieces[c];
			}
			else if (r == 7){
				piece->type = edgePieces[7 - c];
			}
			else if (r == 1 || r == 6){
				piece->type = CHESS_PIECE_PAWN;
			}
		}
	}
	return ERROR_NONE;
}

const char* ChessBoard_SGetCellName(int r, int c) {
	return 0;
}

void ChessLayout_MRecalculate(struct ChessLayout* l, struct ChessBoard* b) {
	l->rows = b->height;
	l->cols = b->width;
	l->cellGap = 1;

	int paddingX = 16;
	int paddingY = 16;

	int w = GetScreenWidth() - 2 * paddingX;
	int h = GetScreenHeight() - 2 * paddingY;

	/* LAYOUT EQUATIONS
	* cellWidth * b->width + cellGap * (b->width - 1) <= w
	* cellWidth * b->height + cellGap * (b->height - 1) <= h
	*
	* cellWidth * b->width + cellGap * b->width - cellGap <= w
	* cellWidth * b->height + cellGap * b->height - cellGap <= h
	*
	* (cellWidth + cellGap) * b->width <= w + cellGap
	* (cellWidth + cellGap) * b->height <= h + cellGap
	*
	* cellWidth <= (w + cellGap)/b->width - cellGap
	* cellWidth <= (h + cellGap)/b->height - cellGap
	*/
	float aspect = (float) w / h;
	float boardAspect = (float) b->width / b->height;

	// cell width if the width is maximum
	int wMaxCellWidth = (w + l->cellGap)/b->width - l->cellGap;
	int hMaxCellWidth = (h + l->cellGap)/b->height - l->cellGap;

	l->cellWidth = MIN(wMaxCellWidth, hMaxCellWidth);

	l->board.width = l->cellWidth * b->width + l->cellGap * (b->width - 1);
	l->board.height = l->cellWidth * b->height + l->cellGap * (b->height - 1);

	l->board.y = paddingY + (h - l->board.height) / 2;
	l->board.x = paddingY + (w - l->board.width) / 2;

	CHESS_LOG_INFO("LAYOUT INFO:\n");
	CHESS_LOG_INFO("\tRender Size: %d x %d\n", w, h);
	CHESS_LOG_INFO("\tCell: %d\n", l->cellWidth);
}

void Chess_MInitializeDefault(struct Chess* c) {
	ChessBoard_MInitialize(&c->board, DEFAULT_CHESS_BOARD_WIDTH, DEFAULT_CHESS_BOARD_HEIGHT);
	enum Error e = ChessBoard_MInitializeDefaultStartingPieces(&c->board);
	if(!ERROR_OK(e)) {
		CHESS_LOG_FATAL("Could not initialize default chess board: %d", e);
	}

	ChessLayout_MRecalculate(&c->layout, &c->board);
}

Rectangle ChessLayout_MGetCellRect(struct ChessLayout* l, int r, int c) {
	int w = l->cellWidth;
	int x = l->board.x + (w + l->cellGap) * c;
	int y = l->board.y + (w + l->cellGap) * r;
	return (Rectangle) {
		x, y,
		w, w,
	};
}

struct IPoint ChessLayout_MGetCellFromPoint(struct ChessLayout* l, int x, int y) {
	// l->board
}

void Chess_MDraw(struct Chess* chess){
	BeginDrawing();
	ClearBackground(RAYWHITE);

	if(IsWindowResized()){
		ChessLayout_MRecalculate(&chess->layout, &chess->board);
	}

	for(int r = 0; r < 8; ++r){
		for(int c = 0; c < 8; ++c){
			Rectangle rect = ChessLayout_MGetCellRect(&chess->layout, chess->board.height - r - 1, c);
			DrawRectangleRec(rect, GRAY);

			struct ChessPiece* piece = ChessBoard_MGetPiece(&chess->board, r, c);
			if(piece->type == CHESS_PIECE_NONE) continue;

			const char* pieceName = ChessPieceType_MGetShortName(piece->type);
			struct Color pieceColor = piece->side == CHESS_WHITE ? WHITE : BLACK;

			DrawText(
				pieceName,
				rect.x, rect.y, 16, pieceColor
			);
		}
	}

	EndDrawing();
}

void Chess_MProcessInput(struct Chess* c){
	if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
		int mouseX = GetMouseX();
		int mouseY = GetMouseY();
	}
}

void Chess_MUpdate(struct Chess* c){
	Chess_MDraw(c);
	Chess_MProcessInput(c);
}

void Chess_MDestruct(struct Chess* c) {
	ChessBoard_MDestruct(&c->board);
}
