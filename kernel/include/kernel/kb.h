#ifndef __KB_H
#define __KB_H

#include <stdio.h>
#include <stdlib.h>
#include <kernel/system.h>

#define SCANCODE_BACKSPACE  0x0e
#define SCANCODE_TAB        0x0f
#define SCANCODE_A          0x1e
#define SCANCODE_E          0x12
#define SCANCODE_X          0x2d
#define SCANCODE_ENTER      0x1c
#define SCANCODE_DELETE     0x53
#define SCANCODE_INSERT     0x52
#define SCANCODE_END        0x4f
#define SCANCODE_HOME       0x47
#define SCANCODE_PGDN       0x51
#define SCANCODE_PGUP       0x49
#define SCANCODE_UP         0x48
#define SCANCODE_LEFT       0x4b
#define SCANCODE_RIGHT      0x4d
#define SCANCODE_DOWN       0x50

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t extended:1;
    uint8_t control:1;
    uint8_t shift:1;
    uint8_t capslock:1;
    uint8_t numlock:1;
} flags_t;

typedef struct {
    flags_t flags;
    char keycode;
    unsigned char scancode;
} input_t;

#ifdef __cplusplus
}
#endif

#endif
