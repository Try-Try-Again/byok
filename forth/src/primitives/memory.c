#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <kernel/tty.h>
#include <kernel/vga.h>

#include <primitives.h>
#include <stack_machine/common.h>
#include <stack_machine/context.h>
#include <stack_machine/entry.h>
#include <stack_machine/error.h>
#include <stack_machine/compiler.h>

state_t __NEST(context_t *ctx)
{
    pushnum(ctx->rs, (int)ctx->ip);
    ctx->ip = ++ctx->w.ptr;
    return OK;
}


state_t __UNNEST(context_t *ctx)
{
    if (popnum(ctx->rs, (int *)&ctx->ip))
    {
        return EXIT;
    }
    else
    {
        return stack_underflow(ctx);
    }
}

state_t __COMMA(context_t *ctx)
{
    int n;
    if (popnum(ctx->ds, &n))
    {
        comma(ctx, (word_t)n);
        return OK;
    }
    else
    {
        return stack_underflow(ctx);
    }
}

state_t __DOLIT(context_t *ctx)
{
    pushnum(ctx->ds, (*(int *)ctx->ip++));
    return OK;
}

/*
state_t __ALLOT(context_t *ctx)
{
    int n;
    if (popnum(ctx->ds, &n))
    {
        ctx->dp += (n / CELL);
        assert(ctx->dp - ctx->mem < MEMSIZ);
        return OK;
    }
    else
    {
        return stack_underflow(ctx);
    }
}
*/


state_t __CELLS(context_t *ctx)
{
    int n1, n2;
    if (popnum(ctx->ds, &n1))
    {
        n2 = n1 <= 0 ? 0 : ((n1 - 1) / sizeof(ctx->dp)) + 1;
        pushnum(ctx->ds, n2);
        return OK;
    }
    else
    {
        return stack_underflow(ctx);
    }
}


state_t __HERE(context_t *ctx)
{
    pushnum(ctx->ds, (int)ctx->dp);
    return OK;
}

state_t __COLON(context_t *ctx)
{
    static entry_t nest = { .code_ptr = &__NEST, .name = "NEST" };
    if (ctx->state == SMUDGE)
    {
        return error(ctx, -29);  // compiler nesting
    }

    // Skip to next token
    ctx->tib->token = strtok_r(NULL, DELIMITERS, ctx->tib->saveptr);
    if (ctx->tib->token != NULL)
    {
        char *name = strdup(ctx->tib->token);
        add_word(ctx, name, comma(ctx, (word_t)(int *)&nest));

        if (ctx->echo) {
            terminal_setcolor(0x0F);
            printf("Compiling: %s (0x%x)", name, ctx->last_word->param);
            terminal_setcolor(0x07);
            terminal_writestring("\n");
        }

        return SMUDGE;
    }
    else
    {
        return OK;
    }
}

state_t __SEMICOLON(context_t *ctx)
{
    static entry_t unnest = { .code_ptr = &__UNNEST, .name = "UNNEST" };
    comma(ctx, (word_t)(int *)&unnest);
    ctx->last_word->alloc_size = (int)ctx->dp - ctx->last_word->param.val;
    return SMUDGE_OK;
}

state_t __IMMEDIATE(context_t *ctx)
{
    assert(ctx->last_word != NULL);
    entry_t *entry = ctx->last_word;
    entry->flags |= FLAG_IMMEDIATE;
    return OK;
}

state_t __FETCH(context_t *ctx)
{
    word_t addr;
    if (popnum(ctx->ds, (int *)&addr))
    {
        if (addr.addr % sizeof(word_t) != 0)
            return error(ctx, -23);  // address alignment exception

        pushnum(ctx->ds, *addr.ptr);
        return OK;
    }
    else
    {
        return stack_underflow(ctx);
    }
}

state_t __C_FETCH(context_t *ctx)
{
    word_t addr;
    if (popnum(ctx->ds, (int *)&addr))
    {
        pushnum(ctx->ds, *((unsigned char *)addr.ptr));
        return OK;
    }
    else
    {
        return stack_underflow(ctx);
    }
}

state_t __STORE(context_t *ctx)
{
    int x;
    word_t addr;
    if (popnum(ctx->ds, (int *)&addr) && popnum(ctx->ds, &x))
    {
        if (addr.addr % sizeof(word_t) != 0)
            return error(ctx, -23);  // address alignment exception

        *addr.ptr = x;
        return OK;
    }
    else
    {
        return stack_underflow(ctx);
    }
}


state_t __PLUS_STORE(context_t *ctx)
{
    int x;
    word_t addr;

    if (popnum(ctx->ds, (int *)&addr) && popnum(ctx->ds, &x))
    {
        if (addr.addr % sizeof(word_t) != 0)
            return error(ctx, -23);  // address alignment exception

        *addr.ptr += x;
        return OK;
    }
    else
    {
        return stack_underflow(ctx);
    }
}


state_t __C_STORE(context_t *ctx)
{
    int x;
    word_t addr;
    if (popnum(ctx->ds, (int *)&addr) && popnum(ctx->ds, &x))
    {
        *((char *)addr.ptr) = (unsigned char)x & 0xFF;
        return OK;
    }
    else
    {
        return stack_underflow(ctx);
    }
}


state_t __VARIABLE(context_t *ctx)
{
    // Skip to next token
    ctx->tib->token = strtok_r(NULL, DELIMITERS, ctx->tib->saveptr);
    if (ctx->tib->token != NULL)
    {
        entry_t *entry;
        if (find_entry(ctx->exe_tok, ctx->tib->token, &entry) != 0)
        {
            char *name = strdup(ctx->tib->token);
            add_variable(ctx, name, comma(ctx, (word_t)0));
        }
    }
    return OK;
}

state_t __CONSTANT(context_t *ctx)
{
    int x;
    if (popnum(ctx->ds, &x))
    {
        // Skip to next token
        ctx->tib->token = strtok_r(NULL, DELIMITERS, ctx->tib->saveptr);
        if (ctx->tib->token != NULL)
        {
            entry_t *entry;
            if (find_entry(ctx->exe_tok, ctx->tib->token, &entry) != 0)
            {
                char *constant_name = strdup(ctx->tib->token);
                add_constant(ctx, constant_name, x);
            }
        }
        return OK;
    }
    else
    {
        return stack_underflow(ctx);
    }
}

/*
WORD should be defined in terms of PARSE and MOVE

state_t __WORD(context_t *ctx)
{
    int ch;
    if (popnum(ctx->ds, &ch))
    {
        ctx->tib->token = strtok_r(NULL, DELIMITERS, ctx->tib->saveptr);
        if (ctx->tib->token != NULL)
        {
            printf("word token = %s\n", ctx->tib->token);
        }

        return OK;
    }
    else
    {
        return stack_underflow(ctx);
    }
}
*/


state_t __SOURCE(context_t *ctx)
{
   pushnum(ctx->ds, (int)ctx->tib->buffer);
   pushnum(ctx->ds, strlen(ctx->tib->buffer));
   return OK;
}

state_t __TO_IN(context_t *ctx)
{
    pushnum(ctx->ds, (int)ctx->tib->buffer + ctx->tib->cur_offset);
    return OK;
}


state_t __PARSE(context_t *ctx)
{
    int ch;
    if (popnum(ctx->ds, &ch))
    {
        if (ch < 0 || ch > 255)
            return error(ctx, -24);  // invalid numeric argument

        char delim[] = { ch & 0xff, 0 };
        char *start = ctx->tib->token;

        ctx->tib->token = strtok_r(NULL, delim, ctx->tib->saveptr);
        if (ctx->tib->token != NULL)
        {
            int offset = ctx->tib->token - start;
            pushnum(ctx->ds, (int)ctx->tib->buffer + ctx->tib->cur_offset + offset);
            pushnum(ctx->ds, strlen(ctx->tib->token));
        }

        return OK;
    }
    else
    {
        return stack_underflow(ctx);
    }
}

state_t __TICK(context_t *ctx)
{
    ctx->tib->token = strtok_r(NULL, DELIMITERS, ctx->tib->saveptr);
    if (ctx->tib->token != NULL)
    {
        entry_t *entry;
        if (find_entry(ctx->exe_tok, ctx->tib->token, &entry) == 0)
        {
            pushnum(ctx->ds, (int)entry);
            return OK;
        }
    }

    return error_msg(ctx, -13, ": '%s'", ctx->tib->token); // word not found
}

state_t __EXECUTE(context_t *ctx)
{
    int xt;
    if (popnum(ctx->ds, &xt))
    {
        entry_t *entry = (entry_t *)xt;
        return entry->code_ptr(ctx);
    }
    else
    {
        return stack_underflow(ctx);
    }
}

state_t __CREATE(context_t *ctx)
{
    ctx->tib->token = strtok_r(NULL, DELIMITERS, ctx->tib->saveptr);
    if (ctx->tib->token != NULL)
    {
        add_word(ctx, strdup(ctx->tib->token), ctx->dp);
    }
    return OK;
}


state_t __WORDS(context_t *ctx)
{
    char **words = get_words(ctx->exe_tok);

    int acc = 0;
    while (*words != NULL)
    {
        int len = strlen(*words);
        if (acc + len >= 79)
        {
            printf("\n");
            acc = 0;
        }

        acc += len + 1;
        printf("%s ", *words);
        words++;
    }

    free(words);
    return OK;
}

state_t __THROW(context_t *ctx)
{
    int errno;
    if (popnum(ctx->ds, &errno))
    {
        return error(ctx, errno);
    }
    else
    {
        return stack_underflow(ctx);
    }
}

state_t __QERROR(context_t *ctx)
{
    int cond;
    int errno;
    if (popnum(ctx->ds, &errno) && popnum(ctx->ds, &cond))
    {
        return cond == 0 ? OK : error(ctx, errno);
    }
    else
    {
        return stack_underflow(ctx);
    }
}

state_t __MOVE(context_t *ctx)
{
    addr_t a1;
    addr_t a2;
    unsigned int u;

    if (popnum(ctx->ds, (int)&u) && popnum(ctx->ds, &a2) && popnum(ctx->ds, &a1))
    {
        memmove((void *)a2, (void *)a1, sizeof(word_t) * u);
        return OK;
    }
    else
    {
        return stack_underflow(ctx);
    }
}


state_t __CMOVE(context_t *ctx)
{
    addr_t a1;
    addr_t a2;
    unsigned int u;

    if (popnum(ctx->ds, &u) && popnum(ctx->ds, &a2) && popnum(ctx->ds, &a1))
    {
        memmove((void *)a2, (void *)a1, u);
        return OK;
    }
    else
    {
        return stack_underflow(ctx);
    }
}

state_t __BRANCH(context_t *ctx)
{
    int jmp = (*ctx->ip).val / sizeof(int);
    ctx->ip += jmp;
    return OK;
}

state_t __0BRANCH(context_t *ctx)
{
    int x;
    if (popnum(ctx->ds, &x))
    {
        if (x == 0)
        {
            return __BRANCH(ctx);
        }
        else
        {
            ctx->ip++;
            return OK;
        }
    }
    return stack_underflow(ctx);
}

state_t __LITERAL(context_t *ctx)
{
    if (ctx->state != SMUDGE)
        return error(ctx, -14); // use only during compilation

    int x;
    if (popnum(ctx->ds, &x))
    {
        literal(ctx, x);
        static entry_t comma_entry = { .code_ptr = &__COMMA, .name = "," };
        comma(ctx, (word_t)(int *)&comma_entry);
        return SMUDGE;
    }
    else
    {
        return stack_underflow(ctx);
    }
}

state_t __GT_BODY(context_t *ctx)
{
    int xt;
    if (popnum(ctx->ds, &xt))
    {
        entry_t *entry = (entry_t *)xt;
        pushnum(ctx->ds, entry->param.val);
        return OK;
    }
    else
    {
        return stack_underflow(ctx);
    }
}

// 00002e2c:  0a 20 ff 2b  |....|  DISASSEMBLE

state_t __DISASSEMBLE(context_t *ctx)
{
    void print_line(unsigned int addr, unsigned int value, char *name)
    {
        char buf[80] = { 0 };
        char *out = buf;

        char hex_addr[sizeof(unsigned int) + 1];

        itoa(addr, hex_addr, 16);
        out = rpad(out, hex_addr, 8, '0');
        out = write(out, ':');
        out = write(out, ' ');
        out = write(out, ' ');

        out = hex_bytes(out, &value, 4);
        out = write(out, ' ');
        out = write(out, '|');
        out = safe_puts(out, &value, 4);
        out = write(out, '|');

        out = write(out, ' ');
        out = write(out, ' ');
        memcpy(out, name, strlen(name));
        printf("%s\n", buf);
    }

    int size;
    word_t addr;
    if (popnum(ctx->ds, &size) && popnum(ctx->ds, (int *)&addr))
    {
        entry_t *entry = NULL;
        for (int i = 0; i < size; i++)
        {
            int value = addr.ptr[i];
            // TODO determine a better way to identify XTs - magic number?
            //if (entry != NULL && strcmp(entry->name, "(LIT)") == 0)
            //{
            //    print_line(&value, value, NULL);
            //}
            //else
            //{
                entry = (entry_t*)value;
                print_line(&addr.ptr[i], value, entry->name);
            //}
        }
        return OK;
    }
    else
    {
        return stack_underflow(ctx);
    }
}


void init_memory_words(context_t *ctx)
{
    hashtable_t *htbl = ctx->exe_tok;
    add_primitive(htbl, "CELLS", __CELLS, "( n1 -- n2 )", "n2 is the size in address units of n1 cells.");
    add_primitive(htbl, ",", __COMMA, "( x -- )", "Reserve one cell of data space and store x in the cell.");
//    add_primitive(htbl, "ALLOT", __ALLOT, "( n -- )", "If n is greater than zero, reserve n address units of data space. If n is less than zero, release |n| address units of data space. If n is zero, leave the data-space pointer unchanged.");
    add_primitive(htbl, "HERE", __HERE, "( -- addr )","addr is the data-space pointer.");
    add_primitive(htbl, ":", __COLON, "( C: \"<spaces>name\" -- colon-sys )", "Enter compilation state and start the current definition, producing colon-sys.");
    add_primitive(htbl, ";", __SEMICOLON, "( C: colon-sys -- )", "End the current definition, allow it to be found in the dictionary and enter interpretation state, consuming colon-sys.");
    add_primitive(htbl, "SOURCE", __SOURCE, "( -- c-addr u )", "c-addr is the address of, and u is the number of characters in, the input buffer.");
    add_primitive(htbl, ">IN", __TO_IN, "( -- a-addr )", "a-addr is the address of a cell containing the offset in characters from the start of the input buffer to the start of the parse area.");
    add_primitive(htbl, "C!", __C_STORE, "( char c-addr -- )", "Store char at c-addr.");
    add_primitive(htbl, "C@", __C_FETCH, "( c-addr -- x )", "Fetch the character stored at c-addr.");
    add_primitive(htbl, "!", __STORE, "( x a-addr -- )", "Store x at a-addr.");
    add_primitive(htbl, "+!", __PLUS_STORE, "( x a-addr -- )", "Adds x to the single cell number at a-addr.");
    add_primitive(htbl, "@", __FETCH, "( a-addr -- x )", "x is the value stored at a-addr.");
    add_primitive(htbl, "MOVE", __MOVE, "( a1 a2 u --  )", "");
    add_primitive(htbl, "CMOVE", __CMOVE, "( a1 a2 u --  )", "");
    add_primitive(htbl, "VARIABLE", __VARIABLE, "( \"<spaces>name\" -- )", "Skip leading space delimiters. Parse name delimited by a space. Create a definition for name with the execution semantics: `name Execution: ( -- a-addr )`. Reserve one cell of data space at an aligned address.");
    add_primitive(htbl, "CONSTANT", __CONSTANT, "( x \"<spaces>name\" -- )", "Skip leading space delimiters. Parse name delimited by a space. Create a definition for name with the execution semantics: `name Execution: ( -- x )`, which places x on the stack.");
//    add_primitive(htbl, "WORD", __WORD, "( char \"<chars>ccc<char>\" -- c-addr )", "Skip leading delimiters. Parse characters ccc delimited by char. ");
    add_primitive(htbl, "PARSE", __PARSE, "( char \"ccc<char>\" -- c-addr u )", "Parse ccc delimited by the delimiter char. c-addr is the address (within the input buffer) and u is the length of the parsed string. If the parse area was empty, the resulting string has a zero length.");
    add_primitive(htbl, "THROW", __THROW, "( i*x -- )", "");
    add_primitive(htbl, "?ERROR", __QERROR, "", "");
    add_primitive(htbl, "WORDS", __WORDS, "( -- )", "List the definition names in alphabetical order.");
    add_primitive(htbl, "'", __TICK, "( \"<spaces>name\" -- xt )", "Skip leading space delimiters. Parse name delimited by a space. Find name and return xt, the execution token for name.");
    add_primitive(htbl, "EXECUTE", __EXECUTE, "( i*x xt -- j*x )", "Remove xt from the stack and perform the semantics identified by it. Other stack effects are due to the word EXECUTEd.");
    add_primitive(htbl, "CREATE", __CREATE, "( \"<spaces>name\" -- )", "Skip leading space delimiters. Parse name delimited by a space. Create a definition for name with the execution semantics: name Execution: ( -- a-addr )");
    add_primitive(htbl, "EXIT", __UNNEST, "Execution: ( -- ) ( R: nest-sys -- )", "Return control to the calling definition specified by nest-sys. Before executing EXIT within a do-loop, a program shall discard the loop-control parameters by executing UNLOOP.");

    add_primitive(htbl, "IMMEDIATE", __IMMEDIATE, "( -- )", "Make the most recent definition an immediate word.");
    set_flags(htbl, ";", FLAG_IMMEDIATE);

    add_primitive(htbl, "BRANCH", __BRANCH, "( -- )", "");
    add_primitive(htbl, "0BRANCH", __0BRANCH, "( x -- )", "");
    add_primitive(htbl, "(LIT)", __DOLIT, "", "");
    add_primitive(htbl, "LITERAL", __LITERAL, "Compilation: ( x -- ), Runtime: ( -- x )", "Append the run-time semantics to the current definition.");
    set_flags(htbl, "LITERAL", FLAG_IMMEDIATE);

    add_primitive(htbl, ">BODY", __GT_BODY, "( xt -- pfa )", "pfa is the parameter field address corresponding to xt.");
    add_primitive(htbl, "DISASSEMBLE", __DISASSEMBLE, "( len a-addr -- )", "");

    add_constant(ctx, "CELL", CELL);
    add_constant(ctx, "DP", (int)&ctx->dp);
    add_constant(ctx, "TIB", (int)ctx->tib->buffer);
    add_constant(ctx, "BASE", (int)&ctx->base);
    add_constant(ctx, "ECHO", (int)&ctx->echo);
    add_constant(ctx, "STATE", (int)&ctx->state);
    add_constant(ctx, "LATEST", (int)&ctx->last_word);
}
