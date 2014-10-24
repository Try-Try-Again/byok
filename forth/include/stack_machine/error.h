#ifndef _STACK_MACHINE_ERROR_H
#define _STACK_MACHINE_ERROR_H 1

#include <stack_machine/context.h>

extern state_t sm_abort(context_t *ctx);
extern state_t error(context_t *ctx, int errno, char *msg, ...);
extern state_t stack_underflow(context_t *ctx);

#endif