#include "stm32f10x.h"
#include "stm32-ints.h"

#include <stdint.h>
#include <stddef.h>

extern char ram_app_load_addr[];

#define bit_set(var,bitno) ((var) |= 1 << (bitno))
#define bit_clr(var,bitno) ((var) &= ~(1 << (bitno)))
#define testbit(var,bitno) (((var)>>(bitno)) & 0x01)

static void uart_init(void)
{
	/* PA9 - UART1_TX -> output 10MHz, Alternate function output push-pull */
	bit_set(GPIOA->CRH, 7);
	bit_clr(GPIOA->CRH, 6);
	bit_clr(GPIOA->CRH, 5);
	bit_set(GPIOA->CRH, 4);

	bit_set(RCC->APB2ENR, 14); /* enable USART1 clock */
	USART1->BRR = 0x1a0; /* 26 mantissa, 0 fraction -> 115200 baud */
	bit_set(USART1->CR1, 15); /* enable OVER8 = 1  */
	bit_set(USART1->CR1, 13); /* enable USART1 */
	bit_clr(USART1->SR, 6); /* clear UART transmission complete flag */
	bit_set(USART1->CR1, 3); /* enable USART1 TX */
	bit_set(USART1->CR1, 2); /* enable USART1 RX */
}

static void uart_send_byte(uint8_t byte)
{
	USART1->DR = byte;
	while(testbit(USART1->SR, 6) == 0);
}

static uint8_t uart_read_byte(void)
{
	while(testbit(USART1->SR, 5) == 0);
	return USART1->DR & 0xff;
}

void set_fast_clk(void)
{
	/* setting SYSCLK to 24 MHz internal clock
	SYSCLK = (HSI / 2) * PLL_MUL = (8 / 2) * 6 = 24 MHz */ 
	bit_set(RCC->CFGR, 20); /* PLL x 6 (must be set while PLL off) */
	bit_set(RCC->CR, 24); /* PLL ON */
	bit_set(RCC->CFGR, 1); /* PLL CLK is SYSCLK */
	while(testbit(RCC->CR, 25) == 0); /* wait for PLLRDY */
}

uint32_t crc32_ieee_update(uint32_t crc, const uint8_t *data, size_t len)
{
	/* crc table generated from polynomial 0xedb88320 */
	const uint32_t table[16] = {
		0x00000000U, 0x1db71064U, 0x3b6e20c8U, 0x26d930acU, 0x76dc4190U, 0x6b6b51f4U,
		0x4db26158U, 0x5005713cU, 0xedb88320U, 0xf00f9344U, 0xd6d6a3e8U, 0xcb61b38cU,
		0x9b64c2b0U, 0x86d3d2d4U, 0xa00ae278U, 0xbdbdf21cU,
	};

	crc = ~crc;

	for (size_t i = 0; i < len; i++) {
		uint8_t byte = data[i];

		crc = (crc >> 4) ^ table[(crc ^ byte) & 0x0f];
		crc = (crc >> 4) ^ table[(crc ^ ((uint32_t)byte >> 4)) & 0x0f];
	}

	return (~crc);
}

uint32_t crc32_ieee(const uint8_t *data, size_t len)
{
	return crc32_ieee_update(0x0, data, len);
}

void main(void){
	set_fast_clk(); /* enable 24 MHz SYSCLK */

	uart_init();

	uint32_t fw_len = 0;
	uint32_t fw_crc32 = 0;

	/* we use 24 bits length */
	for (size_t i = 0; i < 3; i++) {
		fw_len |= uart_read_byte() << (i * 8);
	}

	for (size_t i = 0; i < 4; i++) {
		fw_crc32 |= uart_read_byte() << (i * 8);
	}

	uint8_t *ram_load_dest;
	ram_load_dest = (void *)ram_app_load_addr;
	for (size_t i = 0; i < fw_len; i++) {
		ram_load_dest[i] = uart_read_byte();
	}

	uint32_t calc_crc32 = crc32_ieee(ram_load_dest, fw_len);

	if (calc_crc32 != fw_crc32) {
		while(1);
	}

	/* TODO: reset UART regs */

	uint32_t ram_app_sp_start_val = (uint32_t)(*(uint32_t *)(ram_app_load_addr));

	uint32_t *ram_app_start_addr = (uint32_t *)(ram_app_load_addr + 4);
	void (*ram_app)(void) = (void *)(*ram_app_start_addr);

	/* interrupt remap to RAM application vector table */
	SCB->VTOR = (uint32_t)ram_app_load_addr;

	/* set SP to the value set in the RAM application */
	__asm__("mov sp, %0" : : "r" (ram_app_sp_start_val));
	ram_app();

	/* should not be reachable, but it's needed. If we don't add it,
	   the compiler considers ram_app() call the exit point of main()
	   and tries to drop its stack, affecting SP
	 */
	while(1);
}

void nmi_handler(void)
{
	return;
}

void hardfault_handler(void)
{
	return;
}
