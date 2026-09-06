#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "stm32f10x.h"
#include "stm32-ints.h"

#define bit_set(var,bitno) ((var) |= 1 << (bitno))
#define bit_clr(var,bitno) ((var) &= ~(1 << (bitno)))
#define testbit(var,bitno) (((var)>>(bitno)) & 0x01)

#define SYS_CLK_HZ 24000000
#define TMR_FREQ_HZ 1
#define TMR_PRESC 1000
#define TMR_LOAD_VAL (((SYS_CLK_HZ) / (TMR_PRESC)) / (TMR_FREQ_HZ))

#define TMR2_IRQ_NO 28

static uint8_t cnt = 1;
static const char glob[] = "test123\r\n";

void IRQ_enable(uint8_t nr)
{
	uint8_t base = nr / 32;

	if (nr > 80) {
		/* unsupported IRQ NR */
		return;
	}

	bit_set(NVIC->ISER[base], nr % 32);
}

void TIM2_IRQHandler(void)
{
	/* clear UIF flag(IRQ) */
	bit_clr(TIM2->SR, 0);

	bool gpio_is_on = testbit(GPIOC->ODR, 9);

	if (cnt % 4 == 0) {
		gpio_is_on ? bit_clr(GPIOC->ODR, 9) : bit_set(GPIOC->ODR, 9);
		cnt = 1;
		return;
	}

	cnt++;
}

void timer2_init(void)
{
	bit_set(RCC->APB1ENR, 0); /* enable TMR2 clock */
	TIM2->ARR = TMR_LOAD_VAL;
	TIM2->PSC = TMR_PRESC - 1;
	TIM2->CR1 = 0;
	TIM2->CR2 = 0;
	bit_set(TIM2->CR1, 7); /* auto reload */
	bit_set(TIM2->CR1, 0); /* counter enable */
	bit_set(TIM2->DIER, 0); /* TMR2 interrupt enable */
}


void uart_init(void)
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
	//bit_set(USART1->CR1, 6); /* enable USART1 TX IRQ */
}

void uart_send_byte(char byte)
{
	USART1->DR = byte;
	while(testbit(USART1->SR, 6) == 0);
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

void set_gpio_out(GPIO_TypeDef *gpio_bank, uint8_t pin)
{
	vu32 *dest;
	uint8_t pin_offset;

	if (pin > 15)
		return;

	dest = pin > 7 ? &(gpio_bank->CRH) : &(gpio_bank->CRL);
	pin_offset = pin > 7 ? pin - 8 : pin;

	/* output 10MHz */
	bit_set(*dest, pin_offset * 4);
	bit_clr(*dest, pin_offset * 4 + 1);
	bit_clr(*dest, pin_offset * 4 + 2);
	bit_clr(*dest, pin_offset * 4 + 3);
}

void set_gpio_in(GPIO_TypeDef *gpio_bank, uint8_t pin)
{
	vu32 *dest;
	dest = pin > 7 ? &(gpio_bank->CRH) : &(gpio_bank->CRL);

	/* input with pull-up / pull-down */
	bit_clr(*dest, pin * 4);
	bit_clr(*dest, pin * 4 + 1);
	bit_clr(*dest, pin * 4 + 2);
	bit_set(*dest, pin * 4 + 3);
}

int main(void)
{
	set_fast_clk(); /* enable 24 MHz SYSCLK */

	RCC->APB2ENR |= 0x10 | 0x04 | 0x01; /* Enable GPIOC (bit 4) and GPIOA (bit 2) */

	uart_init();
	set_gpio_out(GPIOC, 9);

	bit_set(GPIOC->ODR, 9);

	timer2_init();
	IRQ_enable(TMR2_IRQ_NO);

	while (1) {
		int cnt = strlen(glob);
		for (int i = 0; i < cnt; i++) {
			uart_send_byte(glob[i]);
		}
	}

	return 0;
}

void nmi_handler(void)
{
	return;
}

void hardfault_handler(void)
{
	return;
}

