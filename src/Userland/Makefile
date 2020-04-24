CXXFLAGS = -g -I../../include -fno-rtti -fno-exceptions -ffreestanding -fno-builtin -Wall -nostdlib -fno-threadsafe-statics
CC = i686-elf-gcc

OBJ = 		[$(wildcard ../Kernel/*.o       \
				../Drivers/*.o      \
				../CPU/*.o          \
				../FS/*.o           \
				../Sys/*.o          \
				../Util/*.o         \
				../Memory/*.o       \
				../Lib/*.o          \
				../Lib/stdlib/*.o   \
				)]

default:
	$(CC) $(CXXFLAGS) -o hello_world.o hello_world.cpp ../Drivers/Screen.o ../Sys/Io.o ../Util/Memcpy.o -Ttext 0x1001000 -Tbss 0x1008000 -e 0x1001000
	$(CC) $(CXXFLAGS) -o sh.o sh.cpp ../Drivers/Screen.o ../Sys/Io.o ../Util/Memcpy.o -Ttext 0x1001000 -Tbss 0x1008000 -e 0x1001000

#$(CC) $(CXXFLAGS) -o hello_world.o hello_world.cpp ../Drivers/Screen.o ../Sys/Io.o ../Util/Memcpy.o -Ttext 0x1000000 -e 0x1001000