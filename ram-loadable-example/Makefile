CROSS_COMPILE = arm-none-eabi-
CC      = $(CROSS_COMPILE)gcc
LD      = $(CROSS_COMPILE)ld -v
CP      = $(CROSS_COMPILE)objcopy
OD      = $(CROSS_COMPILE)objdump

CFLAGS  =  -I./ -fno-common -O0 -mcpu=cortex-m3 -mthumb -g -Wall -specs=picolibc.specs
LFLAGS  = -Tstm32_ram_load_picolibc.ld
CPFLAGS = -Obinary
ODFLAGS = -S

all: example

clean:
	-rm -f *.lst *.o *.elf *.bin

example: example.elf
	@ echo "...copying"
	$(CP) $(CPFLAGS) example.elf example.bin
	$(OD) $(ODFLAGS) example.elf > example.lst

example.elf: example.c stm32_ram_load_picolibc.ld
	$(CC) $(CFLAGS) $(LFLAGS) example.c -o example.elf
