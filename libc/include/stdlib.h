#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <stddef.h>
#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

#if NDEBUG
#define assert(expr) 0
#else
#define assert(expr) do { ((expr) ? 0 :  __assert_failed(__STRING(expr), __FILE__, __LINE__)); } while(0)
#endif

#define abort()       __abort(__FILE__, __LINE__)

__attribute__((__noreturn__))
extern void __assert_failed(char *expr, char *file, int line);

__attribute__((__noreturn__))
extern void __abort(char *file, int line);

// TODO: these should be in string.h ??
extern char* itoa(int value, char* str, int base);
extern char* dtoa(double value, char* str);
extern int atoi(char *str, int base);

extern void* malloc( unsigned nbytes );
extern void* calloc( unsigned n, unsigned size );
extern void free( void* ap );

#define min(a,b) (a < b ? a : b)
#define max(a,b) (a > b ? a : b)

#ifdef __cplusplus
}
#endif

#endif
