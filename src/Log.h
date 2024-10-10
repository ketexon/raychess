#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#define CHESS_LOG_INFO(...) printf(__VA_ARGS__)
#define CHESS_LOG_ERROR(...) fprintf(stderr, __VA_ARGS__)
#define CHESS_LOG_FATAL(...) fprintf(stderr, __VA_ARGS__)

#endif // LOG_H
