#include "stm32l496xx.h"
void SysTick_Handler(void);
typedef uint32_t(*Stack_Ptr)[16];
volatile int a=0,b=0,c=0;
volatile int cnt=0;
void context_switch(void);
void task_1(void);
void task_2(void);
void task_3(void);
uint32_t stack_1[]={0x18,0x19,0x1a,0x1b,0x14,0x15,0x16,0x17,0x10,0x11,0x12,0x13,0x1c,0x309,(uint32_t)task_1,0x01000000};
uint32_t stack_2[]={0x28,0x29,0x2a,0x2b,0x14,0x15,0x16,0x17,0x10,0x11,0x12,0x13,0x1c,0x309,(uint32_t)task_2,0x01000000};
uint32_t stack_3[]={0x38,0x39,0x3a,0x3b,0x14,0x15,0x16,0x17,0x10,0x11,0x12,0x13,0x1c,0x309,(uint32_t)task_3,0x01000000};
Stack_Ptr stack[3]={&stack_1,&stack_2,&stack_3};
int main()
{


	RCC->CR |= RCC_CR_HSION;
	while (!(RCC->CR & RCC_CR_HSIRDY)); // Wait until HSI16 is ready

	// Set HSI16 as system clock
	RCC->CFGR |= RCC_CFGR_SW_HSI;
	   // Configure SysTick for 1ms interrupts (assuming 16MHz clock)
	    SysTick->LOAD = (16000000 / 1000) - 1; // 1ms interval
	    SysTick->VAL = 0;
	    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
	                    SysTick_CTRL_TICKINT_Msk |
	                    SysTick_CTRL_ENABLE_Msk;
	while(1){

	}
}
void SysTick_Handler(void){

    // Reset cnt before using it
	WWDG->CR = 0x7F;
	asm volatile (
	        "PUSH {R4-R7} \n"  // Push R4-R7
	        "MOV R0,R8 \n"  // NVIC ISER0 register address
	        "MOV R1,R9 \n"  // Enable IRQ
			"MOV R2,R10 \n"
			"MOV R3,R11 \n"
			"PUSH {R0-R3} \n"
	    );
	// Change the Stack Pointer
	__set_MSP((uint32_t)stack[(cnt)%3]);
	cnt++;


	//POP Operation
	asm volatile (
	        "POP {R0-R3} \n"  // Push R4-R7
	        "MOV R8,R0 \n"  // NVIC ISER0 register address
	        "MOV R9,R1 \n"  // Enable IRQ
			"MOV R10,R2 \n"
			"MOV R11,R3 \n"
			"POP {R4-R7} \n"
			"BX LR \n"
	    );
}
void task_1(void){
while(1){
	a++;
}
}
void task_2(void){
	while(1){
		b++;
	}
}
void task_3(void){
	while(1){
		c++;
	}
}
