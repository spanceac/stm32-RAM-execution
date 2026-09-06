/* Vector Table */

void _start (void) __attribute__ ((weak));
void NMI_Handler (void) __attribute__ ((weak));
void HardFault_Handler (void) __attribute__ ((weak));
void MemManage_Handler (void) __attribute__ ((weak));
void BusFault_Handler (void) __attribute__ ((weak));
void UsageFault_Handler (void) __attribute__ ((weak));
void SVC_Handler (void) __attribute__ ((weak));
void DebugMon_Handler (void) __attribute__ ((weak));
void PendSV_Handler (void) __attribute__ ((weak));
void SysTick_Handler (void) __attribute__ ((weak));
void WWDG_IRQHandler (void) __attribute__ ((weak));
void PVD_IRQHandler (void) __attribute__ ((weak));
void TAMPER_STAMP_IRQHandler (void) __attribute__ ((weak));
void RTC_WKUP_IRQHandler (void) __attribute__ ((weak));
void FLASH_IRQHandler (void) __attribute__ ((weak));
void RCC_IRQHandler (void) __attribute__ ((weak));
void EXTI0_IRQHandler (void) __attribute__ ((weak));
void EXTI1_IRQHandler (void) __attribute__ ((weak));
void EXTI2_IRQHandler (void) __attribute__ ((weak));
void EXTI3_IRQHandler (void) __attribute__ ((weak));
void EXTI4_IRQHandler (void) __attribute__ ((weak));
void DMA1_Channel1_IRQHandler (void) __attribute__ ((weak));
void DMA1_Channel2_IRQHandler (void) __attribute__ ((weak));
void DMA1_Channel3_IRQHandler (void) __attribute__ ((weak));
void DMA1_Channel4_IRQHandler (void) __attribute__ ((weak));
void DMA1_Channel5_IRQHandler (void) __attribute__ ((weak));
void DMA1_Channel6_IRQHandler (void) __attribute__ ((weak));
void DMA1_Channel7_IRQHandler (void) __attribute__ ((weak));
void ADC1_IRQHandler (void) __attribute__ ((weak));
void EXTI9_5_IRQHandler (void) __attribute__ ((weak));
void TIM1_BRK_TIM15_IRQHandler (void) __attribute__ ((weak));
void TIM1_UP_TIM16_IRQHandler (void) __attribute__ ((weak));
void TIM1_TRG_COM_TIM17_IRQHandler (void) __attribute__ ((weak));
void TIM1_CC_IRQHandler (void) __attribute__ ((weak));
void TIM2_IRQHandler (void) __attribute__ ((weak));
void TIM3_IRQHandler (void) __attribute__ ((weak));
void TIM4_IRQHandler (void) __attribute__ ((weak));
void I2C1_EV_IRQHandler (void) __attribute__ ((weak));
void I2C1_ER_IRQHandler (void) __attribute__ ((weak));
void I2C2_EV_IRQHandler (void) __attribute__ ((weak));
void I2C2_ER_IRQHandler (void) __attribute__ ((weak));
void SPI1_IRQHandler (void) __attribute__ ((weak));
void SPI2_IRQHandler (void) __attribute__ ((weak));
void USART1_IRQHandler (void) __attribute__ ((weak));
void USART2_IRQHandler (void) __attribute__ ((weak));
void USART3_IRQHandler (void) __attribute__ ((weak));
void EXTI15_10_IRQHandler (void) __attribute__ ((weak));
void RTCAlarm_IRQHandler (void) __attribute__ ((weak));
void CEC_IRQHandler (void) __attribute__ ((weak));
void TIM12_IRQHandler (void) __attribute__ ((weak));
void TIM13_IRQHandler (void) __attribute__ ((weak));
void TIM14_IRQHandler (void) __attribute__ ((weak));
void ADC3_IRQHandler (void) __attribute__ ((weak));
void FSMC_IRQHandler (void) __attribute__ ((weak));
void TIM5_IRQHandler (void) __attribute__ ((weak));
void SPI3_IRQHandler (void) __attribute__ ((weak));
void UART4_IRQHandler (void) __attribute__ ((weak));
void UART5_IRQHandler (void) __attribute__ ((weak));
void TIM6_DAC_IRQHandler (void) __attribute__ ((weak));
void TIM7_IRQHandler (void) __attribute__ ((weak));
void DMA2_Channel1_IRQHandler (void) __attribute__ ((weak));
void DMA2_Channel2_IRQHandler (void) __attribute__ ((weak));
void DMA2_Channel3_IRQHandler (void) __attribute__ ((weak));
void DMA2_Channel4_5_IRQHandler (void) __attribute__ ((weak));
void DMA2_Channel5_IRQHandler (void) __attribute__ ((weak));

/* defined in linker script */
extern char __stack[];

void * int_vectors[] __attribute__ ((section(".vectors"))) = {
	(void *) __stack,
	(void *) _start,
	(void *) NMI_Handler,
	(void *) HardFault_Handler,
	(void *) MemManage_Handler,
	(void *) BusFault_Handler,
	(void *) UsageFault_Handler,
	0, 0, 0, 0,
	(void *) SVC_Handler,
	(void *) DebugMon_Handler,
	0,
	(void *) PendSV_Handler,
	(void *) SysTick_Handler,
	(void *) WWDG_IRQHandler,
	(void *) PVD_IRQHandler,
	(void *) TAMPER_STAMP_IRQHandler,
	(void *) RTC_WKUP_IRQHandler,
	(void *) FLASH_IRQHandler,
	(void *) RCC_IRQHandler,
	(void *) EXTI0_IRQHandler,
	(void *) EXTI1_IRQHandler,
	(void *) EXTI2_IRQHandler,
	(void *) EXTI3_IRQHandler,
	(void *) EXTI4_IRQHandler,
	(void *) DMA1_Channel1_IRQHandler,
	(void *) DMA1_Channel2_IRQHandler,
	(void *) DMA1_Channel3_IRQHandler,
	(void *) DMA1_Channel4_IRQHandler,
	(void *) DMA1_Channel5_IRQHandler,
	(void *) DMA1_Channel6_IRQHandler,
	(void *) DMA1_Channel7_IRQHandler,
	(void *) ADC1_IRQHandler,
	0, 0, 0, 0,
	(void *) EXTI9_5_IRQHandler,
	(void *) TIM1_BRK_TIM15_IRQHandler,
	(void *) TIM1_UP_TIM16_IRQHandler,
	(void *) TIM1_TRG_COM_TIM17_IRQHandler,
	(void *) TIM1_CC_IRQHandler,
	(void *) TIM2_IRQHandler,
	(void *) TIM3_IRQHandler,
	(void *) TIM4_IRQHandler,
	(void *) I2C1_EV_IRQHandler,
	(void *) I2C1_ER_IRQHandler,
	(void *) I2C2_EV_IRQHandler,
	(void *) I2C2_ER_IRQHandler,
	(void *) SPI1_IRQHandler,
	(void *) SPI2_IRQHandler,
	(void *) USART1_IRQHandler,
	(void *) USART2_IRQHandler,
	(void *) USART3_IRQHandler,
	(void *) EXTI15_10_IRQHandler,
	(void *) RTCAlarm_IRQHandler,
	(void *) CEC_IRQHandler,
	(void *) TIM12_IRQHandler,
	(void *) TIM13_IRQHandler,
	(void *) TIM14_IRQHandler,
	0,
	(void *) ADC3_IRQHandler,
	(void *) FSMC_IRQHandler,
	0,
	(void *) TIM5_IRQHandler,
	(void *) SPI3_IRQHandler,
	(void *) UART4_IRQHandler,
	(void *) UART5_IRQHandler,
	(void *) TIM6_DAC_IRQHandler,
	(void *) TIM7_IRQHandler,
	(void *) DMA2_Channel1_IRQHandler,
	(void *) DMA2_Channel2_IRQHandler,
	(void *) DMA2_Channel3_IRQHandler,
	(void *) DMA2_Channel4_5_IRQHandler,
	(void *) DMA2_Channel5_IRQHandler,
	0, 0, 0, 0, 0, 0, 0, 0,
	0,
};
