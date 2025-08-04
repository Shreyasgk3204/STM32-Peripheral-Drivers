#include "stm32l496xx.h"
#include "FreeRTOS.h"
#include "task.h"
/* System Call Table - MUST contain exactly 70 entries */
const UBaseType_t uxSystemCallImplementations[70] ={0};
int main()
{
	vTaskStartScheduler();
	return 0;
}
