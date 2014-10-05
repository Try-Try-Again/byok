#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
extern void abort(void);
extern char* itoa( int value, char* str, int base );

#ifdef __cplusplus
}
#endif

#endif
