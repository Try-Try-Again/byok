#ifndef _CONTEXT_H
#define _CONTEXT_H 1

#include <collections/stack.h>
#include <collections/hashtable.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { OK=0, SMUDGE=1, ERROR, NONE, SMUDGE_OK } state_t;

typedef struct {
    char *buffer;
    int cur_offset;
    char *token;
} inbuf_t;

typedef unsigned int addr_t;
typedef unsigned char byte_t;

typedef union {
    byte_t bytes[sizeof(int) / sizeof(byte_t)];
    int val;
    addr_t addr;
    int *ptr;
} word_t;

struct context;

typedef struct {
    int len;
    char *name;
    unsigned int flags;
    char *stack_effect;
    char *docstring;
    state_t (*code_ptr)(struct context *ctx);
    word_t param;
} entry_t;

typedef struct context {
    inbuf_t *tib;               // input buffer

    word_t *mem;                // memory
    word_t *dp;                 // data pointer
    word_t *ip;                 // instruction pointer
    word_t w;                   // word register

    stack_t *ds;                // data stack
    stack_t *rs;                // return stack

    hashtable_t *exe_tok;       // execution tokens
    entry_t *last_word;         // last defined word
    unsigned int base;
    unsigned int echo;

    state_t state;

    int errno;
    char *err_msg;
} context_t;

#ifdef __cplusplus
}
#endif

#endif