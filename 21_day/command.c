#include <stdio.h>

#include "bootpack.h"
#include "command.h"
#include "console.h"
#include "desctbl.h"
#include "fs.h"
#include "graphic.h"
#include "memory.h"
#include "sheet.h"
#include "task.h"
#include "app.h"

void cmd_mem(struct Console *cons, unsigned int memtotal) {
    struct MemMan *memman = (struct MemMan *)MEMMAN_ADDR;
    char s[60];

    sprintf(s, "total   %dMB\nfree %dKB\n\n", memtotal / (1024 * 1024), memman_total(memman) / 1024);
    cons_putstr(cons, s);
}

void cmd_clear(struct Console *cons) {
    struct Sheet *sheet = cons->sheet;

    for (int y = 28; y < 28 + 128; y++) {
        for (int x = 8; x < 8 + 240; x++) {
            sheet->buf[x + y * sheet->bxsize] = COL8_000000;
        }
    }

    sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
    cons->cur_y = 28;
}

void cmd_ls(struct Console *cons) {
    struct FileInfo *finfo = (struct FileInfo *)(ADR_DISKIMG + 0x002600);
    char s[30];

    for (int i = 0; i < 224; i++) {
        if (finfo[i].name[0] == '\0') {
            break;
        }

        if (finfo[i].name[0] != 0xe5) {
            if (!(finfo[i].type & 0x18)) {
                sprintf(s, "filename.ext   %d\n", finfo[i].size);

                for (int j = 0; j < 8; j++) {
                    s[j] = finfo[i].name[j];
                }
                s[9] = finfo[i].ext[0];
                s[10] = finfo[i].ext[1];
                s[11] = finfo[i].ext[2];

                cons_putstr(cons, s);
            }
        }
    }

    cons_newline(cons);
}

void cmd_cat(struct Console *cons, int *fat, char *cmdline) {
    struct MemMan *memman = (struct MemMan *)MEMMAN_ADDR;
    struct FileInfo *finfo = file_search(cmdline + 4, (struct FileInfo *)(ADR_DISKIMG + 0x002600), 224);
    char *p;

    if (finfo) {      /*找到文件的情况*/
        p = (char *)memman_alloc_4k(memman, finfo->size);
        file_load_file(finfo->clustno, finfo->size, p, fat, (char *)(ADR_DISKIMG + 0x003e00));
        cons_putnstr(cons, p, finfo->size);
        memman_free_4k(memman, (int)p, finfo->size);
    } else {          /*没有找到文件的情况*/
        cons_putstr(cons, "File not found.\n");
    }

    cons_newline(cons);
}

void cmd_hlt(struct Console *cons, int *fat) {
    struct MemMan *memman = (struct MemMan *) MEMMAN_ADDR;
    struct FileInfo *finfo = file_search("HELLO.HRB", (struct FileInfo *)(ADR_DISKIMG + 0x002600), 224);
    struct SegmentDescriptor *gdt = (struct SegmentDescriptor *) ADR_GDT;
    char *p;

    if (finfo) {      /*找到文件的情况*/
        char *p = (char *) memman_alloc_4k(memman, finfo->size);
        file_load_file(finfo->clustno, finfo->size, p, fat, (char *)(ADR_DISKIMG + 0x003e00));
        set_segmdesc(gdt + 1003, finfo->size - 1, (int) p, AR_CODE32_ER);
        far_call(0, 1003 * 8);
        memman_free_4k(memman, (int) p, finfo->size);
    } else {          /*没有找到文件的情况*/
        cons_putstr(cons, "File not found.\n");
    }

    cons_newline(cons);
}

int cmd_app(struct Console *cons, int *fat, char *cmdline) {
    struct MemMan *memman = (struct MemMan *)MEMMAN_ADDR;
    struct FileInfo *finfo;
    struct SegmentDescriptor *gdt = (struct SegmentDescriptor *)ADR_GDT;
    struct Task *task = task_now();
    char name[18], *p, *q;
    int i;

    /*根据命令行生成文件名*/
    for (i = 0; i < 13; i++) {
        if (cmdline[i] <= ' ') {
            break;
        }

        name[i] = cmdline[i];
    }
    name[i] = '\0'; /*暂且将文件名的后面置为0*/

    /*寻找文件 */
    finfo = file_search(name, (struct FileInfo *)(ADR_DISKIMG + 0x002600), 224);
    if (finfo == NULL && name[i - 1] != '.') {
        /*由于找不到文件，故在文件名后面加上“.hrb”后重新寻找*/
        name[i] = '.';
        name[i + 1] = 'H';
        name[i + 2] = 'R';
        name[i + 3] = 'B';
        name[i + 4] = '\0';

        finfo = file_search(name, (struct FileInfo *)(ADR_DISKIMG + 0x002600), 224);
    }

    if (finfo) {
        /*找到文件的情况*/
        p = (char *)memman_alloc_4k(memman, finfo->size);
        char *q = (char *)memman_alloc_4k(memman, 64 * 1024);
        *((int *) 0xfe8) = (int) p;
        file_load_file(finfo->clustno, finfo->size, p, fat, (char *)(ADR_DISKIMG + 0x003e00));
        
        set_segmdesc(gdt + 1003, finfo->size - 1, (int)p, AR_CODE32_ER + 0x60);
        set_segmdesc(gdt + 1004, 64 * 1024 - 1, (int)q, AR_DATA32_RW + 0x60); // The memory for application

        start_app(0, 1003 * 8, 64 * 1024, 1004 * 8, &(task->tss.esp0));

        memman_free_4k(memman, (int)p, finfo->size + 6);
        memman_free_4k(memman, (int)q, 64 * 1024);
        cons_newline(cons);

        return 1;
    }
    /*没有找到文件的情况*/
    return 0;
}