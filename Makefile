
CXXFLAGS = -m32 -Wall -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings -nostdlib -g
ASFLAGS = --32
LDFLAGS = -melf_i386
NASMFLAGS = -g
LD = i686-elf-ld
AS = i686-elf-as
NASM = nasm

GCC=i686-elf-gcc
GDB=gdb
objects = 	obj/loader.o \
			obj/drivers/screen.o \
			obj/kernel/kernel.o  \
			obj/sys/interrupt.o \
			obj/sys/interrupt_asm.o \
			obj/sys/io.o \
			obj/util/atoi.o \
			obj/util/memcpy.o
 
default: kernel.bin

run: kernel.bin
	qemu-system-i386 -fda kernel.bin

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	$(GCC) $(CXXFLAGS) -c -o $@ $<

obj/%.o: src/%.asm
	mkdir -p $(@D)
	$(NASM) $(NASMFLAGS) -f elf32 -g -F dwarf -o $@ $<

kernel.bin: linker.ld $(objects)
	$(GCC) -T $< -o kernel.bin -ffreestanding -nostdlib $(objects)

kernel.elf: debug_linker.ld $(objects)
	$(GCC) -T $< -o kernel.elf -ffreestanding -nostdlib $(objects)

clean:
	-rm -rf obj iso kernel.iso kernel.elf kernel.bin isodir

debug: kernel.bin kernel.elf
	qemu-system-i386 -s -fda kernel.bin -S &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"


# kernel.iso: kernel.bin
# 	mkdir -p isodir/boot/grub
# 	cp kernel.bin isodir/boot/kernel.bin
# 	cp grub.cfg isodir/boot/grub/grub.cfg
# 	grub-mkrescue -o kernel.iso isodir