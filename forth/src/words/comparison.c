#include <stdlib.h>
#include <math.h>

#include <words.h>
#include <stack_machine/common.h>
#include <stack_machine/context.h>
#include <stack_machine/entry.h>
#include <stack_machine/error.h>

arity2stackop(__EQ, x1 == x2)
arity2stackop(__NEQ,  x1 != x2)
arity2stackop(__LT, x1 < x2)
arity2stackop(__GT, x1 > x2)
arity2stackop(__ULT, (unsigned int)x1 < (unsigned int)x2)
arity2stackop(__UGT, (unsigned int)x1 > (unsigned int)x2)
arity1stackop(__ISNEG, x1 < 0)
arity1stackop(__ISZERO, x1 == 0)
arity1stackop(__ISPOS, x1 > 0)
arity3stackop(__WITHIN, x1 >= x2 && x1 < x3)

void init_comparison_words(hashtable_t *htbl)
{
    add_entry(htbl, "=",  __EQ,   "( x1 x2 -- f )", "compares top two stack elements, returns true flag if equal, false otherwise.");
    add_entry(htbl, "<>", __NEQ,  "( x1 x2 -- f )", "compares top two stack elements, returns true flag if different, false otherwise.");
    add_entry(htbl, "<",  __LT,   "( n1 n2 -- f )", "compares signed numbers n1 with n2, returns true if n1 is less then n2.");
    add_entry(htbl, ">",  __GT,   "( n1 n2 -- f )", "compares signed numbers n1 with n2, returns true if n1 is greater then n2.");
    add_entry(htbl, "U<", __ULT,  "( u1 u2 -- f )", "compares unsigned numbers u1 with u2, returns true if n1 is lower then n2.");
    add_entry(htbl, "U>", __UGT,  "( u1 u2 -- f )", "compares unsigned numbers u1 with u2, returns true if n1 is higher then n2.");
    add_entry(htbl, "0<", __ISNEG, "( n -- f )", "return a true flag if value of n is negative.");
    add_entry(htbl, "0=", __ISZERO, "( x -- f )", "return a true flag if value of x is zero.");
    add_entry(htbl, "0>", __ISPOS,  "( n -- f )", "return a true flag if value of x is greater than zero.");
    add_entry(htbl, "WITHIN", __WITHIN, "( x1 x2 x3 -- f )", "return a true flag if x1 is in the range of x2 ... x3-1.");
}
