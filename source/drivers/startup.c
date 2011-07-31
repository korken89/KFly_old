/******************************************************************************
 * @file :		startup.c
 * @brief :		Startup File
 * @version : 	V1.0
 * @author :	Emil Fresk
 ******************************************************************************/

#include "LPC17xx.h"

extern void Timer0_IRQHandler(void);
extern void UART0_IRQHandler(void);
extern void EINT3_IRQHandler(void);
extern void I2C0_IRQHandler(void);
extern void main(void);
extern void memset(int*, int, int);
extern void memcpy(int*, int*, int);

extern void xPortSysTickHandler(void);
extern void xPortPendSVHandler(void);
extern void vPortSVCHandler( void );

void Reset_Handler(void);
void Default_Handler(void);

#define STACK_SIZE	8192
__attribute__((section(".stackarea"))) char STACK[STACK_SIZE];

/******************************************************************************
* Vector Table
******************************************************************************/

__attribute__((section(".vectors"))) void* vectors[] = {
	&STACK[STACK_SIZE-5],		/* -4 from top. Workaround for GDB bug */
	Reset_Handler,				/* Reset Handler */
	0, //NMI_Handler,			/* NMI Handler */
	0, //HardFault_Handler,		/* Hard Fault Handler */
	0, //MemManage_Handler,		/* MPU Fault Handler */
	0, //BusFault_Handler,		/* Bus Fault Handler */
	0, //UsageFault_Handler,	/* Usage Fault Handler */
	0,						 	/* Reserved */
	0,						 	/* Reserved */
	0,						 	/* Reserved */
	0,						 	/* Reserved */
	vPortSVCHandler,			/* SVCall Handler */
	0, //DebugMon_Handler,		/* Debug Monitor Handler */
	0,						 	/* Reserved */
	xPortPendSVHandler,			/* PendSV Handler */
	xPortSysTickHandler,		/* SysTick Handler */

	// External Interrupts
	0, //WDT_IRQHandler,		/* Watchdog Timer */
	Timer0_IRQHandler,			/* Timer0 */
	0, //TIMER1_IRQHandler,		/* Timer1 */
	0, //TIMER2_IRQHandler,		/* Timer2 */
	0, //TIMER3_IRQHandler,		/* Timer3 */
	UART0_IRQHandler,			/* UART0 */
	0, //UART1_IRQHandler,		/* UART1 */
	0, //UART2_IRQHandler,		/* UART2 */
	0, //UART3_IRQHandler,		/* UART3 */
	0, //PWM1_IRQHandler,		/* PWM1 */
	I2C0_IRQHandler,			/* I2C0 */
	0, //I2C1_IRQHandler,		/* I2C1 */
	0, //I2C2_IRQHandler,		/* I2C2 */
	0, //SPI_IRQHandler,		/* SPI */
	0, //SSP0_IRQHandler,		/* SSP0 */
	0, //SSP1_IRQHandler,		/* SSP1 */
	0, //PLL0_IRQHandler,		/* PLL0 (Main PLL) */
	0, //RTC_IRQHandler,		/* Real Time Clock */
	0, //EINT0_IRQHandler,		/* External Interrupt 0 */
	0, //EINT1_IRQHandler,		/* External Interrupt 1 */
	0, //EINT2_IRQHandler,		/* External Interrupt 2 */
	EINT3_IRQHandler,			/* External Interrupt 3 and GPIO Interrupts */
	0, //ADC_IRQHandler,		/* A/D Converter */
	0, //BOD_IRQHandler,		/* Brown Out Detect */
	0, //USB_IRQHandler,		/* USB */
	0, //CAN_IRQHandler,		/* CAN */
	0, //DMA_IRQHandler,		/* GP DMA */
	0, //I2S_IRQHandler,		/* I2S */
	0, //ENET_IRQHandler,		/* Ethernet */
	0, //RIT_IRQHandler,		/* Repetitive Interrupt Timer */
	0, //MCPWM_IRQHandler,		/* Motor Control PWM */
	0, //QEI_IRQHandler,		/* Quadrature Encoder Interface */
	0, //PLL1_IRQHandler,		/* PLL1 (USB PLL) */
};

/*******************************************************************************
* Function Name : Reset_Handler
* Description	: Gets run after Reset. Fixes SRAM and sets BSS segment = 0 and calls main program.
*******************************************************************************/
void Reset_Handler(void)
{
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");

	extern int _BSS_START;
	extern int _BSS_END;
	extern int _DATA_START;
	extern int _DATA_END;
	extern int _DATA_FLASH_START;

	//Copy data from Flash to SRAM
	memcpy(&_DATA_START, &_DATA_FLASH_START, &_DATA_END - &_DATA_START);

	// Clear the BSS segment
	memset(&_BSS_START, 0, &_BSS_END - &_BSS_START);
	
	// Go to main program
	main();
}

void Default_Handler(void)
{
	while (1);
}
