#include "stm32l496xx.h"
#include "FreeRTOS.h"
#include "task.h"
volatile int a=0;
/* System Call Table - MUST contain exactly 70 entries */
const UBaseType_t uxSystemCallImplementations[70] ={0};
void xTask_1(void * pvParm);
int main()
{
	xTaskCreate(xTask_1,"Start",100,(int*)&a,1,NULL);
	vTaskStartScheduler();

	return 0;
}
void xTask_1(void * pvParam)
{
	int value = *(int*)(pvParam);
	while(1)
	{
		value++;
	}


}
