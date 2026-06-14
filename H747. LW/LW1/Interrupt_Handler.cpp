//====================================================================================================
#include "main.h"
#include "Interrupt_Handler.h"
//====================================================================================================
extern volatile uint32_t msTicks;

#ifdef __cplusplus
extern "C" {

	void SysTick_Handler(void){
		msTicks++;
	}

	void HardFault_Handler(void) {
		NVIC_SystemReset();
	}
	
	void EXTI15_10_IRQHandler(void){
		
	}
}
#endif
