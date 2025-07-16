//-------------------------------------------------------------------------------------------//
#include "TIM.h"
//-------------------------------------------------------------------------------------------//
void TIM_Init(void){
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	TIM1->CR1 |= TIM_CR1_OPM;
	TIM1->CR1 |= TIM_CR1_URS;
	TIM1->CR1 |= TIM_CR1_CEN;
}
//-------------------------------------------------------------------------------------------//
void Delay_us(volatile uint16_t us){ // us <= 65534
	TIM1->EGR |= TIM_EGR_UG;
	TIM1->PSC = 0x000F;
	
	TIM1->ARR = us;
	TIM1->CR1 |= TIM_CR1_CEN;
	while(!(TIM1->SR & (TIM_SR_UIF)));
	TIM1->SR = 0;
}
void MegaDelay_us(volatile uint16_t us){ // us <= 65534
	for(volatile int i = 0; i < 10; i++){
		Delay_us(us);
	}
}
//-------------------------------------------------------------------------------------------//
