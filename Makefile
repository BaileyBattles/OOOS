C_SOURCES = $(wildcard src/Kernel/*.cpp       \
					   src/Drivers/*.cpp      \
					   src/CPU/*.cpp          \
					   src/FS/*.cpp           \
					   src/Sys/*.cpp          \
					   src/Util/*.cpp         \
					   src/Memory/*.cpp       \
					   src/Lib/*.cpp          \
					   src/Lib/stdlib/*.cpp   \
					   )

BINUTILS = $(wildcard src/Binutils/*.cpp)

# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.cpp=.o    \
        src/CPU/Interrupt.o} 

# Change this if your cross-compiler is somewhere else
CC = i686-elf-gcc
GDB = gdb
# -g: Use debugging symbols in gcc
CFLAGS = -g -Iinclude -fno-rtti -fno-exceptions -ffreestanding -fno-builtin -Wall -nostdlib -fno-threadsafe-statics

kernel.iso: kernel.elf
	mkdir -p iso
	mkdir -p iso/boot
	mkdir -p iso/boot/grub
	touch iso/boot/grub/grub.cfg 

	echo 'set timeout=0'                  >  iso/boot/grub/grub.cfg
	echo 'set default=0'                >> iso/boot/grub/grub.cfg
	echo 'menuentry "OOOS" {'             >> iso/boot/grub/grub.cfg
	echo '   multiboot /boot/kernel.elf'  >> iso/boot/grub/grub.cfg
	echo '}'                              >> iso/boot/grub/grub.cfg

	cp '$<' iso/boot
	grub-mkrescue -o '$@' iso

run: kernel.elf
	qemu-system-i386 -kernel '$<' -m 512 -drive file=drive/storage.img
	
run-iso: kernel.iso
	qemu-system-i386 -cdrom '$<' -m 512

storage.img: scripts/writeBINFAT16.py
	dd if=/dev/zero of=drive/storage.img bs=1M count=1024
	python3 '$<'

binutils:
	make -C src/Binutils

# Used for debugging purposes
kernel.elf: src/Boot/Entry.o ${OBJ}
	i686-elf-ld -o $@ -T linker.ld $^ 

# Open the connection to qemu and load our kernel-object file with symbols
debug: kernel.elf
	qemu-system-i386 -s -S -kernel kernel.elf -drive file=drive/storage.img -m 512 -d guest_errors,int &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# Generic rules for wildcards
# To make an object, always compile from its .cpp
%.o: %.cpp ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf32 -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf *.img
	rm -rf src/Kernel/*.o \
		   src/Memory/*.o \
		   src/Boot/*.bin \
		   src/Drivers/*.o \
		   src/Boot/*.o \
		   src/CPU/*.o \
		   src/Util/*.o \
		   src/Sys/*.o \
		   src/FS/*.o \
		   src/Lib/*.o \
		   src/Lib/stdlib/*.o \
		   iso
