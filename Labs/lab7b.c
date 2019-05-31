
/* MUHAMMAD TAREQUZZAMAN 100954008 */

#include <stdint.h>
#include "../inc/msp432p401r.h"
#include "../inc/BSP.h"				// Need to also Add Corresponding  BSP.c to the project
#include "../inc/CortexM.h"   // Need to also Add Corresponding  CortexM.c to the project

volatile uint32_t selectInterupt = 0;
volatile uint32_t adcInterupt = 0;
volatile uint32_t counts = 0;		// Global variables for tracking

volatile uint16_t CLEAN; 
uint16_t x,y;
uint8_t select;

// Interrupt service routine
// Executed every 333ns*(period)
void SysTick_Handler(void){   // Name must be identical to that in startupxxxx.s file. 

	ADC14CTL1 = (ADC14CTL1&~0x001F0000) | (0 << 16); 
	ADC14CTL0 |= 0x00000003;
	//counts++;  // debug step

}

void ADC14_IRQHandler(void){
	/* ADC14 IRQ Handler FOR JOYSTIC */
	
		uint16_t x,y;
		uint8_t select;
		
		
		BSP_Joystick_Input(&x,&y,&select);
		BSP_LCD_DrawString(10, 1, "     ", BSP_LCD_Color565(0,255,0));  // To erase larger number before display of smaller number
		BSP_LCD_SetCursor(10, 1);
		BSP_LCD_OutUDec(x, BSP_LCD_Color565(0,255,0));
		
		BSP_LCD_DrawString(10, 2, "     ", BSP_LCD_Color565(0,255,0));  // To erase larger number before display of smaller number
		BSP_LCD_SetCursor(10, 2);
		BSP_LCD_OutUDec(y, BSP_LCD_Color565(0,255,0));	
		
		BSP_LCD_DrawString(10, 3, "     ", BSP_LCD_Color565(0,255,0));  // To erase larger number before display of smaller number
		BSP_LCD_SetCursor(10, 3);
		BSP_LCD_OutUDec(1^select, BSP_LCD_Color565(0,255,0)); // NOTE: (1 ^ select) ENSURE '1'  WHEN BUTTEN HAVE BEEN PRESSED 
		
	
	
	// Clear memory by reading . clear the appropriate bits in the ADC14IFGR0 register
	uint16_t CLEAN; 
	CLEAN = ADC14MEM0; //(where n is the buffer number you are using
	CLEAN = ADC14MEM1; //(because you must clear all flags in the sequence!)
	//	adcInterupt++; // debug step
	
}
	
void PORT4_IRQHandler(void){
		
		selectInterupt++; // debug step
		
		BSP_LCD_DrawString(10, 3, "     ", BSP_LCD_Color565(0,255,0));  // To erase larger number before display of smaller number
		BSP_LCD_SetCursor(10, 3);
		BSP_LCD_OutUDec(1^select, BSP_LCD_Color565(0,255,0)); // NOTE: (1 ^ select) ENSURE '1'  WHEN BUTTEN HAVE BEEN PRESSED 
		
		P4IFG &= ~ 0x02; // clear the P4.1 IFG flag 
	
}

void SYSTICK_init(void){
	SYSTICK_STCSR = 0;		// Disable SysTick during setup
	SYSTICK_STRVR = 3000-1;	//  300000 If clock is 3 MHz, period is 333ns. Maximum is 2^24-1. Minimum = length of ISR
	SYSTICK_STCVR = 0;	// Any write clear the COUNT bit
	
	SYSPRI3 = (SYSPRI3&0x00FFFFFF)|0x40000000; // priority 2
	SYSTICK_STCSR = BIT0 | BIT1 | BIT2;   		// 0x00000007 to enable interrupts
}
	
void BSP_Joystick_Int_Init(void){
///*
//Locally enable the ADC14 Interrupts (i.e. using ADC14IERx) 
//Enable NVIC interrupts for the ADC14 interrupt (i.e. using NVIC_ISER0/1) 
//Set the NVIC priority level for the ADC14 interrupt (i.e. using NVIC_IPRx) 
//Repeat these three steps for the joystick’s SELECT button. 
//Select need different priority in order to print from  PORT4_IRQHandler
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
	// JoYT x and y 
	ADC14IER0 = 0x00000002; // set enable flag0 : MEM1  
	ADC14IER1 = 0x00000000; // set all to disable 
	NVIC_IPR6 = (NVIC_IPR6&0xFFFFFF00)|0x00000060 ; // Enable priority 3
	NVIC_ISER0 = 0x01000000; // enabel ADC14_IRQHandler 
	
	// JoYT select  P4.1 == select/Z @JOYSTIC 
	P4IES |= 0x02;
	P4IFG &= ~0x02;
	P4IE |=0x02;
	
	NVIC_IPR9 = (NVIC_IPR9 & 0xFF00FFFF )|0x00400000; // Enable priority 2 
	NVIC_ISER1 = 0x00000040; // enabel P4.1 == select JT
	
	__ASM("CPSIE I"); //  wait for Interrupts: Enable 
	
	// LCD Display header X Y Z 
	BSP_LCD_DrawString(0,0,"Joystick Lab7b", BSP_LCD_Color565(255,0,0));
 	BSP_LCD_DrawString(3, 1,"x = ", BSP_LCD_Color565(255,0,0));
	BSP_LCD_DrawString(3, 2,"y = ", BSP_LCD_Color565(255,0,0));
	BSP_LCD_DrawString(3, 3,"S = ", BSP_LCD_Color565(255,0,0));
	
	
}
	

int main(void) {
	
	WDTCTL = WDTPW | WDTHOLD; // Disable Watchdog 
	
	BSP_Clock_InitFastest(); // BSP CLOCK initialization.
	BSP_LCD_Init(); // BSP LCD initialization.
	BSP_Joystick_Init(); // BSP Joystick initialization.
	BSP_Joystick_Int_Init(); // BSP_joystic ADC14 interrupt initialization.
	SYSTICK_init(); // SYSTICK initialization.
	

	//EnableInterrupts(); // NO NEED IF " __ASM("CPSIE I"); " IN BSP_Joystick_Int_Init
 
	
	while(1){
		//__ASM("wfi"); // NO NEED IF " __ASM("CPSIE I"); " IN BSP_Joystick_Int_Init
	}
}