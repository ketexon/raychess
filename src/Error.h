#ifndef ERROR_H
#define ERROR_H

enum Error {
	ERROR_NONE = 0,
	ERROR_INVALID_SIZE = 1,
};

#define ERROR_OK(e) ((e) == ERROR_NONE)

#endif // ERROR_H
