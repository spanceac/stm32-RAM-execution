Executing an application in RAM on an ARM MCU.

# Why

Usually, we write code for a MCU, compile it and burn the generated image(binary, hex file etc.) to the flash memory of the MCU.
Every time the MCU is powered up or reset, it starts executing code(instructions) from flash memory.

Besides flash memory, our MCUs also contain RAM memory so the flashed code can load and store computed data.

A feature that is less used/known/explained in case of ARM MCUs, is their ability to execute code not only from flash memory, but from RAM as well.

Why would you want to execute code from RAM?

* you worn your flash from too many erase cycles and it cannot store code any longer(highly unlikely)
* your MCU doesn't have a flash memory(likely if your MCU "lives" in a FPGA)
* your MCU is part of a product that needs to dynamically load and execute firmware(more likely)
* your MCU is part of an automated pipeline that needs to test firmware very often, which would result in quick flash wear(likely)

# How

For the explanations below, the target is the ARM(Cortex-M3) MCU `STM32F100`, which has 128KB of flash and 8KB of RAM.

## Software prerequisites

- `make`
- `arm-none-eabi-gcc` compiler
- ARM version of `picolibc` C standard libray
- `openocd` if one wants to load the application via a hardware debugger
- `python` if one wants to load the application in RAM through serial port

The example application that is provided to be loaded and executed in RAM is linking with `picolibc` as standard C libray implementation.

**Note**: The usage of `picolibc`(or any libC at all) is not mandatory to execute an application from RAM. The example uses a standard C library in order to provide a template for a real world application.

Information on how to install `picolibc` can be found [here]()https://github.com/spanceac/stm32-picolibc-integration#installingand-maybe-building-it).
If you're using Ubuntu, you can easily `apt install` it.

## The linker script

Both RAM and flash memory are present in the memory map of the MCU. Flash is placed starting at `8000000h` address and RAM starting at `20000000h`.

An usual linker script defines the flash and RAM sections and instructs the linker to use the flash address space for code and the RAM address space for data.

For RAM execution, we need a linker script that completely ignores the flash and instructs the linker to use RAM addresses for both the code and the data.

You can compare a "normal" [linker script](https://github.com/spanceac/stm32-picolibc-integration/blob/main/stm32f100-picolibc.ld) with the one that is intended for [executing the application from RAM](ram-loadable-example/stm32_ram_load_picolibc.ld).

## A STM32F100 MCU boots

When a STM32F100 MCU resets(by power up, reset pin etc.) it loads two values from its flash memory:

- the first word(4 bytes) from the very beginning of flash(offset 0) is loaded into `SP` register to be used as stack pointer base address(top of stack)

- the following word(4 bytes) from flash(offest 4) contain the user code start address and are loaded into `PC` register. This step starts the user code execution.

When executing an application from RAM, loading the values above from flash is useless.
For RAM execution, we need to first place an image in RAM memory and then load the `PC` and `SP` registers with the application's specific values.
How to do this is explained below.

## Loading via debugger

We can use the hardware debugger(usually through SWD pins for STM32) with `openOCD` tool to load the application in RAM and start its execution.

I described [here](https://github.com/spanceac/dma-demo#how-to-test-the-code) how to setup openocd on a Linux system for a STM32F100 Discovery board.

These are the OpenOCD instructions necessary to load a binary in RAM and execute it:

```
reset halt

# set RAM address where to load image(must match the RAM start address from linker script)
set load_addr 0x20000000

set sp_initial_addr $load_addr
set pc_initial_addr $($load_addr + 4)

load_image /path/to/firmware.bin $load_addr

# read word from address 0 of image and load it into SP register
reg sp [mrw $sp_initial_addr]

# read word from address 4 of image and load it into PC register
reg pc [mrw $pc_initial_addr]

# start execution at PC
resume
```

## Loading via my bootloader

As an alternative for debugger loading of application into RAM, we can use a bootloader.
The bootloader will get the user program binary through a communication channel, load it into RAM and execute it.
This bootloader needs to be flashed into the MCU so that it starts when the MCU resets.

As a proof of concept, I wrote a minimal bootloader that uses UART port to receive and load the binary into RAM.
The implementation is far from being resilient to communication loss or synchronization issues.
It was written just as a proof of concept and it serves its purpose.
Of course, a counterpart program that runs on a computer and sends the binary to the bootloader was written.

### RAM partitioning when using the bootloader

The bootloader needs to use some RAM for its variables. For this, we reserve 512 bytes at RAM offset 0 to be used by the bootloader.
The remaining part of RAM will be used for the application. In order to achieve this RAM partioning, we need to:
* set into bootloader's linker script RAM origin address at offset 0 and size of RAM to 512 bytes
* set bootloader's top of stack at 512 bytes
* set into application's linker script the RAM origin address at offest 512 and size of RAM to 512 bytes less than total available RAM

A RAM application that was set up to be loaded and started by the bootloader can be loaded through the debugger as well, as long as you take care to load it at the offset 512 of RAM.

### UART communication protocol

The bootloader works in UART receiver mode only, it sends nothing to the computer.
The python script that sends the binary to the MCU's bootloader will not receive any feedback from the MCU.
There is an integrity check, a CRC32 checksum that is computed on both sides, by the python script and the bootloader.

This is the data sent by the computer script:
- `binary_size`: 3 bytes(24bit LE)
- `binary_CRC32`: 4 bytes(32bit LE)
- the binary content

The bootloader uses UART to receive first the `binary_size`, then the `binary_CRC32`. After that it will wait to receive and copy into RAM a count of `binary_size` bytes.
After it receives this data, it will compute the CRC32 checksum of what it copied into RAM and see if it matches the `binary_CRC32` checksum received from the computer.
If it does match, it will jump execution to the binary from RAM.

The STM32F100 MCU uses USART1 serial port(RX pin PA10) to receive the data from the computer. The UART settings are 115200 baud, 8 data bits, 1 stop bit, no parity, no handshake.

### Jumping to the RAM application

After the bootloader copies into RAM the application and validates its checksum, it needs to start executing it.
In preparation for execution, the bootloader needs to first load the `SP` register with the value desired by the application.
For this it will load the `SP` with the word(4 bytes) value stored at offset 0 of the application

Then final step is to branch and execute the application. The application's entry point address will be read by the bootloader from the word value stored at offset 4 of application. The address is loaded in the `PC` register, which starts the RAM code execution.

## Loading via STM32 internal bootloader

*Note*: This section is a later addition since the first release of this document.

When I wrote [my bootloader](#loading-via-my-bootloader) I was not aware that STM32 built-in bootloaders are capable of copying code to RAM and jump to its execution. But thanks to reddit people I found out that it can. I was imagining that it only works for flashing.

It turns out that the bootloader I wrote is very similar to the STM32 built-in bootloader.

STM32 devices have an internal bootloader that can be used to transfer a firmware binary in their flash or RAM and start its execution.
For each STM32 MCU type, in this [application note](https://www.st.com/resource/en/application_note/an2606-introduction-to-system-memory-boot-mode-on-stm32-mcus-stmicroelectronics.pdf) it is documented:
* how the built-in bootloader can be activated
* which communication methods are available(UART, CAN, USB etc.)
* bootloader's RAM usage
* etc.

For STM32F100 MCUs the communication with the bootloader happens over the UART port according to this [protocol](https://www.st.com/resource/en/application_note/an3155-how-to-use-usart-protocol-in-bootloader-on-stm32-mcus-stmicroelectronics.pdf).

Luckily, developers already wrote the [stm32loader](https://pypi.org/project/stm32loader/) python tool for interacting with the bootloader over UART.

On STM32F100, for the built-in bootloader, the operation mode is:
* tie pin `Boot0` to logical 1 and `Boot1` to logical 0 to activate it
* UART port is *USART1* port with pin PA10 as RX and pin PA9 as TX
* bootloader needs the first 512 bytes of RAM

Since the built-in bootloader needs the first 512 bytes of RAM(exactly as the one written by me), this [RAM partitioning](#ram-partitioning-when-using-the-bootloader) scheme applies for the targeted firmware.

The built-in bootloader can be asked to start code execution which will automatically set `PC` and `SP` registers to the correct values, (again) exactly as the bootloader written by me [does](#jumping-to-the-ram-application).

In order to load an image to 512 bytes RAM offset and start its execution through the built-in bootloader, the following command applies:
```
stm32loader -p UART_PORT -f F1 -w FIRMWARE.bin -a 0x20000200 -v -g 0x20000200
```

Replace `UART_PORT` with the path to your UART port and `FIRMWARE.bin` with the path to the targeted firmware.

## Interrupts

When an interrupt occurs on the STM32 MCU, the MCU reads from the **IVT**(Interrupt Vector Table) the address of the interrupt handler corresponding to that interrupt.
The IVT is placed at the beginning of the flash.
If we want an application that runs exclusively from RAM and also provides interrupt handling, we cannot write the flash memory with the application's interrupt hanlder.
Luckily, the MCU provides interrupt vector rellocation through the **VTOR** register. This register will be loaded with the address of the desired **IVT**.

The following instruction takes care of rellocating the IVT when an application is loaded into RAM through `openocd`:

```
# write VTOR
mww 0xE000ED08 $load_addr
```

If we're using [my bootloader](#loading-via-my-bootloader) to load the application to RAM, the IVT rellocation is handled by the provided bootloader code.

The STM32 [built-in bootloader](#loading-via-stm32-internal-bootloader) doesn't take care of IVT rellocation, so a solution to this problem is to set the **VTOR** register in the application code, before enabling interrupts.

**Note**: Some ARM MCU variants(like Cortex-M0) do not provide the VTOR register. For some variants is optional and for others is mandatory.

# Inside this repo

## The bootloader code

Lives in [loader](loader) folder. The bootloader is used to receive the firmware from a computer via a serial port, copy the firmware to RAM and start its execution.

Type `make` to compile it and then flash `loader.bin` in the MCU.

## The firmware transmitting python script

Lives in [host-loader](host-loader) folder. Contains the python script that sends over serial port the firmware targeted for RAM execution to the MCU flashed bootloader.

Invocation:
```
python host_loader.py UART_PORT FIRMWARE_PATH
``` 

Running depends on pip package `pyserial`.

## The example application code

Lives in [ram-loadable-example](ram-loadable-example) folder. Run `make` to compile it. For successful compilation, `picolibc` is needed, see [software prerequisites](#software-prerequisites).

The resulted firmware(`example.bin`) is compiled by default to be loadable through the bootloader. If you want to load it with a hardware debugger, use instructions from this [section](#loading-via-debugger) with `load_addr` set to `0x20000200`. For this example, [interrupt relocation](#interrupts) is needed.

When running, the firmware is supposed to:
* send in a loop the string `test123` over UART1 port(PA9 being TX pin)
* blink the LED connected at pin PC9, using TIMER2 interrupt

# "Funny" stuff :slightly_smiling_face:

1. Many(but not all) ARM MCUs contain a `MPU`(Memory Protection Unit), which can be used to tag RAM region(s) as `XN`(eXecute Never) and prevent execution of RAM code(by triggering a memory management fault). This is a security feature that is not enabled by default.

2. `snprintf` implementation provided by `picolibc` takes ~7kB of memory. An application that attemps to use this function will not fit in the 8KB available on a STM32F100. Fortunately, there are MCUs with bigger RAM.
