/*
 * FreeRTOSConfig.h
 *
 *  Created on: Jul 25, 2025
 *      Author: Shreyas
 */

#ifndef FREERTOSCONFIG_H_
#define FREERTOSCONFIG_H_

#define configTICK_TYPE_WIDTH_IN_BITS TICK_TYPE_WIDTH_32_BITS
#define configSYSTEM_CALL_STACK_SIZE				512
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 		1
#define configMINIMAL_STACK_SIZE					0x100
#define configMAX_PRIORITIES						5
#define configUSE_PREEMPTION						1
#define configUSE_IDLE_HOOK							0
#define configUSE_TICK_HOOK							0
#define configTOTAL_HEAP_SIZE 						0x1000
#define configCPU_CLOCK_HZ							8000000
#define configTICK_RATE_HZ							10
#define configENFORCE_SYSTEM_CALLS_FROM_KERNEL_ONLY    1  // Security: Only kernel can make system calls
#define configALLOW_UNPRIVILEGED_CRITICAL_SECTIONS     0  // Disallow unprivileged critical sections




#endif /* FREERTOSCONFIG_H_ */
