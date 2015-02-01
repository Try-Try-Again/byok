#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <stddef.h>
#include <stdint.h>

#include <kernel/struct/registers.h>
#include <kernel/asm/io.h>
#include <kernel/asm/interrupt.h>
#include <kernel/asm/spinlock.h>

#include <kernel/kb.h>
#include <kernel/tty.h>
#include <kernel/readline.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void draw_logo();

extern void mmu_install();

extern void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
extern void gdt_install(void);

extern void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
extern void idt_install(void);

extern void isrs_install(void);

extern void irq_install(void);
extern void *irq_install_handler(int irq, void (*handler)(registers_t *r));
extern void irq_uninstall_handler(int irq);

extern void timer_install();
extern void timer_wait(int ticks);

extern char **dump(char *addr, int size, int columns);
extern int pager(char **text);

extern char *sbrk(unsigned bytes);

#ifdef __cplusplus
}
#endif

#endif
