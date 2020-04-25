CXXFLAGS = -g -I../../include -fno-rtti -fno-exceptions -ffreestanding -fno-builtin -Wall -nostdlib -fno-threadsafe-statics
CC = i686-elf-gcc

OBJ = 		$(wildcard ../Drivers/*.o      \
				../CPU/*.o          \
				../FS/*.o           \
				../Sys/*.o          \
				../Util/*.o         \
				../Memory/*.o       \
				../Lib/*.o          \
				../Lib/stdlib/*.o   \
				../Process/*.o      \
				../Kernel/Syscall.o \
				)

default:
	$(CC) $(CXXFLAGS) -o hello_world.o hello_world.cpp ../Drivers/Screen.o ../Sys/Io.o ../Util/Memcpy.o -Ttext 0x81000000 -Tbss 0x81008000 -e 0x81001000
	$(CC) $(CXXFLAGS) -o sh.o sh.cpp $(OBJ) -Ttext 0x81001000 -e 0x81001000