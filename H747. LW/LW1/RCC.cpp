#include "RCC.h"

volatile uint32_t msTicks;

void delay_ms(uint32_t delay){
	uint32_t curTicks;
	
	curTicks = msTicks;
	while((msTicks - curTicks) < delay);
}

uint32_t get_ms(void){
	return msTicks;
}

