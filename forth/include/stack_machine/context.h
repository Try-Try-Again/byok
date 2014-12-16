#ifndef _CONTEXT_H
#define _CONTEXT_H 1

#include <collections/stack.h>
#include <collections/hashtable.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { NONE, OK, SMUDGE, ERROR } state_t;

typedef struct {
    char *buffer;
    int cur_offset;
    char *token;
} inbuf_t;

typedef unsigned int addr_t;

typedef union {
    int val;
    addr_t addr;
    void *ptr;
} word_t;

typedef struct {
    inbuf_t *inbuf;             // input buffer

    word_t *mem;                // memory
    addr_t dp;                  // data pointer
    addr_t ip;                  // instruction pointer
    word_t w;                   // word register

    stack_t *ds;                // data stack
    stack_t *rs;                // return stack
    stack_t *fs;                // float stack

    hashtable_t *exe_tok;       // execution tokens

    state_t state;

    int errno;
    char *err_msg;
} context_t;

#ifdef __cplusplus
}
#endif

#endif