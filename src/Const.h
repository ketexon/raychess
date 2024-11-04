#ifndef CONST_H
#define CONST_H

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_TITLE "RayChess"

#define CELL_COLOR_DARK (Color) { 0xFF, 0x69, 0xB4, 0xFF }
#define CELL_COLOR_LIGHT (Color) { 0xFF, 0xFF, 0xFF, 0xFF }
#define CELL_COLOR_DARK_SELECTED (Color) { 0xFF, 0x2A, 0x94, 0xFF }
#define CELL_COLOR_LIGHT_SELECTED (Color) { 0xD1, 0xD1, 0xD1, 0xFF }

#define POSSIBLE_MOVE_COLOR (Color) { 0x4C, 0xB5, 0xAE, 0xFF }

#define INT_DIVIDE_ROUND_DOWN(a,b) ((a < 0) ^ (b < 0) ? ((a) / (b)) - 1 : ((a) / (b)))
#define INT_DIVIDE_ROUND_UP(a,b) (((a) + (b-1))/(b))

#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef ABS
#define ABS(a) ((a) < 0 ? -(a) : (a))
#endif

#endif // CONST_H
