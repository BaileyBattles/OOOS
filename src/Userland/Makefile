CXXFLAGS = -g -I../../include -fno-rtti -fno-exceptions -ffreestanding -fno-builtin -Wall -nostdlib -fno-threadsafe-statics
CC = /home/bbattles/osdev/opt/cross/bin/i386-elf-gcc

default: ../Kernel/SyscallStubs.o
	$(CC) $(CXXFLAGS) -o sh.o sh.cpp '$<' -Ttext 0x81001000 -e 0x81001000
	$(CC) $(CXXFLAGS) -o ls.o ls.cpp '$<' -Ttext 0x81001000 -e 0x81001000