#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <stack_machine/common.h>
#include <stack_machine/context.h>
#include <stack_machine/compiler.h>
#include <stack_machine/interpreter.h>
#include <stack_machine/entry.h>
#include <stack_machine/error.h>

state_t interpret(context_t *ctx, char *in)
{
    int n = strlen(in);
    if (n == 0)
        return OK;

    if (n >= READLINE_BUFSIZ)
        return error_msg(ctx, -1, ": input larger than TIB size");

    // Copy the input buffer into the context's TIB, then again for tokenizing
    memcpy(ctx->tib->buffer, in, n + 1);
    char *inbuf = strdup(ctx->tib->buffer);
    assert(inbuf != NULL);

    // Begin parsing tokens proper
    ctx->tib->token = strtok_r(inbuf, DELIMITERS, ctx->tib->saveptr);
    while (ctx->tib->token != NULL)
    {
        // Set the current offset
        ctx->tib->cur_offset = ctx->tib->token - inbuf;
        char *s = trim(strdup(ctx->tib->token));
        assert(s != NULL);

        // Is this a word already in the dictionary?
        entry_t *entry;
        if (find_entry(ctx->exe_tok, s, &entry) == 0)
        {
            // Word exists, so set the contents of the word register
            // to the dictionary param,
            ctx->w = entry->param;

            if (immediate_mode(entry) || ctx->state != SMUDGE)
            {
                // Execute immediately if word is marked as IMMEDIATE,
                // or not in compile mode
                state_t retval = entry->code_ptr(ctx);

                // Only propagte the state if an error has been signalled.
                // Certainly don't set to OK if in smudge mode.
                if (retval != OK || ctx->state != SMUDGE)
                    ctx->state = retval;
            }
            else
            {
                // Otherwise compile the execution token into the currently defined word
                compile(ctx, 1, (int)entry);
            }
        }
        else
        {
            int num;
            if (parsenum(s, &num, ctx->base))
            {
                if (ctx->state == SMUDGE)
                {
                    literal(ctx, num);
                }
                else if (pushnum(ctx->ds, num))
                {
                    ctx->state = OK;
                }
                else
                {
                    // ??stack overflow??
                }
            }
            else if (*s != '\0')
            {
                ctx->state = error_msg(ctx, -13, ": '%s'", s); // word not found
            }
        }
        ctx->tib->token = strtok_r(NULL, DELIMITERS, ctx->tib->saveptr);
        free(s);

        if (ctx->state == ERROR) break;
    }
    free(inbuf);

    return ctx->state;
}