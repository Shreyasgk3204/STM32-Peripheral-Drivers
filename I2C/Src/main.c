#include "stm32l496xx.h"
uint8_t tx_buffer[]={1,2,3,4,5,6,7,8};
uint8_t rx_buffer[10]={0};
void ROM_Bytewrite(uint16_t addr,uint8_t data);
void ROM_Pagewrite(uint16_t addr,uint8_t* bfr_ptr,int size);
void ROM_Byteread(uint16_t addr,uint8_t read_data);
void ROM_Pageread(int size);
int i=0;
int main()
{
	// Enable HSI16
RCC->CR |= RCC_CR_HSION;
while (!(RCC->CR & RCC_CR_HSIRDY)); // Wait until HSI16 is ready

// Set HSI16 as system clock
RCC->CFGR |= RCC_CFGR_SW_HSI;
while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI); // Wait until HSI is selected
// Enable GPIOB clock (assuming SDA=PB7, SCL=PB6)
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

	// Enable I2C1 clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;
	// Set PB8 (SCL) and PB9 (SDA) to Alternate Function Mode
		GPIOB->MODER &= ~(GPIO_MODER_MODE8 | GPIO_MODER_MODE9);  // Clear mode bits
		GPIOB->MODER |= (GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1); // AF mode (10)

		// Set to Open-Drain (required for I2C)
		GPIOB->OTYPER |= (GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9);

		// Enable Pull-up resistors (recommended)
		GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9);
		GPIOB->PUPDR |= (GPIO_PUPDR_PUPD8_0 | GPIO_PUPDR_PUPD9_0); // Pull-up (01)

		// Set high-speed (optional but good for I2C)
		GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED8 | GPIO_OSPEEDR_OSPEED9);

		// Select AF4 (I2C1) for PB8 and PB9 (AFRH register for pins 8-15)
		GPIOB->AFR[1] |= (4 << ((8 - 8) * 4)) | (4 << ((9 - 8) * 4)); // AFR[1] for pins 8-15

		//I2C Init
		uint8_t read;
		I2C1->CR2 |= (0x50<<1);//Slave Address
		I2C1->TIMINGR |= 0x3042C3C7;
		ROM_Pagewrite(0x0000,tx_buffer,8);
		for(volatile int k=0;k<10000;k++);
		ROM_Byteread(0x0004,read);
	/*//I2C1->CR2 |= 0x1<<14;
	I2C1->CR2 |= 0x1<<10;
	I2C1->CR2 |= 1<<13;
	while(!((I2C1->ISR)&(1<<2)));
		rx_buffer[0] = I2C1->RXDR;*/
}
void ROM_Bytewrite(uint16_t addr,uint8_t data)
{
	int cnt=0;
	uint8_t tx_buffer[3]={(0x7f00 & addr),(0x00FF)&addr,data};
	I2C1->CR2 |= (0x1<<25); //Auto End
	I2C1->CR2 |= 0x3<<16;
	I2C1->CR1 |= 1<<0;//Peripheral Enable
	I2C1->CR2 |= 0x1<<13;//Start
	while(cnt<=2){
			while(!((I2C1->ISR)&(1<<0)));
				I2C1->TXDR = tx_buffer[cnt++];
	}

}
void ROM_Pagewrite(uint16_t addr,uint8_t bfr_ptr[],int size)
{
	int cnt=0;
	int sz = size;
	uint8_t tx_buffer[64]={(0x7f00 & addr) , (0x00FF&addr)};
		for(int j=2;j<=sz;j++)
		{
			tx_buffer[j]=bfr_ptr[j-2];
		}
		I2C1->CR2 |= (0x1<<25); //Auto End
		I2C1->CR2 |= (sz+2)<<16;
		I2C1->CR1 |= 1<<0;//Peripheral Enable
		I2C1->CR2 |= 0x1<<13;//Start
		while(cnt<=(sz+2)){
				while(!((I2C1->ISR)&(1<<0)));
					I2C1->TXDR = tx_buffer[cnt++];
		}

}
void ROM_Byteread(uint16_t addr,uint8_t read_data)
{
	int cnt=0;
	uint8_t transmit[2]={(0x7f00 & addr),(0x00FF&addr)};
	I2C1->CR2 &= ~(0x1<<25); //Auto End
	I2C1->CR2 &= ~((0xff)<<16);
	I2C1->CR2 |= (2)<<16;
	I2C1->CR1 |= 1<<0;//Peripheral Enable
	I2C1->CR2 |= 0x1<<13;//Start
	while(cnt<2){
			while(!((I2C1->ISR)&(1<<0)));
				I2C1->TXDR = transmit[cnt++];
	}
	I2C1->CR2 |= 0x1<<10;
	I2C1->CR2 &= ~((0xff)<<16);
	I2C1->CR2 |= 0x1<<16;
	I2C1->CR2 |= 0x1<<13;//Start
	while(!((I2C1->ISR)&(1<<2)));
		read_data = I2C1->RXDR;
	//Stop
	I2C1->CR2 |= (0x1<<14);
}
