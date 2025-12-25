CC = gcc
CFLAGS = -m32 -ffreestanding -fno-pic -fno-stack-protector -c
ASM = nasm

# Include libc in the sources
C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c libc/*.c)
OBJ = ${C_SOURCES:.c=.o}

all: os-image

run: os-image
	qemu-system-i386 -fda os-image

# --- FIX: Added libc/*.o to the delete list ---
clean:
	rm -f *.bin *.o os-image kernel/*.o boot/*.o drivers/*.o cpu/*.o libc/*.o

os-image: boot/boot.bin kernel.bin
	cat $^ > os-image

kernel.bin: boot/kernel_entry.o cpu/interrupt.o ${OBJ}
	ld -m elf_i386 -o $@ -Ttext 0x1000 $^ --oformat binary

# Compile assembly files
cpu/interrupt.o: cpu/interrupt.asm
	$(ASM) -f elf $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.asm
	$(ASM) -f elf $< -o $@

boot/boot.bin: boot/boot.asm
	$(ASM) -f bin $< -o $@