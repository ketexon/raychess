#include <raylib.h>

#include "Const.h"
#include "Chess.h"

int main(){
	struct Chess chess;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);

	Chess_MInitializeDefault(&chess);

	while(!WindowShouldClose()){
		BeginDrawing();
		Chess_MUpdate(&chess);
	}

	CloseWindow();

	Chess_MDestruct(&chess);
	return 0;
}
