#include "TIM.h"

void TIM_Init(TIM_TypeDef* TIMx, TIM_InitTypeDef* TIMx_Struct){ 
	uint8_t APB_Prescaler_Value = 0;
	
	switch((uint32_t)TIMx){
		case (uint32_t)TIM1:
			RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
			APB_Prescaler_Value = 1 << (((RCC->CFGR & RCC_CFGR_PPRE2) >> 13) - 0x3);
			break;
		case (uint32_t)TIM2:
			RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
			APB_Prescaler_Value = 1 << (((RCC->CFGR & RCC_CFGR_PPRE1) >> 10) - 0x3);
			break;
		case (uint32_t)TIM3:
			RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
			APB_Prescaler_Value = 1 << (((RCC->CFGR & RCC_CFGR_PPRE1) >> 10) - 0x3);
			break;
		case (uint32_t)TIM4:
			RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
			APB_Prescaler_Value = 1 << (((RCC->CFGR & RCC_CFGR_PPRE1) >> 10) - 0x3);
			break;
		case (uint32_t)TIM5:
			RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
			APB_Prescaler_Value = 1 << (((RCC->CFGR & RCC_CFGR_PPRE1) >> 10) - 0x3);
			break;
		case (uint32_t)TIM9:
			RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;
			APB_Prescaler_Value = 1 << (((RCC->CFGR & RCC_CFGR_PPRE2) >> 13) - 0x3);
			break;
		case (uint32_t)TIM10:
			RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;
			APB_Prescaler_Value = 1 << (((RCC->CFGR & RCC_CFGR_PPRE2) >> 13) - 0x3);
			break;
		case (uint32_t)TIM11:
			RCC->APB2ENR |= RCC_APB2ENR_TIM11EN;
			APB_Prescaler_Value = 1 << (((RCC->CFGR & RCC_CFGR_PPRE2) >> 13) - 0x3);
			break;
	}
	
	TIMx->PSC = (SystemCoreClock / 10000) - 1;
	TIMx->ARR = 3500;
	TIMx->DIER |= TIM_DIER_UIE;
	TIMx->CR1 |= TIM_CR1_OPM;
	TIMx->CR1 |= TIM_CR1_URS;
	TIMx->CR1 |= TIM_CR1_CEN;
	
	switch((uint32_t)TIMx){
		case (uint32_t)TIM1:
			break;
		case (uint32_t)TIM2:
			NVIC_EnableIRQ(TIM2_IRQn);
			NVIC_SetPriority(TIM2_IRQn, 4);
			break;
		case (uint32_t)TIM3:
			NVIC_EnableIRQ(TIM3_IRQn);
			NVIC_SetPriority(TIM3_IRQn, 3);
			break;
	}
	
}
