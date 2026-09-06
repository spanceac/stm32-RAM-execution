CROSS_COMPILE = arm-none-eabi-
CC      = $(CROSS_COMPILE)gcc
LD      = $(CROSS_COMPILE)ld -v
CP      = $(CROSS_COMPILE)objcopy
OD      = $(CROSS_COMPILE)objdump
  
CFLAGS  =  -I./ -c -fno-common -Os -mcpu=cortex-m3 -mthumb -g
LFLAGS  = -Tloader-stm32.ld
CPFLAGS = -Obinary
ODFLAGS = -S

all: test

clean:
	-rm -f loader.lst loader.o loader.elf loader.lst loader.bin

test: loader.elf
	@ echo "...copying"
	$(CP) $(CPFLAGS) loader.elf loader.bin
	$(OD) $(ODFLAGS) loader.elf > loader.lst

loader.elf: loader.o loader-stm32.ld
	@ echo "..linking"
	$(LD) $(LFLAGS) -o loader.elf loader.o

loader.o: loader.c
	@ echo ".compiling"
	$(CC) $(CFLAGS) loader.c


