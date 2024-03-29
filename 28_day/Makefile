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
L_OBJS  := libc/stdio/stdio.bin libc/string/strcmp.bin libc/string/trim.bin libc/string/strncmp.bin \
			libc/stdlib/rand.bin
# Kernel Objects
K_OBJS	:= kernel/bootpack.bin kernel/io.bin kernel/pm.bin kernel/hankaku.bin \
		kernel/desctbl.bin kernel/graphic.bin kernel/int.bin \
		kernel/inthandler.bin kernel/fifo.bin kernel/keyboard.bin \
		kernel/mouse.bin kernel/memory.bin kernel/sheet.bin \
		kernel/window.bin kernel/timer.bin kernel/task_asm.bin \
		kernel/task.bin kernel/console.bin kernel/fs.bin \
		kernel/command.bin kernel/api.bin kernel/api_asm.bin \
		kernel/app.bin kernel/elf.bin
# Applications
APPS	:= app/a.hrb app/hello.hrb app/hello2.hrb app/hello3.hrb app/crack1.hrb app/bug1.hrb\
			app/crack2.hrb app/crack3.hrb app/crack4.hrb app/crack5.hrb app/crack6.hrb \
		    app/bug2.hrb app/bug3.hrb app/bug4.hrb app/hello5.hrb \
			app/winhello.hrb app/winhelo2.hrb app/winhelo3.hrb \
			app/star1.hrb app/stars.hrb app/stars2.hrb app/lines.hrb app/walk.hrb app/noodle.hrb \
			app/beepdown.hrb app/beepup.hrb app/color.hrb app/color2.hrb \
			app/sosu.hrb app/sosu2.hrb app/sosu3.hrb app/typeipl.hrb app/type.hrb

SYS		:= haribote.sys
IMG		:= haribote.img

ifdef DEBUG
QEMU_FLAGS += -gdb tcp::1234 -S
CFLAGS += -g
endif

kernel/ipl.bin:
	$(AS) -f bin kernel/ipl.asm -o kernel/ipl.bin -l kernel/ipl.lst

kernel/asmhead.bin:
	$(AS) -f bin kernel/asmhead.asm -o kernel/asmhead.bin -l kernel/asmhead.lst

kernel/hankaku.bin: ../hankaku/hankaku.asm
	$(AS) -f elf $< -o $@

kernel/%.bin: kernel/%.asm
	$(AS) -f elf $< -o $@ -l $(subst .asm,.lst,$<)

%.bin: %.c							# "-c" tells the compiler to generate an object file rather than an executable file
	$(CC) $(CFLAGS) -c $< -o $@		

# Applications
app/api.lib:
	$(AS) -f elf app/api.asm -o $@

%.hrb: %.asm
	$(AS) -f bin $< -o $@  -l $(subst .asm,.lst,$<)

app/%.o: app/%.c
	$(CC) -Wall -Wno-format -Wno-unused -std=gnu99 -m32 -I ./libc/include -c $< -o $@

app/%.hrb: app/%.o app/api.lib
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie $^ -o $@

app/winhelo2.hrb: app/api.lib
	$(CC) -Wall -Wno-format -Wno-unused -std=gnu99 -m32 -c app/winhello2.c -o app/winhello2.o
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie app/winhello2.o app/api.lib -o app/winhelo2.hrb

app/winhelo3.hrb: app/api.lib
	$(CC) -Wall -Wno-format -Wno-unused -std=gnu99 -m32 -c app/winhello3.c -o app/winhello3.o
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie app/winhello3.o app/api.lib -o app/winhelo3.hrb

app/stars.hrb: app/stars.o app/api.lib libc/stdlib/rand.bin
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie $^ -o $@

app/stars2.hrb: app/stars2.o app/api.lib libc/stdlib/rand.bin
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie $^ -o $@

app/noodle.hrb: app/noodle.o app/api.lib libc/stdio/stdio.bin
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie $^ -o $@

app/color.hrb: app/api.lib
	$(CC) -Wall -Wno-format -Wno-unused -std=gnu99 -m32 -c app/color.c -o app/color.o
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie app/color.o app/api.lib -o app/color.hrb

app/color2.hrb: app/api.lib
	$(CC) -Wall -Wno-format -Wno-unused -std=gnu99 -m32 -c app/color2.c -o app/color2.o
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie app/color2.o app/api.lib -o app/color2.hrb

# app/carck7.hrb: app/api.o
# 	$(CC) -Wall -Wno-format -Wno-unused -std=gnu99 -m32 -c app/carck7.c -o app/carck7.o
# 	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie app/carck7.o app/api.o -o app/carck7.hrb

app/sosu.hrb: app/sosu.o app/api.lib libc/stdio/stdio.bin
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie $^ -o $@

app/sosu2.hrb: app/sosu2.o app/api.lib libc/stdio/stdio.bin
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie $^ -o $@

app/sosu3.hrb: app/sosu3.o app/api.lib libc/stdio/stdio.bin
	$(LD) -m elf_i386 --oformat elf32-i386 -e main -pie $^ -o $@

# Kernel
kernel/kernel.sys: ${K_OBJS} ${L_OBJS}
	$(LD) -m elf_i386 --oformat binary -o kernel/kernel.sys -T kernel/kernel.ld $^

haribote.sys: kernel/asmhead.bin kernel/kernel.sys
	cat kernel/asmhead.bin > haribote.sys
	cat kernel/kernel.sys >> haribote.sys

image: kernel/ipl.bin haribote.sys ${APPS}
	dd if=/dev/zero of=$(IMG) bs=512 count=2880
	dd if=kernel/ipl.bin of=$(IMG) bs=512 count=1 conv=notrunc
	mcopy -i $(IMG) haribote.sys ::/
	mcopy -i $(IMG) Makefile ::/
	mcopy -i $(IMG) ${APPS} ::/
	mcopy -i $(IMG) kernel/ipl.asm ::/

all: ${L_OBJS} ${K_OBJS} ${APPS} haribote.sys image

clean:
	rm -rf $(IMG)
	rm -rf *.bin
	rm -rf *.lst
	rm -rf *.obj
	rm -rf *.sys
	rm -rf app/*.hrb
	rm -rf **/*.bin
	rm -rf **/*.lib
	rm -rf **/*.lst
	rm -rf **/*.o
	rm -rf **/*.sys
	rm -rf **/**/*.bin

qemu: clean all
	$(QEMU) -fda $(IMG) $(QEMU_FLAGS)

.PHONY:
	all