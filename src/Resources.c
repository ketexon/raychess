#include "Resources.h"

#include "generated/Assets.h"


void Resources_MInitialize(struct Resources* r) {
	r->pieces.black.id = 0;
	r->pieces.white.id = 0;

	Image blackPieces = LoadImageFromMemory(
		".png",
		BLACK_PIECES__DATA,
		sizeof(BLACK_PIECES__DATA)
	);
	Image whitePieces = LoadImageFromMemory(
		".png",
		WHITE_PIECES__DATA,
		sizeof(WHITE_PIECES__DATA)
	);

	r->pieces.black = LoadTextureFromImage(blackPieces);
	r->pieces.white = LoadTextureFromImage(whitePieces);

	r->pieces.pawn.x = BLACK_PIECES__PAWN__X;
	r->pieces.pawn.y = BLACK_PIECES__PAWN__Y;
	r->pieces.pawn.width = BLACK_PIECES__PAWN__WIDTH;
	r->pieces.pawn.height = BLACK_PIECES__PAWN__HEIGHT;

	r->pieces.knight.x = BLACK_PIECES__KNIGHT__X;
	r->pieces.knight.y = BLACK_PIECES__KNIGHT__Y;
	r->pieces.knight.width = BLACK_PIECES__KNIGHT__WIDTH;
	r->pieces.knight.height = BLACK_PIECES__KNIGHT__HEIGHT;

	r->pieces.bishop.x = BLACK_PIECES__BISHOP__X;
	r->pieces.bishop.y = BLACK_PIECES__BISHOP__Y;
	r->pieces.bishop.width = BLACK_PIECES__BISHOP__WIDTH;
	r->pieces.bishop.height = BLACK_PIECES__BISHOP__HEIGHT;

	r->pieces.rook.x = BLACK_PIECES__ROOK__X;
	r->pieces.rook.y = BLACK_PIECES__ROOK__Y;
	r->pieces.rook.width = BLACK_PIECES__ROOK__WIDTH;
	r->pieces.rook.height = BLACK_PIECES__ROOK__HEIGHT;

	r->pieces.queen.x = BLACK_PIECES__QUEEN__X;
	r->pieces.queen.y = BLACK_PIECES__QUEEN__Y;
	r->pieces.queen.width = BLACK_PIECES__QUEEN__WIDTH;
	r->pieces.queen.height = BLACK_PIECES__QUEEN__HEIGHT;

	r->pieces.king.x = BLACK_PIECES__KING__X;
	r->pieces.king.y = BLACK_PIECES__KING__Y;
	r->pieces.king.width = BLACK_PIECES__KING__WIDTH;
	r->pieces.king.height = BLACK_PIECES__KING__HEIGHT;

	UnloadImage(blackPieces);
	UnloadImage(whitePieces);
}

void Resources_MDestruct(struct Resources* r) {
	UnloadTexture(r->pieces.black);
	UnloadTexture(r->pieces.white);
}
