/* MUHAMMAD TAREQUZZAMAN 100954008 */

#include <stdint.h>
#include "../inc/msp432p401r.h"
#include "../inc/BSP.h"				// Need to also Add Corresponding  BSP.c to the project
#include "../inc/CortexM.h"   // Need to also Add Corresponding  CortexM.c to the project

volatile uint32_t selectInterupt = 0;
volatile uint32_t adcInterupt = 0;
volatile uint32_t counts = 0;		// Global variables for tracking

// Interrupt service routine
// Executed every 333ns*(period)
void SysTick_Handler(void){   // Name must be identical to that in startupxxxx.s file. 
///*  	ADC14CTL0 ???
//
//		P.S : BIT3 value is reserved as ZERO . 
//		BIT0: ADC14SC = 1 ; ADC14 start conversion. Software-controlled sample-and-conversion start.
//												ADC14SC and ADC14ENC may be set together with one instruction. ADC14SC
//												is reset automatically.
//	
//		BIT1: ADC14ENC = 1 ; : ADC14 enable conversion
//*/
	
///*		ADC14CTL1 ???
//		ADC14CSTARTADDx --> BIT16 - BIT20  == 111111 b == 0x1F 
//	
//		NOTE: ADC14 ADC14CSTARTADDx conversion start address. These bits select which ADC14 conversion
//		memory register is used for a single conversion or for the first conversion in a
//		sequence. The value of CSTARTADDx is 0h to 1Fh, corresponding to
//		ADC14MEM0 to ADC14MEM31
//*/
	
	ADC14CTL1 = (ADC14CTL1&~0x001F0000) | (2 << 16); // clear STARTADDx bit field and configure for STARTADDx = 2
	ADC14CTL0 |= 0x00000003; //enable conversions. 
	
	
	//counts++;  // debug step

}

void ADC14_IRQHandler(void){
	/* ADC14 IRQ Handler called upon after MEM4 */
	
		uint16_t x,y,z;
	
		BSP_Accelerometer_Input(&x,&y,&z); // read BSP_Accelerometer_Input
	
	/*PRINT BSP_Accelerometer_Input value */
		BSP_LCD_DrawString(10, 1, "     ", BSP_LCD_Color565(0,255,0));  // To erase larger number before display of smaller number
		BSP_LCD_SetCursor(10, 1);
		BSP_LCD_OutUDec(x, BSP_LCD_Color565(0,255,0));
		
		BSP_LCD_DrawString(10, 2, "     ", BSP_LCD_Color565(0,255,0));  // To erase larger number before display of smaller number
		BSP_LCD_SetCursor(10, 2);
		BSP_LCD_OutUDec(y, BSP_LCD_Color565(0,255,0));	
		
		BSP_LCD_DrawString(10, 3, "     ", BSP_LCD_Color565(0,255,0));  // To erase larger number before display of smaller number
		BSP_LCD_SetCursor(10, 3);
		BSP_LCD_OutUDec(z, BSP_LCD_Color565(0,255,0));
	
	
	// Clear memory by reading . clear the appropriate bits in the ADC14IFGR0 register
	uint16_t CLEAN; 
	CLEAN = ADC14MEM2; // Accelerometer used MEM2 --> MEM4
	CLEAN = ADC14MEM3; // therefore, 
	CLEAN = ADC14MEM4;// clear all flags in the sequence MEM2 --> MEM4 (FIFO)
	//	adcInterupt++; // debug step
	
}
	


void SYSTICK_init(void){
	SYSTICK_STCSR = 0;		// Disable SysTick during setup
	SYSTICK_STRVR = 3000-1;	// If clock is 3 MHz, period is 333ns. Maximum is 2^24-1. Minimum = length of ISR
	SYSTICK_STCVR = 0;	// Any write clear the COUNT bit
	
	SYSPRI3 = (SYSPRI3&0x00FFFFFF)|0x40000000; // priority 2
	SYSTICK_STCSR = BIT0 | BIT1 | BIT2;   		// 0x00000007 to enable interrupts
}
	
void BSP_ADC14_USER_Init(void){
///*
//Locally enable the ADC14 Interrupts (i.e. using ADC14IER0 = 0x00000010 ; ADC14IER1 = 0x00000000;) 
//Enable NVIC interrupts for the ADC14 interrupt (i.e. using NVIC_ISER0) 
//Set the NVIC priority level for the ADC14 interrupt (i.e. using NVIC_IPR6) 
//*/
/*	
MEM0 – Joystick x (A9) -- P6.0
MEM1 – Joystick y (A16) -- P4.4
Joystick Select J1.5 -- P4.1
MEM2 – Accelero x (A14) -- P6.1
MEM3 – Accelero y (A13) -- P4.0
MEM4 – Accelero z (A11) -- P4.2
MEM5 – Microphone (A10) -- P4.3	
	
*/	
	
	// Accelerometer x,y,z 
	ADC14IER0 = 0x00000010; // set enable flag0 : MEM2  , seq MEM0 --> MEM4 : for JT to Accelerometer
	ADC14IER1 = 0x00000000; // set all to disable 
	NVIC_IPR6 = (NVIC_IPR6&0xFFFFFF00)|0x00000040 ; // ADC14:IRQ 24: Enable: priority 2
	NVIC_ISER0 = 0x01000000; // enabel ADC14_IRQHandler 
	

	
	__ASM("CPSIE I"); //  wait for Interrupts: Enable 
	
	// LCD Display header X Y Z 
	BSP_LCD_DrawString(0,0,"Accelerometer Lab7C", BSP_LCD_Color565(255,0,0));
 	BSP_LCD_DrawString(3, 1,"x = ", BSP_LCD_Color565(255,0,0));
	BSP_LCD_DrawString(3, 2,"y = ", BSP_LCD_Color565(255,0,0));
	BSP_LCD_DrawString(3, 3,"Z = ", BSP_LCD_Color565(255,0,0));
	
	
}

int main(void) {
	
	WDTCTL = WDTPW | WDTHOLD; // Disable Watchdog 
	
	BSP_Clock_InitFastest(); // BSP CLOCK initialization.
	BSP_LCD_Init(); // BSP LCD initialization.
	BSP_Accelerometer_Init();// BSP Accelerometer initialization. 
	BSP_ADC14_USER_Init(); //  ADC14 interrupt initialization : USER DEFINE .
	SYSTICK_init(); // SYSTICK initialization.
	

//EnableInterrupts(); // NO NEED IF " __ASM("CPSIE I"); " IN BSP_ADC14_USER_Init();
 
	
	while(1){
		//__ASM("wfi"); // NO NEED IF " __ASM("CPSIE I"); " IN BSP_ADC14_USER_Init();
	}
}