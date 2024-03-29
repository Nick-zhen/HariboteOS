ifndef GCCPREFIX
GCCPREFIX := x86_64-elf-
endif

AS	:= nasm
CC	:= $(GCCPREFIX)gcc
LD	:= $(GCCPREFIX)ld
OBJCOPY	:= $(GCCPREFIX)objcopy
QEMU	:= qemu-system-i386

# disable warning
CFLAGS += -Wall -Wno-format -Wno-unused 
# static linking which tells the compiler to link the resulting object file statically
CFLAGS += -std=gnu99 -static -m32		
# "-I./libc/include" specifies the directory where the compiler should look for header files.
CFLAGS += -I ./include -I ./libc/include				
CFLAGS += -ffunction-sections -nostdlib -nostdinc -fno-builtin -ffreestanding
CFLAGS += -fno-pie

QEMU_FLAGS 	+= -no-reboot -d in_asm

# C Library Objects
L_OBJS  := libc/stdio/stdio.bin libc/string/strcmp.bin libc/string/trim.bin libc/string/strncmp.bin
# Kernel Objects
K_OBJS	:= bootpack.bin io.bin pm.bin hankaku.bin desctbl.bin graphic.bin \
		int.bin inthandler.bin fifo.bin keyboard.bin mouse.bin memory.bin \
		sheet.bin window.bin timer.bin task_asm.bin task.bin console.bin  \
		fs.bin command.bin api_asm.bin api.bin app.bin
# Applications
APPS	:= app/hello.hrb app/hello2.hrb app/a.hrb app/hello3.hrb app/crack1.hrb \
			app/crack2.hrb
SYS		:= haribote.sys
IMG		:= haribote.img

ifdef DEBUG
QEMU_FLAGS += -gdb tcp::1234 -S
CFLAGS += -g
endif

ipl.bin:
	$(AS) -f bin ipl.asm -o ipl.bin -l ipl.lst

asmhead.bin:
	$(AS) -f bin asmhead.asm -o asmhead.bin -l asmhead.lst

hankaku.bin: ../hankaku/hankaku.asm
	$(AS) -f elf $< -o $@

%.bin: %.asm
	$(AS) -f elf $< -o $@ -l $(subst .asm,.lst,$<)

%.bin: %.c							# "-c" tells the compiler to generate an object file rather than an executable file
	$(CC) $(CFLAGS) -c $< -o $@		

%.hrb: %.asm
	$(AS) -f bin $< -o $@  -l $(subst .asm,.lst,$<)

app/a_asm.o:
	$(AS) -f elf app/a_asm.asm -o app/a_asm.o

app/a.hrb: app/a_asm.o
	$(CC) -Wall -Wno-format -Wno-unused -std=gnu99 -m32 -c app/a.c -o app/a.o
	$(LD) -m elf_i386 --oformat binary -e main app/a.o app/a_asm.o -o app/a.hrb

app/hello3.hrb: app/a_asm.o
	$(CC) -Wall -Wno-format -Wno-unused -std=gnu99 -m32 -c app/hello3.c -o app/hello3.o
	$(LD) -m elf_i386 --oformat binary -e main app/hello3.o app/a_asm.o -o app/hello3.hrb

app/crack1.hrb: app/a_asm.o
	$(CC) -Wall -Wno-format -Wno-unused -std=gnu99 -m32 -c app/crack1.c -o app/crack1.o
	$(LD) -m elf_i386 --oformat binary -e main app/crack1.o app/a_asm.o -o app/crack1.hrb

kernel.sys: ${K_OBJS} ${L_OBJS}
	$(LD) -m elf_i386 --oformat binary -o kernel.sys -T kernel.ld $^

haribote.sys: asmhead.bin kernel.sys
	cat asmhead.bin > haribote.sys
	cat kernel.sys >> haribote.sys

image: ipl.bin haribote.sys ${APPS}
	dd if=/dev/zero of=$(IMG) bs=512 count=2880
	dd if=ipl.bin of=$(IMG) bs=512 count=1 conv=notrunc
	mcopy -i $(IMG) haribote.sys ::/
	mcopy -i $(IMG) Makefile ::/
	mcopy -i $(IMG) ${APPS} ::/


all: ${OBJS} haribote.sys image

clean:
	rm -rf *.bin
	rm -rf *.sys
	rm -rf *.obj
	rm -rf *.lst
	rm -rf $(IMG)
	rm -rf **/**/*.bin
	rm -rf app/*.hrb
	rm -rf **/*.o
	rm -rf **/*.lst

qemu: clean all
	$(QEMU) -fda $(IMG) $(QEMU_FLAGS)

.PHONY:
	all