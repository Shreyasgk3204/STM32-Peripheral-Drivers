#include "stm32l496xx.h"

uint16_t tx_buffer[4] = {1, 2, 3, 4};
uint16_t rx_buffer[8] = {0};
int i = 0,k=0;
volatile int count=0;
void SPI1_IRQHandler(void);

int main(void) {
    // Enable HSI Clock (16 MHz internal oscillator)
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY));
    RCC->CFGR |= 0x1<<0;
    // Enable GPIOA (NSS) and GPIOB (SCK, MISO, MOSI)
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN;

    // Enable SPI1 Clock (APB2)
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    RCC->AHB1ENR |= (0x1<<0);

    // Configure NSS (PA4) as Software-Managed Output
    GPIOA->MODER &= ~(GPIO_MODER_MODE4);
    GPIOA->MODER |= (0x2<<8);  // Output mode
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT4);   // Push-pull
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD4);   // No pull-up/pull-down
    GPIOA->AFR[0] |= (0x5<<16);
   // GPIOA->BSRR = (1 << 4);                // Set NSS high (inactive)

    // Configure SCK (PB3), MISO (PB4), MOSI (PB5) as AF5 (SPI1)
    GPIOB->MODER = 0;
    GPIOB->MODER |=  ((2U << (2*3)) | (2U << (2*4)) | (2U << (2*5))); // AF mode
    GPIOB->MODER |= 0x1<<28 | 0x1<<14;
    GPIOB->MODER |= 0x1<<20 | 0x1<<22;
    GPIOB->AFR[0] &= ~((0xF << (4*3)) | (0xF << (4*4)) | (0xF << (4*5)));
    GPIOB->AFR[0] |=  ((5U << (4*3)) | (5U << (4*4)) | (5U << (4*5))); // AF5 (SPI1)

    // SPI1 Configuration (Master, Mode 3, 8-bit data, Software NSS)
    SPI1->CR1 = 0;
    SPI1->CR1 |= SPI_CR1_MSTR;            // Master mode
    SPI1->CR1 |= (0x3 << 0);              // CPOL=1, CPHA=1 (SPI Mode 3)
    SPI1->CR1 |= (0x0 << 3);              // Baud rate = fPCLK/16
    //SPI1->CR1 |= SPI_CR1_SSM;             // Software NSS management
   // SPI1->CR1 |= SPI_CR1_SSI;             // Internal NSS high (ignored in master)
    SPI1->CR2 |= (0x1<<2);
    SPI1->CR2 |= 0xf<<8; // 8-bit data
    //SPI1->CR2 |= (0x3<<6);//Enable Interrupts
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);
    NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    NVIC_SetPriority(DMA1_Channel3_IRQn,6);
    NVIC_SetPriority(DMA1_Channel2_IRQn,4);
   // GPIOA->BSRR = (1 << (4 + 16));    // PA4 = LOW (active)
    //DMA Channel 3(SPI1_TX) Configurations
    DMA1_Channel3->CCR |= ((0x9<<4) | (0x5<<8)) | (0x1<<1);

    DMA1_Channel3->CNDTR = 4;
    DMA1_Channel3->CMAR = tx_buffer;
    DMA1_Channel3->CPAR = (uint32_t)&(SPI1->DR);
    //DMA Channel 4(SPI1_RX) Configurations
    DMA1_Channel2->CCR |= (0x8<<4) | 0x5<<8 | (0x1<<1);
    DMA1_Channel2->CCR |= (0x3<<12);
    DMA1_Channel2->CNDTR = 0x4;
    DMA1_Channel2->CMAR = rx_buffer;
    DMA1_Channel2->CPAR = (uint32_t)&(SPI1->DR);
    DMA1_CSELR->CSELR =(0x1<<8)|(0x1<<4);
    //SPI Dma Enable
    DMA1_Channel3->CCR |= (0x1<<0);
    DMA1_Channel2->CCR |= (0x1<<0);
    SPI1->CR1 |= SPI_CR1_SPE;
    SPI1->CR2 |= 0x1<<0;
    for(volatile int i=0;i<100;i++);
    //SPI1->CR2 |= 0x1<<1;
            // Enable SPI


    /*uint8_t k = 0;
    while (k < 4) {
        // Pull NSS low (start transmission)
        GPIOA->BSRR = (1 << (4 + 16));    // PA4 = LOW (active)

        // Send dummy data (0xFF) to read from slave
       while (!(SPI1->SR & SPI_SR_TXE)); // Wait for TX buffer empty
        SPI1->DR = tx_buffer[i++];                  // Send dummy byte

        // Wait for RX data
        while (!(SPI1->SR & SPI_SR_RXNE)); // Wait for RX buffer not empty
        rx_buffer[k++] = SPI1->DR;         // Read received data

        // Pull NSS high (end transmission)
        GPIOA->BSRR = (1 << 4) ;          // PA4 = HIGH (inactive)
    }*/

}
void DMA1_Channel2_IRQHandler(void)
{
	if(DMA1->ISR & (0x1<<5))
	{
		DMA1->IFCR |= 0x1<<5;
		GPIOB->BSRR = 0x1<<7;
		GPIOB->BSRR = 0x1<<26|0x1<<11;

	}
}
void DMA1_Channel3_IRQHandler(void)
{
	if(DMA1->ISR & (0x1<<9))
	{
		DMA1->IFCR |= 0x1<<9;
		GPIOB->BSRR = 0x1<<14;
		GPIOB->BSRR = 0x1<<10|0x1<<27;

	}
}
/*void SPI1_IRQHandler(void)
{
	count++;
    if ((SPI1->SR & SPI_SR_TXE))
    {
     SPI1->DR = tx_buffer[i++];
    }
     if((SPI1->SR & SPI_SR_RXNE))
     {
          rx_buffer[k++] = SPI1->DR;
     }
     if((count == 4))
     {
    	 NVIC_DisableIRQ(SPI1_IRQn);
    	// GPIOA->BSRR = (1 << 4) ;
     }
}*/
