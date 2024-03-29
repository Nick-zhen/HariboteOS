#include <stdio.h>

#include "bootpack.h"
#include "fifo.h"
#include "graphic.h"
#include "int.h"
#include "io.h"
#include "keyboard.h"
#include "mouse.h"
#include "console.h"

void init_pic(void) {
  // 禁止所有中断
  io_out8(PIC0_IMR, 0xff);  // interrupt mask register
  io_out8(PIC1_IMR, 0xff);

  io_out8(PIC0_ICW1, 0x11); // 边缘触发模式
  io_out8(PIC0_ICW2, 0x20); // IRQ0-7由INT20-27接收
  io_out8(PIC0_ICW3, 1 << 2); // PIC1由IRQ2连接
  io_out8(PIC0_ICW4, 0x01); // 无缓冲区模式

  io_out8(PIC1_ICW1, 0x11); // 边缘触发模式
  io_out8(PIC1_ICW2, 0x28); // IRQ8-15由INT28-2f接收
  io_out8(PIC1_ICW3, 2); // PIC1由IRQ2连接
  io_out8(PIC1_ICW4, 0x01); // 无缓冲区模式

  io_out8(PIC0_IMR, 0xfb); // PIC1以外中断全部禁止
  io_out8(PIC1_IMR, 0xff); // 禁止全部中断
}


void int_handler27(int *esp) {
	io_out8(PIC0_OCW2, 0x67);

	return;
}

int *int_handler0c(int *esp) {
  struct Task *task = task_now();
  struct Console *cons = task->cons;
  char s[30];

  cons_putstr(cons, "\nINT 0C:\n Stack Exception.\n");
  sprintf(s, "EIP = %X\n", esp[11]);
  cons_putstr(cons, s);

  return &(task->tss.esp0);
}

int *int_handler0d(int *esp) {
  struct Task *task = task_now();
  struct Console *cons = task->cons;
  char s[30];

  cons_putstr(cons, "\nINT 0D:\n General Protected Exception.\n");
  sprintf(s, "EIP = %X\n", esp[11]);
  cons_putstr(cons, s);

  return &(task->tss.esp0);
}