#ifndef RESOURCES_H
#define RESOURCES_H

#include <raylib.h>

struct TextureSlice {
	Texture2D texture;
	Rectangle rect;
};

struct Resources {
	struct {
		Texture2D black;
		Texture2D white;

		Rectangle pawn;
		Rectangle bishop;
		Rectangle knight;
		Rectangle rook;
		Rectangle queen;
		Rectangle king;
	} pieces;
};

void Resources_MInitialize(struct Resources*);
void Resources_MDestruct(struct Resources*);

#endif // RESOURCES_H
