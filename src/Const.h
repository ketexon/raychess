#ifndef CONST_H
#define CONST_H

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_TITLE "RayChess"

#define CELL_COLOR_DARK (Color) { 0xFF, 0x69, 0xB4, 0xFF }
#define CELL_COLOR_LIGHT (Color) { 0xFF, 0xFF, 0xFF, 0xFF }

#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#endif // CONST_H
