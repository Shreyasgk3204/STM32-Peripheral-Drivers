#include "stm32l496xx.h"
uint8_t tx_buffer[6]="Hello";
uint8_t read_cmd[2]={0xC0,0x10};
uint8_t rx_buffer[6]={0};
volatile int i = 0;
int main(void)
{
    // Enable Clocks
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY));
	RCC->CFGR |= (0x1<<0);
	RCC->CFGR |= (0x4<<8);
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;  // Enable GPIOA clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;  // Enable GPIOB clock
	RCC->APB1ENR1 |= (1<<18);
	RCC->AHB1ENR |= (1<<0);
	GPIOB->MODER = 0;
	GPIOB->MODER |= (0x2<<20) | (0x2<<22);
	// Set PA6 to Alternate Function Mode (AF7 = USART3_CTS)
	GPIOA->MODER &= ~(GPIO_MODER_MODE6);      // Clear existing mode
	GPIOA->MODER |= (2 << GPIO_MODER_MODE6_Pos); // Set to AF (0b10)

	// Select AF7 (USART3) for PA6
	GPIOA->AFR[0] &= ~(0xF << (6 * 4));        // Clear AF bits for PA6
	GPIOA->AFR[0] |= (7 << (6 * 4));           // Set AF7 (0b0111)
	// Set PB1 to Alternate Function Mode (AF7 = USART3_RTS)
	GPIOB->MODER &= ~(GPIO_MODER_MODE1);      // Clear existing mode
	GPIOB->MODER |= (2 << GPIO_MODER_MODE1_Pos); // Set to AF (0b10)

	// Select AF7 (USART3) for PB1
	GPIOB->AFR[0] &= ~(0xF << (1 * 4));       // Clear AF bits for PB1
	GPIOB->AFR[0] |= (7 << (1 * 4));          // Set AF7 (0b0111)
	// Enable Pull-Up on PA6 (CTS)
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD6);      // Clear existing setting
	GPIOA->PUPDR |= (1 << GPIO_PUPDR_PUPD6_Pos); // Pull-Up (0b01)

	// Enable Pull-Up on PB1 (RTS)
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD1);      // Clear existing setting
	GPIOB->PUPDR |= (1 << GPIO_PUPDR_PUPD1_Pos); // Pull-Up (0b01)
	//GPIOB->PUPDR |= (0x1<<20);
	GPIOB->AFR[1] |= (0x7<<8) | (0x7<<12);
	//USART DMA Mode
/*	DMA1_Channel2->CNDTR = 6;
	DMA1_Channel2->CPAR = rx_buffer;
	DMA1_Channel2->CMAR = tx_buffer;
	DMA1_Channel2->CCR |= (0x1<<4  | (0x1<<7) | (0x1<<14) | (0x1<<6));
	DMA1_Channel2->CCR |= 0x1<<0;
	//DMA1_CSELR->CSELR = 0x2<<4;*/

	USART3->BRR = 0x341;
	USART3->CR1 |= USART_CR1_TXEIE;
	//USART3->CR3 |= (1<<7);//DMA Request
	//USART3->CR1 |= (1<<10);
	//USART3->CR1 |= (1<<9);
	USART3->CR1 |= (1<<12);
	USART3->CR1 |= (1<<3);
	USART3->CR3 |= (0x7<<8);
	USART3->CR1 |= (1<<0);
	NVIC_EnableIRQ(USART3_IRQn);


}
void USART3_IRQHandler(void)
{

	if(((USART3->ISR)&(USART_ISR_TXE)))// & ((USART3->ISR)&(USART_ISR_CTS)))
	{
		if(i == 5){
            USART3->CR1 &= ~USART_CR1_TXEIE;

		}
		USART3->TDR = tx_buffer[i++];
		USART3->ICR |= USART_ICR_TCCF;
	}
	if(((USART3->ISR)&(USART_ISR_CTSIF)))
	{
		USART3->ICR |= USART_ICR_CTSCF;
	}

}


