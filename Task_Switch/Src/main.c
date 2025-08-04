#include "stm32l496xx.h"
#include "FreeRTOS.h"
#include "task.h"
volatile int a=0,b=0;
/* System Call Table - MUST contain exactly 70 entries */
const UBaseType_t uxSystemCallImplementations[70] ={0};
void xTask_1(void * pvParm);
void xTask_2(void * pvParm);
int main()
{

	xTaskCreate(xTask_1,"Start",100,(int*)&a,1,NULL);
	xTaskCreate(xTask_2,"End",100,(int*)&b,2,NULL);
	vTaskStartScheduler();

	return 0;
}
void xTask_1(void * aParam)
{
	volatile int* value =  ((int*)(aParam)) ;
	while(1)
	{
		(*value)++;
		vTaskDelay(pdMS_TO_TICKS(100));  // Yield CPU every 100ms

	}


}
void xTask_2(void * bParam)
{
	volatile int* value =  ((int*)(bParam));
	while(1)
	{
		(*value)++;
		vTaskDelay(pdMS_TO_TICKS(100));  // Yield CPU every 100ms

	}


}

