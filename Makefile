C_SOURCES = $(wildcard src/kernel/*.cpp   \
					   src/drivers/*.cpp  \
					   src/cpu/*.cpp      \
					   src/sys/*.cpp      \
					   src/util/*.cpp     \
					   )
# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.cpp=.o src/cpu/interrupt.o} 

# Change this if your cross-compiler is somewhere else
CC = i686-elf-gcc
GDB = gdb
# -g: Use debugging symbols in gcc
CFLAGS = -g -Iinclude -fno-rtti

# First rule is run by default
os-image.bin: src/boot/bootsect.bin kernel.bin
	cat $^ > os-image.bin

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
kernel.bin: src/boot/kernel_entry.o ${OBJ}
	i686-elf-ld -o $@ -Ttext 0x1000 $^ --oformat binary

# Used for debugging purposes
kernel.elf: src/boot/kernel_entry.o ${OBJ}
	i686-elf-ld -o $@ -Ttext 0x1000 $^ 

run: os-image.bin
	qemu-system-i386 -fda os-image.bin

# Open the connection to qemu and load our kernel-object file with symbols
debug: os-image.bin kernel.elf
	qemu-system-i386 -s -fda os-image.bin -d guest_errors,int &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# Generic rules for wildcards
# To make an object, always compile from its .cpp
%.o: %.cpp ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf
	rm -rf src/kernel/*.o \
		   src/boot/*.bin \
		   src/drivers/*.o \
		   src/boot/*.o \
		   src/cpu/*.o \
		   src/util/*.o \
		   src/sys/*.o
