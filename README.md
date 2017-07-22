# BYOK - a bare-metal x86 Forth OS

A hobby implementation of a [Forth](https://en.wikipedia.org/wiki/Forth_(programming_language))
interpreter and compiler, running directly on top of an x86 machine. The
screencast below has been sped up, to offset my slow typing.

![screencast](https://raw.githubusercontent.com/rm-hull/byok/master/doc/screencast-039ece6f.gif)

## Setup / Building

For development and testing, using [QEMU](http://wiki.qemu.org/Main_Page) is recommended.
Using bochs, virtual box or VMware should equally work.

From Ubuntu command line (older OS versions may need `qemu-kvm` instead of `qemu-system-i386`):

    $ sudo apt-get install qemu-system-x86 ghex nasm xorriso make grub-pc-bin

Building requires a barebones [i686-elf or ARMv7 cross compiler](https://github.com/rm-hull/barebones-toolchain)
installing first. Follow the instructions on that page, and then check it works by running:

    $ i686-elf-gcc --version
    i686-elf-gcc (GCC) 4.9.1
    Copyright (C) 2014 Free Software Foundation, Inc.
    This is free software; see the source for copying conditions.  There is NO
    warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

Next, to completely rebuild the kernel:

    $ make clean
    $ make build

To build an ISO image of the kernel:

    $ make iso

This may then either be burned onto a CD-ROM, or loaded onto a USB stick. Alternatively,
running it in qemu:

    $ make qemu

## Debugging

From the ubuntu command line install gdb and [nemiver](https://en.wikipedia.org/wiki/Nemiver):

    $ sudo apt-get install gdb nemiver

Build an iso image and then launch qemu, making it wait for the debugger
to attach:

    $ make iso qemu-gdb

In a separate terminal, launch gdb (or nemiver) and at the GDB prompt,
connect to the suspended QEMU and load the symbols from the kernel
image:

    $ gdb

    (gdb) target remote localhost:1234
    Remote debugging using localhost:1234
    0x0000fff0 in ?? ()

    (gdb) symbol-file kernel/byok.kernel
    Reading symbols from kernel/byok.kernel...done.

Next, pick a function to break on, and continue/step/inspect as normal:

    (gdb) break repl
    Breakpoint 1 at 0x101e30: file src/stack_machine/repl.c, line 69.

    (gdb) cont
    Continuing.

    Breakpoint 1, repl () at src/stack_machine/repl.c:69
    69	{

    (gdb) next
    72	    history_t *hist = init_history(READLINE_HISTSIZ);

    (gdb) info registers
    eax            0xffffffff	-1
    ecx            0xe	14
    edx            0x0	0
    ebx            0x400	1024
    esp            0x114988	0x114988
    ebp            0x0	0x0
    esi            0x0	0
    edi            0x0	0
    eip            0x101e3a	0x101e3a <repl+10>
    eflags         0x200206	[ PF IF ID ]
    cs             0x8	8
    ss             0x10	16
    ds             0x10	16
    es             0x10	16
    fs             0x10	16
    gs             0x10	16

## Implemented Words


### forth/src/primitives/arithmetics.c

| Word | Stack Effect | Description |
|------|--------------|-------------|
| * | ( x1 x2 -- x3 ) | multiplies x1 with x2, leaves result x3. |
| */ | ( n1 n2 n3 -- n4 ) | multiplies then divides (n1 x n2) / n3. |
| */MOD | ( n1 n2 n3 -- n4 n5) | multiplies then divides (n1 x n2) / n3, returning the remainder n n4 and quotient in n5. |
| + | ( x1 x2 -- x3 ) | adds x1 and x2, leaves result x3. |
| - | ( x1 x2 -- x3 ) | subtracts x2 from x1, leaves result x3. |
| / | ( x1 x2 -- x3 ) | divides x1 by x2, leaves result x3. |
| /MOD | ( n1 n2 -- n-rem n-quot ) | calculates and returns remainder and quotient of division n1/n2. |
| 1+ | ( x1 -- x2 ) | increments x1 by 1. |
| 1- | ( x1 -- x2 ) | decrements x1 by 1. |
| 2* | ( x1 -- x2 ) | multiply x1 by 2. |
| 2+ | ( x1 -- x2 ) | increments x1 by 2. |
| 2- | ( x1 -- x2 ) | decrements x1 by 2. |
| 2/ | ( n1 -- n2 ) | divide n1 by 2. |
| ABS | ( n -- u ) | return absolute value of n. |
| MAX | ( n1 n2 -- n3 ) | return the greater of the two signed numbers n1 and n2. |
| MIN | ( n1 n2 -- n3 ) | return the lesser of the two signed numbers n1 and n2. |
| MOD | ( n1 n2 -- n3 ) | calculates and returns remainder of division n1/n2. |
| NEGATE | ( n1 -- n2 ) | change sign of n1. |

### forth/src/primitives/bit_logic.c

| Word | Stack Effect | Description |
|------|--------------|-------------|
| AND | ( x1 x2 -- x3 ) | bitwise and x1 with x2, return result x3. |
| INVERT | ( x1 -- x2 ) | return the bitwise complement of x1. |
| LSHIFT | ( u1 u2 -- u3 ) | logical shift left u1 by u2 bits. |
| OR | ( x1 x2 -- x3 ) | bitwise or x1 with x2, return result x3. |
| RSHIFT | ( u1 u2 -- u3 ) | logical shift right u1 by u2 bits. |
| XOR | ( x1 x2 -- x3 ) | bitwise exclusive-or x1 with x2, return result x3. |

### forth/src/primitives/comparison.c

| Word | Stack Effect | Description |
|------|--------------|-------------|
| 0< | ( n -- f ) | return a true flag if value of n is negative. |
| 0<> | ( x -- f ) | return a true flag if value of x is not zero. |
| 0= | ( x -- f ) | return a true flag if value of x is zero. |
| 0> | ( n -- f ) | return a true flag if value of x is greater than zero. |
| < | ( n1 n2 -- f ) | compares signed numbers n1 with n2, returns true if n1 is less then n2. |
| <> | ( x1 x2 -- f ) | compares top two stack elements, returns true flag if different, false otherwise. |
| = | ( x1 x2 -- f ) | compares top two stack elements, returns true flag if equal, false otherwise. |
| > | ( n1 n2 -- f ) | compares signed numbers n1 with n2, returns true if n1 is greater then n2. |
| U< | ( u1 u2 -- f ) | compares unsigned numbers u1 with u2, returns true if n1 is lower then n2. |
| U> | ( u1 u2 -- f ) | compares unsigned numbers u1 with u2, returns true if n1 is higher then n2. |
| WITHIN | ( x1 x2 x3 -- f ) | return a true flag if x1 is in the range of x2 ... x3-1. |

### forth/src/primitives/io.c

| Word | Stack Effect | Description |
|------|--------------|-------------|
| . | ( n -- ) | convert signed number n to string of digits, and output. |
| .S | ( -- ) | display stack contents. |
| CLS | ( -- ) | clear screen. |
| CURSOR | ( start end -- ) |  |
| EMIT | ( ascii -- ) | outputs ascii as character. |
| KEY | ( -- ascii ) | waits for key, returns ascii. |
| LIST | ( block -- ) |  |
| LOAD | ( block -- ) |  |
| PAGE | ( -- ) | clear screen. |
| SPACES | ( u -- ) | outputs u space characters. |
| TYPE | ( addr n -- ) | outputs the contents of addr for n bytes. |
| U. | ( u -- ) | convert unsigned number n to string of digits, and output. |

### forth/src/primitives/memory.c

| Word | Stack Effect | Description |
|------|--------------|-------------|
| ! | ( x a-addr -- ) | Store x at a-addr. |
| ' | ( \<spaces>name\" -- xt )" | Skip leading space delimiters. Parse name delimited by a space. Find name and return xt, the execution token for name. |
| (LIT) |  |  |
| +! | ( x a-addr -- ) | Adds x to the single cell number at a-addr. |
| , | ( x -- ) | Reserve one cell of data space and store x in the cell. |
| 0BRANCH | ( x -- ) |  |
| : | ( C: \<spaces>name\" -- colon-sys )" | Enter compilation state and start the current definition, producing colon-sys. |
| ; | ( C: colon-sys -- ) | End the current definition, allow it to be found in the dictionary and enter interpretation state, consuming colon-sys. |
| >BODY | ( xt -- pfa ) | pfa is the parameter field address corresponding to xt. |
| >IN | ( -- a-addr ) | a-addr is the address of a cell containing the offset in characters from the start of the input buffer to the start of the parse area. |
| ?ERROR |  |  |
| @ | ( a-addr -- x ) | x is the value stored at a-addr. |
| ALLOT | ( n -- ) | If n is greater than zero, reserve n address units of data space. If n is less than zero, release |n| address units of data space. If n is zero, leave the data-space pointer unchanged. |
| BRANCH | ( -- ) |  |
| C! | ( char c-addr -- ) | Store char at c-addr. |
| C@ | ( c-addr -- x ) | Fetch the character stored at c-addr. |
| CELLS | ( n1 -- n2 ) | n2 is the size in address units of n1 cells. |
| CMOVE | ( a1 a2 u --  ) |  |
| CONSTANT | ( x \<spaces>name\" -- )" | Skip leading space delimiters. Parse name delimited by a space. Create a definition for name with the execution semantics: `name Execution: ( -- x )`, which places x on the stack. |
| CREATE | ( \<spaces>name\" -- )" | Skip leading space delimiters. Parse name delimited by a space. Create a definition for name with the execution semantics: name Execution: ( -- a-addr ) |
| DISASSEMBLE | ( len a-addr -- ) |  |
| EXECUTE | ( i*x xt -- j*x ) | Remove xt from the stack and perform the semantics identified by it. Other stack effects are due to the word EXECUTEd. |
| EXIT | Execution: ( -- ) ( R: nest-sys -- ) | Return control to the calling definition specified by nest-sys. Before executing EXIT within a do-loop, a program shall discard the loop-control parameters by executing UNLOOP. |
| HERE | ( -- addr ) | addr is the data-space pointer. |
| IMMEDIATE | ( -- ) | Make the most recent definition an immediate word. |
| LATEST | ( -- xt ) |  |
| LITERAL | Compilation: ( x -- ), Runtime: ( -- x ) | Append the run-time semantics to the current definition. |
| MOVE | ( a1 a2 u --  ) |  |
| NAME> | ( xt -- len a-addr) | the address and length of the name of the execution token. |
| PARSE | ( char \ccc<char>\" -- c-addr u )" | Parse ccc delimited by the delimiter char. c-addr is the address (within the input buffer) and u is the length of the parsed string. If the parse area was empty, the resulting string has a zero length. |
| SOURCE | ( -- c-addr u ) | c-addr is the address of, and u is the number of characters in, the input buffer. |
| THROW | ( i*x -- ) |  |
| VARIABLE | ( \<spaces>name\" -- )" | Skip leading space delimiters. Parse name delimited by a space. Create a definition for name with the execution semantics: `name Execution: ( -- a-addr )`. Reserve one cell of data space at an aligned address. |
| WORD | ( char \<chars>ccc<char>\" -- c-addr )" | Skip leading delimiters. Parse characters ccc delimited by char.  |
| WORDS | ( -- ) | List the definition names in alphabetical order. |

### forth/src/primitives/misc.c

| Word | Stack Effect | Description |
|------|--------------|-------------|
| DUMP | ( n addr -- ) | Dumps n bytes starting from addr. |
| LICENSE | ( -- ) | displays the MIT license text. |

### forth/src/primitives/stack_manip.c

| Word | Stack Effect | Description |
|------|--------------|-------------|
| -ROT | ( x1 x2 x3 -- x3 x1 x2 ) | rotate the top three stack entries. |
| 2DROP | ( x1 x2 -- ) | drop cell pair x1 x2 from the stack. |
| 2DUP | ( x1 x2 -- x1 x2 x1 x2 ) | duplicate cell pair x1 x2. |
| 2OVER | ( x1 x2 x3 x4-- x1 x2 x3 x4 x1 x2) | copy cell pai x1 x2 to the top of the stack. |
| 2SWAP | ( x1 x2 x3 x4 -- x3 x4 x2 x1) | exchange the top two cell pairs. |
| >R | ( x -- )  ( R:  -- x) | move x to the return stack. |
| ?DUP | ( x -- 0 \| x x ) | duplicate top stack element if it is non-zero. |
| DEPTH | ( -- n ) | the number of single-cell values contained in the data stack before n was placed on the stack. |
| DROP | ( x -- ) | drop top stack element. |
| DUP | ( x -- x x ) | duplicate top stack element. |
| NIP | ( x1 x2 -- x2 ) | remove NOS. |
| OVER | ( x1 x2 -- x1 x2 x1) | copy NOS (next of stack) to top of stack. |
| PICK | ( xu ... x1 x0 u -- xu ... x1 x0 xu ) | remove u. Copy the xu to the top of the stack. |
| R> | ( -- x ) ( R:  x -- ) | move x from the return stack to the data stack. |
| R@ | ( -- x ) ( R:  x -- x) | copy x from the return stack to the data stack. |
| RDEPTH | ( -- n ) | the number of single-cell values contained in the return stack. |
| RDROP | ( -- ) ( R:  x -- ) | drop top return stack element. |
| ROT | ( x1 x2 x3 -- x2 x3 x1 ) | rotate the top three stack entries. |
| SWAP | ( x1 x2 -- x2 x1) | swap top two stack elements. |
| TUCK | ( x1 x2 -- x2 x1 x2 ) | copy the first (top) stack item below the second stack item. |



## TODO

Some basic kernel operations need writing before work on the interpreter can be started:

* ~~Scrolling TTY~~
* ~~Control character output~~
* ~~printf %d and %x formats~~
* Debug printing
* ~~Global Descriptor Table~~
* ~~Interrupt Descriptor Table~~
* ~~Interrupt Service Routines / Exception handling~~
* ~~Keyboard driver readline input~~
* ~~Memory allocator~~
* ~~sbrk implementation~~
* ~~libm implementation~~
* ~~spinlock implementation~~
* Fix compiler warnings
* Build instructions for Mac/Windows
* ~~assert~~
* ~~basic linked-list, stack, queue, hashtable implementations~~
* Virtual consoles
* ~~Readline history & tab completion~~
* Travis CI builds
* ~~atoi,~~ atof, ~~strdup, trim~~ implementations
* Extended memory / [Paging](http://wiki.osdev.org/Setting_Up_Paging)
* Disk access
* ~~Editor~~
* Support ARMv7 architecture
* Higher VGA display modes
* Selectable keyboard maps

Interpreter-proper tasks:

* ~~Memory space~~
* ~~Return stack~~
* ~~Compiler~~
* Expand word vocabulary
* Unit tests

## Contributors

Pull requests are always welcome. There is plenty to do, please let me know
if you can help out; submit a request for commit access.

## References

### Forth

* http://thinking-forth.sourceforge.net/
* http://www.jupiter-ace.co.uk/index_Forth_general.html
* https://en.wikipedia.org/wiki/Forth_(programming_language)
* http://wiki.forthfreak.net/index.cgi?jsforth
* http://www.complang.tuwien.ac.at/anton/euroforth/ef99/ertl99.pdf
* http://www.bradrodriguez.com/papers/bnfparse.htm
* http://colorforth.com/POL.htm
* http://foja.dcs.fmph.uniba.sk/kompilatory/docs/compiler.pdf
* http://yosefk.com/blog/my-history-with-forth-stack-machines.html
* http://wiki.laptop.org/go/Forth_Lesson_0
* http://pforth.googlecode.com/svn/trunk/fth/system.fth
* http://www.forth.com/starting-forth/index.html
* http://forth.sourceforge.net/standard/dpans/
* https://groups.google.com/forum/?hl=en#!forum/comp.lang.forth

### OS Development

* http://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf
* http://www.osdever.net/tutorials/view/brans-kernel-development-tutorial
* http://www.acm.uiuc.edu/sigops/roll_your_own/i386/idt.html
* http://wiki.osdev.org/Main_Page
* http://wiki.qemu.org/Main_Page
* https://web.archive.org/web/20090303224539/http://ibm.com/developerworks/linux/library/l-gas-nasm.html
* http://www.mathematik.uni-bielefeld.de/~rehmann/Ckurs-f/b04/alloc.h
* http://sourceware.org/newlib/
* http://www.netlib.org/fdlibm/
* http://geezer.osdevbrasil.net/osd/kbd
* http://www.jbox.dk/sanos/
* http://www.musl-libc.org/
* https://www.linux.com/learn/docs/ldp/605-linux-i386-boot-code-howto
* http://tldp.org/HOWTO/Linux-Init-HOWTO-3.html

### ARMv7 Toolchain

* http://www.kegel.com/crosstool/
* http://wiki.lochraster.org/wiki/ARM7_GNU_Toolchain

### Misc

* https://github.com/orangeduck/mpc
* http://libcello.org/
* http://lists.cs.uiuc.edu/pipermail/cfe-dev/2008-August/002670.html
* https://gcc.gnu.org/onlinedocs/gcc-4.0.4/gcc/Nested-Functions.html#Nested-Functions
* https://en.wikipedia.org/wiki/VGA-compatible_text_mode
* https://www.kernel.org/doc/Documentation/svga.txt
* http://www.osdever.net/FreeVGA/home.htm
* http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html

## License

The MIT License (MIT)

Copyright (c) 2015 Richard Hull

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
