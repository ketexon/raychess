#ifndef ERROR_H
#define ERROR_H

#include <stdint.h>

typedef uint8_t Error;

#define ERROR_NONE 0
#define ERROR_INVALID_SIZE 1

#define ERROR_OK(e) ((e) == ERROR_NONE)

#endif // ERROR_H
