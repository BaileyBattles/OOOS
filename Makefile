C_SOURCES = $(wildcard src/kernel/*.cpp   \
					   src/drivers/*.cpp  \
					   src/cpu/*.cpp      \
					   src/sys/*.cpp      \
					   src/util/*.cpp     \
					   src/memory/*.cpp   \
					   )
# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.cpp=.o    \
        src/cpu/interrupt.o} 

# Change this if your cross-compiler is somewhere else
CC = i686-elf-gcc
GDB = gdb
# -g: Use debugging symbols in gcc
CFLAGS = -g -Iinclude -fno-rtti -ffreestanding -fno-builtin -Wall

kernel.img: kernel.elf
	mkdir -p iso
	mkdir -p iso/boot
	mkdir -p iso/boot/grub
	touch iso/boot/grub/grub.cfg 

	echo 'set timeout=0'                  >  iso/boot/grub/grub.cfg
	echo 'set default="0"'                >> iso/boot/grub/grub.cfg
	echo 'menuentry "OOOS" {'             >> iso/boot/grub/grub.cfg
	echo '   multiboot /boot/kernel.elf'  >> iso/boot/grub/grub.cfg
	echo '}'                              >> iso/boot/grub/grub.cfg

	cp '$<' iso/boot
	grub-mkrescue -o '$@' iso

run: kernel.elf
	qemu-system-i386 -kernel '$<' -m 512

# Used for debugging purposes
kernel.elf: src/boot/entry.o ${OBJ}
	i686-elf-ld -o $@ -T linker.ld $^ 

# Open the connection to qemu and load our kernel-object file with symbols
debug: kernel.elf
	qemu-system-i386 -s -S -kernel kernel.elf -m 512 -d guest_errors,int &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# Generic rules for wildcards
# To make an object, always compile from its .cpp
%.o: %.cpp ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf32 -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf *.img
	rm -rf src/kernel/*.o \
		   src/memory/*.o \
		   src/boot/*.bin \
		   src/drivers/*.o \
		   src/boot/*.o \
		   src/cpu/*.o \
		   src/util/*.o \
		   src/sys/*.o \
		   iso
