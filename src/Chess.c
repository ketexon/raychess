#include "Chess.h"
#include "Error.h"
#include "Log.h"
#include "Const.h"

#include <stdlib.h>

const char* ChessPieceType_MGetName(ChessPieceType type){
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

const char* ChessPieceType_MGetShortName(ChessPieceType type){
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


Error ChessBoard_MInitializeDefaultStartingPieces(struct ChessBoard* b) {
	ChessPieceType edgePieces[] = {
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
	// for cell r,c
	// the rect is
	// r.x: l->board.x + (cellGap + cellWidth) * c,
	// r.y: l->board.y + (cellGap + cellWidth) * r,
	// r.w,r.h: l->board.cellWidth
	// thus: r.x <= x <= r.x + r.w
	// thus: 0 <= x - board.x + (cellGap + cellWidth)*c <= cellWidth
	// thus: (board.x - x)/(cellGap + cellWidth)
	//			<= c
	//			<= (board.x - x + cellWidth)/(cellGap + cellWidth)
	// since c is an int, c = (board.x - x)/(cellGap + cellWidth)
	int c = (x - l->board.x)/(l->cellGap + l->cellWidth);
	int r = (y - l->board.y)/(l->cellGap + l->cellWidth);
	return (struct IPoint) {
		c, r
	};
}

struct IPoint Chess_MGetGridCellFromLocal(struct Chess *chess, int r, int c){
	struct IPoint out;

	out.x = chess->side == CHESS_WHITE ? c : (chess->board.width - c - 1);
	out.y = chess->side == CHESS_WHITE ? (chess->board.height - r - 1) : r;

	return out;
}

struct IPoint Chess_MGetLocalCellFromGrid(struct Chess *chess, int r, int c){
	// the transformation is symmetric
	return Chess_MGetGridCellFromLocal(chess, r, c);
}

void Chess_MInitializeDefault(struct Chess* c) {
	c->side = CHESS_BLACK;
	ChessBoard_MInitialize(&c->board, DEFAULT_CHESS_BOARD_WIDTH, DEFAULT_CHESS_BOARD_HEIGHT);
	Error e = ChessBoard_MInitializeDefaultStartingPieces(&c->board);
	if(!ERROR_OK(e)) {
		CHESS_LOG_FATAL("Could not initialize default chess board: %d", e);
	}

	ChessLayout_MRecalculate(&c->layout, &c->board);
}

void Chess_MDraw(struct Chess* chess){
	BeginDrawing();
	ClearBackground(RAYWHITE);

	if(IsWindowResized()){
		ChessLayout_MRecalculate(&chess->layout, &chess->board);
	}

	for(int r = 0; r < chess->board.height; ++r){
		for(int c = 0; c < chess->board.width; ++c){
			struct IPoint gridCell = Chess_MGetGridCellFromLocal(
				chess,
				r,
				c
			);

			Rectangle rect = ChessLayout_MGetCellRect(
				&chess->layout,
				gridCell.y,
				gridCell.x
			);

			Color color = (
				(gridCell.y + gridCell.x) % 2 == 0
				? CELL_COLOR_LIGHT
				: CELL_COLOR_DARK
			);

			DrawRectangleRec(rect, color);

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
	int mouseX = GetMouseX();
	int mouseY = GetMouseY();

	struct IPoint gridCell = ChessLayout_MGetCellFromPoint(
		&c->layout,
		mouseX,
		mouseY
	);

	c->hoveredCell = Chess_MGetLocalCellFromGrid(
		c,
		gridCell.x,
		gridCell.y
	);

	if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
	}

}

void Chess_MUpdate(struct Chess* c){
	Chess_MDraw(c);
	Chess_MProcessInput(c);
}

void Chess_MDestruct(struct Chess* c) {
	ChessBoard_MDestruct(&c->board);
}
