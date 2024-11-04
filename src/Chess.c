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
	struct IPoint cell;
	cell.r = r;
	cell.c = c;
	return ChessBoard_MGetPiecePoint(b, cell);
}

struct ChessPiece* ChessBoard_MGetPiecePoint(struct ChessBoard* b, struct IPoint point){
	if(!ChessBoard_MCellInBounds(b, point)) return 0;
	return &b->pieces[point.c + b->width * point.r];
}

int ChessBoard_MCellInBounds(struct ChessBoard* b, struct IPoint point){
	return (
		point.r >= 0 && point.c >= 0
		&& point.r < b->height && point.c < b->width
	);
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

Rectangle ChessLayout_MGetCellPieceRect(struct ChessLayout* l, int r, int c) {
	Rectangle cellRect = ChessLayout_MGetCellRect(l, r, c);
	float padding = cellRect.width * 0.1f;

	cellRect.x += padding;
	cellRect.y += padding;
	cellRect.width -= 2 * padding;
	cellRect.height -= 2 * padding;

	return cellRect;
}

struct IPoint ChessLayout_MGetGridCellFromPoint(struct ChessLayout* l, int x, int y) {
	struct IPoint out;
	if(
		x < l->board.x || x > l->board.x + l->board.width
		|| y < l->board.y || y > l->board.y + l->board.height
	) {
		out.r = -1;
		out.c = -1;
		return out;
	}
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
	// int c = INT_DIVIDE_ROUND_DOWN(x - l->board.x, l->cellGap + l->cellWidth);
	out.c = INT_DIVIDE_ROUND_DOWN(x - (int)(l->board.x), (l->cellGap + l->cellWidth));
	out.r = INT_DIVIDE_ROUND_DOWN(y - (int)(l->board.y), (l->cellGap + l->cellWidth));
	return out;
}

struct IPoint Chess_MGetGridCellFromLocal(struct Chess *chess, struct IPoint cell){
	struct IPoint out;

	out.x = chess->side == CHESS_WHITE ? cell.c : (chess->board.width - cell.c - 1);
	out.y = chess->side == CHESS_WHITE ? (chess->board.height - cell.r - 1) : cell.r;

	return out;
}

struct IPoint Chess_MGetLocalCellFromGrid(struct Chess *chess, struct IPoint cell){
	// the transformation is symmetric
	return Chess_MGetGridCellFromLocal(chess, cell);
}

void Chess_MInitializeDefault(struct Chess* c) {
	c->hoveredCell.r = -1;
	c->hoveredCell.c = -1;
	c->selectedCell.r = -1;
	c->selectedCell.c = -1;

	c->side = CHESS_WHITE;
	c->turn = CHESS_WHITE;

	c->possibleMoves = 0;
	c->nPossibleMoves = 0;

	c->nMoveHistory = 0;
	c->moveHistoryCap = 64;
	c->moveHistory = malloc(c->moveHistoryCap * sizeof(struct Move));

	c->enpassantSquare.r = -1;
	c->enpassantSquare.c = -1;

	ChessBoard_MInitialize(&c->board, DEFAULT_CHESS_BOARD_WIDTH, DEFAULT_CHESS_BOARD_HEIGHT);
	Error e = ChessBoard_MInitializeDefaultStartingPieces(&c->board);
	if(!ERROR_OK(e)) {
		CHESS_LOG_FATAL("Could not initialize default chess board: %d", e);
	}

	ChessLayout_MRecalculate(&c->layout, &c->board);

	Resources_MInitialize(&c->resources);
}

void Chess_MDrawCell(
	struct Chess* chess,
	struct IPoint cell
) {
	struct IPoint gridCell = Chess_MGetGridCellFromLocal(
		chess,
		cell
	);

	Rectangle rect = ChessLayout_MGetCellRect(
		&chess->layout,
		gridCell.r,
		gridCell.c
	);

	int light = (gridCell.r + gridCell.c) % 2 == 0;
	int hovered = chess->hoveredCell.r == cell.r && chess->hoveredCell.c == cell.c;
	struct ChessPiece* piece = ChessBoard_MGetPiecePoint(&chess->board, cell);
	int selected = chess->selectedCell.r == cell.r && chess->selectedCell.c == cell.c;

	Color color = (
		light
		? selected ? CELL_COLOR_LIGHT_SELECTED : CELL_COLOR_LIGHT
		: selected ? CELL_COLOR_DARK_SELECTED : CELL_COLOR_DARK
	);

	DrawRectangleRec(rect, color);
	if(hovered){
		DrawRectangleLinesEx(rect, 2, BLUE);
	}

	if(piece->type != CHESS_PIECE_NONE) {
		Rectangle pieceRect = ChessLayout_MGetCellPieceRect(&chess->layout, gridCell.r, gridCell.c);
		struct TextureSlice slice = Chess_MGetTextureSliceFromPiece(chess, *piece);
		DrawTexturePro(
			slice.texture,
			slice.rect,
			pieceRect,
			(Vector2) { 0 },
			0,
			RAYWHITE
		);
	}
}

void Chess_MDrawPossibleMoves(struct Chess* chess){
	for(int i = 0; i < chess->nPossibleMoves; ++i){
		struct IPoint possibleMove = chess->possibleMoves[i];
		struct IPoint possibleMoveGrid = Chess_MGetGridCellFromLocal(chess, possibleMove);

		// draw a circle
		Rectangle rect = ChessLayout_MGetCellRect(&chess->layout, possibleMoveGrid.r, possibleMoveGrid.c);
		Vector2 center;
		
		DrawCircle(
			rect.x + rect.width/2,
			rect.y + rect.height/2,
			rect.width / 4,
			POSSIBLE_MOVE_COLOR
		);
	}
}

struct TextureSlice Chess_MGetTextureSliceFromPiece(struct Chess* c, struct ChessPiece p){
	struct TextureSlice out = { 0 };

	out.texture = p.side == CHESS_WHITE ? c->resources.pieces.white : c->resources.pieces.black;
	switch(p.type){
		case CHESS_PIECE_PAWN: out.rect = c->resources.pieces.pawn; break;
		case CHESS_PIECE_KNIGHT: out.rect = c->resources.pieces.knight; break;
		case CHESS_PIECE_BISHOP: out.rect = c->resources.pieces.bishop; break;
		case CHESS_PIECE_ROOK: out.rect = c->resources.pieces.rook; break;
		case CHESS_PIECE_QUEEN: out.rect = c->resources.pieces.queen; break;
		case CHESS_PIECE_KING: out.rect = c->resources.pieces.king; break;
	}

	return out;
}

void Chess_MDraw(struct Chess* chess){
	BeginDrawing();
	ClearBackground(RAYWHITE);

	if(IsWindowResized()){
		ChessLayout_MRecalculate(&chess->layout, &chess->board);
	}

	struct IPoint cell;
	for(int r = 0; r < chess->board.height; ++r){
		for(int c = 0; c < chess->board.width; ++c){
			cell.r = r;
			cell.c = c;
			Chess_MDrawCell(chess, cell);
		}
	}

	Chess_MDrawPossibleMoves(chess);

	EndDrawing();
}


struct ChessPiece* Chess_MGetEnPassantPiece(struct Chess* c) {
	if(c->enpassantSquare.r < 0) return 0;
	struct IPoint point = c->enpassantSquare;
	point.r += c->side == CHESS_WHITE ? -1 : 1;
	return ChessBoard_MGetPiecePoint(&c->board, point);
}


struct IPoint* Chess_MCalculatePossibleMoves(struct Chess* chess, struct IPoint cell, int* nPossibleMovesOut){
	*nPossibleMovesOut = 0;

	if(chess->selectedCell.r < 0 || chess->selectedCell.c < 0) {
		return 0;
	}

	struct ChessPiece* piece = ChessBoard_MGetPiece(
		&chess->board,
		chess->selectedCell.r,
		chess->selectedCell.c
	);

	if(!piece || piece->type == CHESS_PIECE_NONE){
		return 0;
	}

	int nPossibleMoves = 0;
	struct IPoint* possibleMoves = calloc(
		chess->board.width * chess->board.height,
		sizeof(struct IPoint)
	);

	// Calculate the directions a piece can move
	// (eg. for bishop, it can move in the diagonals,
	// rook the orthogonals, etc)
	int nDirs = 0;
	struct IPoint dirs[8] = { 0 };

	if(piece->type == CHESS_PIECE_ROOK || piece->type == CHESS_PIECE_QUEEN){
		dirs[nDirs++].r = 1;
		dirs[nDirs++].r = -1;
		dirs[nDirs++].c = 1;
		dirs[nDirs++].c = -1;
	}

	if(piece->type == CHESS_PIECE_BISHOP || piece->type == CHESS_PIECE_QUEEN){
		dirs[nDirs].r = 1; 
		dirs[nDirs++].c = 1; 

		dirs[nDirs].r = -1; 
		dirs[nDirs++].c = 1; 

		dirs[nDirs].r = -1; 
		dirs[nDirs++].c = -1; 

		dirs[nDirs].r = 1; 
		dirs[nDirs++].c = -1; 
	}

	struct ChessPiece* otherPiece;

	for(int i = 0; i < nDirs; ++i){
		struct IPoint dir = dirs[i];
		for(int dist = 1;; ++dist){
			struct IPoint otherCell;
			otherCell.r = cell.r + dist * dir.r;
			otherCell.c = cell.c + dist * dir.c;

			otherPiece = ChessBoard_MGetPiecePoint(
				&chess->board,
				otherCell
			);

			if(!otherPiece) {
				break;
			}
			
			// if the cell is empty or it has an enemy piece
			// it is a candidate
			if(otherPiece->type == CHESS_PIECE_NONE || otherPiece->side != piece->side){
				possibleMoves[nPossibleMoves++] = otherCell;
			}

			// if the cell has any piece, we cannot go through it
			if(otherPiece->type != CHESS_PIECE_NONE){
				break;
			}
		}
	}

	if(piece->type == CHESS_PIECE_PAWN){
		struct IPoint otherCell;
		otherCell.c = cell.c;
		// direction the pawn moves
		int moveDir = (piece->side == CHESS_WHITE ? 1 : -1);
		otherCell.r = cell.r + moveDir;

		// check in front of pawn
		otherPiece = ChessBoard_MGetPiecePoint(&chess->board, otherCell);

		if(otherPiece && otherPiece->type == CHESS_PIECE_NONE){
			possibleMoves[nPossibleMoves++] = otherCell;
			// if pawn has not moved, check if can move twice
			if (
				cell.r == 1 && piece->side == CHESS_WHITE
				|| cell.r == chess->board.nRows - 2 && piece->side == CHESS_BLACK
			){
				otherCell.r = cell.r + 2 * moveDir;
				otherCell.c = cell.c;
				otherPiece = ChessBoard_MGetPiecePoint(&chess->board, otherCell);
				if(otherPiece && otherPiece->type == CHESS_PIECE_NONE){
					possibleMoves[nPossibleMoves++] = otherCell;
				}
			}
		}

		// normal pawn captures
		for (int i = -1; i <= 1; i += 2){
			otherCell.r = cell.r + moveDir;
			otherCell.c = cell.c + i;

			otherPiece = ChessBoard_MGetPiecePoint(&chess->board, otherCell);

			if(otherPiece && otherPiece->type != CHESS_PIECE_NONE && otherPiece->side != piece->side){
				possibleMoves[nPossibleMoves++] = otherCell;
			}
		}

		// en passant captures
		// printf("%d %d\n", chess->enpassantSquare.r, chess->enpassantSquare.c);
		if (chess->enpassantSquare.r >= 0){
			if(ABS(cell.c - chess->enpassantSquare.c) == 1 && chess->enpassantSquare.r == cell.r + moveDir){
				possibleMoves[nPossibleMoves++] = chess->enpassantSquare;
			}
		}
	}

	if(piece->type == CHESS_PIECE_KNIGHT){
		struct IPoint otherCell;
		for(int i = -1; i <= 1; i += 2){
			for(int j = -1; j <= 1; j += 2){
				for(int r = 1; r <= 2; ++r){
					otherCell.r = cell.r + i * r;
					otherCell.c = cell.c + j * (3 - r);

					otherPiece = ChessBoard_MGetPiecePoint(&chess->board, otherCell);
					if(otherPiece && (otherPiece->type == CHESS_PIECE_NONE || otherPiece->side != piece->side)){
						possibleMoves[nPossibleMoves++] = otherCell;
					}
				}
			}
		}

	}

	if(nPossibleMoves == 0){
		free(possibleMoves);
		return 0;
	}

	possibleMoves = realloc(possibleMoves, nPossibleMoves * sizeof(struct IPoint));
	*nPossibleMovesOut = nPossibleMoves;

	return possibleMoves;
}

void Chess_MForceMove(struct Chess* c, struct IPoint from, struct IPoint to){
	struct ChessPiece* fromPiece = ChessBoard_MGetPiecePoint(&c->board, from);
	struct ChessPiece* toPiece = ChessBoard_MGetPiecePoint(&c->board, to);
	if(fromPiece && toPiece){
		*toPiece = *fromPiece;
		fromPiece->type = CHESS_PIECE_NONE;
	}
}

void Chess_MSwapTurns(struct Chess* c){
	c->turn = c->turn == CHESS_WHITE ? CHESS_BLACK : CHESS_WHITE;
}

void Chess_MSaveMove(struct Chess* c, struct Move move){
	if(c->nMoveHistory + 1 >= c->moveHistoryCap){
		c->moveHistoryCap *= 2;
		c->moveHistory = realloc(c->moveHistory, c->moveHistoryCap * sizeof(struct Move));
	}
	c->moveHistory[c->nMoveHistory++] = move;

	c->enpassantSquare.r = -1;
	c->enpassantSquare.c = -1;

	if(move.piece.type == CHESS_PIECE_PAWN){
		int delta = move.to.r - move.from.r;
		if(delta > 1 || -delta > 1){
			c->enpassantSquare = move.from;
			c->enpassantSquare.r += delta/2;
		}
	}
}

int Chess_MTryMove(struct Chess* c, struct IPoint from, struct IPoint to){
	struct ChessPiece* piece = ChessBoard_MGetPiecePoint(&c->board, from);
	if(!piece || piece->type == CHESS_PIECE_NONE || piece->side != c->turn){
		return 0;
	}

	int nPossibleMoves;
	struct IPoint* possibleMoves = Chess_MCalculatePossibleMoves(c, from, &nPossibleMoves);

	int moved = 0;

	for(int i = 0; i < nPossibleMoves; ++i){
		struct IPoint possibleMove = possibleMoves[i];

		if(possibleMove.r == to.r && possibleMove.c == to.c){
			moved = 1;

			struct Move move;
			move.piece = *piece;
			move.from = from;
			move.to = to;
			move.capture.type = CHESS_PIECE_NONE;

			struct ChessPiece* capture = ChessBoard_MGetPiecePoint(&c->board, possibleMove);

			if(capture) {
				move.capture = *capture;
			}

			// check en passant
			if(piece->type == CHESS_PIECE_PAWN) {
				printf("%d %d %d %d\n", to.r, to.c, c->enpassantSquare.r, c->enpassantSquare.c);
				if (to.r == c->enpassantSquare.r && to.c == c->enpassantSquare.c){
					printf("EN PASSANT\n");
					capture = Chess_MGetEnPassantPiece(c);
					move.capture = *capture;
					capture->type = CHESS_PIECE_NONE;
				}
			}
			Chess_MForceMove(c, from, to);
			Chess_MSaveMove(c, move);
			Chess_MSwapTurns(c);
			break;
		}
	}

	if(possibleMoves){
		free(possibleMoves);
		nPossibleMoves = 0;
		possibleMoves = 0;
	}

	return moved;
}

int Chess_MTrySelectPiece(struct Chess* c, struct IPoint cell) {
	struct ChessPiece* piece = ChessBoard_MGetPiecePoint(
		&c->board,
		c->hoveredCell
	);

	if(piece->type != CHESS_PIECE_NONE && piece->side == c->turn) {
		c->selectedCell = c->hoveredCell;

		// recalculate possible moves from selection
		c->possibleMoves = Chess_MCalculatePossibleMoves(c, c->hoveredCell, &c->nPossibleMoves);
		for (int i = 0; i < c->nPossibleMoves; ++i){
			struct IPoint possibleMove = c->possibleMoves[i];
		}
		return 1;
	}
	return 0;
}

void Chess_MProcessInput(struct Chess* c){
	int mouseX = GetMouseX();
	int mouseY = GetMouseY();

	struct IPoint gridCell = ChessLayout_MGetGridCellFromPoint(
		&c->layout,
		mouseX,
		mouseY
	);

	c->hoveredCell = Chess_MGetLocalCellFromGrid(
		c,
		gridCell
	);

	if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
		if(ChessBoard_MCellInBounds(&c->board, c->hoveredCell)) {
			if (c->possibleMoves){
				free(c->possibleMoves);
				c->possibleMoves = 0;
				c->nPossibleMoves = 0;
			}

			int moved = Chess_MTryMove(c, c->selectedCell, c->hoveredCell);
			c->selectedCell.r = -1;

			// our click was not a move, so see 
			// if we clicked on a piece
			if(!moved) {
				Chess_MTrySelectPiece(c, c->hoveredCell);
			}
		}
		else {
			c->selectedCell.r = -1;
		}
	}
}

void Chess_MUpdate(struct Chess* c){
	Chess_MDraw(c);
	Chess_MProcessInput(c);
}

void Chess_MDestruct(struct Chess* c) {
	ChessBoard_MDestruct(&c->board);
	Resources_MDestruct(&c->resources);

	if(c->possibleMoves){
		free(c->possibleMoves);
		c->possibleMoves = 0;
		c->nPossibleMoves = 0;
	}

	free(c->moveHistory);
	c->moveHistory = 0;
	c->nMoveHistory = 0;
	c->moveHistoryCap = 0;
}
