/* Muhammad Tarequzzaman || 100954008 */

#include <stdint.h>
//#include <msp.h>
#include "../inc/msp432p401r.h"
#include "../inc/BSP.h"				// Need to also Add Corresponding  BSP.c to the project
#include "../inc/CortexM.h"   // Need to also Add Corresponding  CortexM.c to the project

uint32_t lux, l; // pointer value for LightSensor
int32_t Volt, localTemp, tempC,oneV; // pointer values for TempSensor
/*---------------Dont use this value for something else----------------------*/


void Port1_init(void){
	P1SEL1 &= ~(BIT0);
	P1SEL0 &= ~(BIT0);
	P1DIR |= BIT0;	// 1 for Output
}


void TimerA0_Init(void){
	// Using the initialization from Valvano Volumne I, page 357, Program 9.8
	TA0CTL &= ~0x0030; 	// User function - Clear Bit 4 and 5 - Stop mode, save power
	TA0CTL = 0x0282;		// COMPARE interrupts, no I/O 
	// Bit 9:8 = 10 - SMCLK
	// Bit 7:6 = 10 - Divide by 4 // note: this value change the interval of caling IRQ, make small divide to create more frequncy 
	// Bit 5:4 = 00 - Going to be Continuous Mode (timer counts up to 0xFFFF) but for now STOP mode
	// Bit 2 TACLR = 1 - Timer_A clear. SETTING this bit reset TAxR
	// Bit 1 TAIE = 1 - Enable interrupt
	// Bit 0 TAIFG = 0 - No interrupt pending.
		
	TA0EX0 = 0x0003;	// TAIDEX = 011 for Divide by 4 Input Divider expansion
  NVIC_IPR2 = (NVIC_IPR2&0xFFFF00FF)|0x00004000; // priority 2 in bits 13-15
  NVIC_ISER0 |= BIT9;				// enable interrupt 9 in NVIC
	TA0CTL |= 0x0024;        // reset and start Timer A0 in CONTINUOUS (MC = 10)
	
	__ASM("CPSIE I");
} 
/* TA0 N IRQ Handler*/
void TA0_N_IRQHandler(void) {
//Toggle the Red LED at 1 Hz, using Timer_A0
	
	TA0CTL &= ~0x0001;    	// Acknowledge Interrupt 0
	P1OUT ^= BIT0; // red led

	BSP_LightSensor_End(&lux); // light sensor 

	BSP_TempSensor_End(&Volt, &localTemp); // TempSensor 
	
	tempC= localTemp/100000; //local temperature (units 100,000*C)
	l = lux/100; 
	
	/* Light sensor  output*/													
	BSP_LCD_DrawString(8, 6, "              ", BSP_LCD_Color565(0,255,0));  // To erase larger number before display of smaller number
	BSP_LCD_SetCursor(8, 6);
	BSP_LCD_OutUDec(l, BSP_LCD_Color565(0,255,0));

	/* temperature output*/
	BSP_LCD_DrawString(8, 1, "              ", BSP_LCD_Color565(0,255,0));  // To erase larger number before display of smaller number
	BSP_LCD_SetCursor(8, 1);
	BSP_LCD_OutUDec(tempC, BSP_LCD_Color565(0,255,0));
		
}

int main(void) {
	
	WDTCTL = WDTPW | WDTHOLD; // watch dog timer 
	/*Initialization setup */
	BSP_LCD_Init();
	BSP_TempSensor_Init();
	BSP_LightSensor_Init();
	Port1_init();
	/* Calling Timer A0 Init */
	TimerA0_Init();	

	/*LCD display setup*/
  BSP_LCD_DrawString(0,0,"Temp Sensor", BSP_LCD_Color565(255,0,0));
 	//BSP_LCD_DrawString(1, 1,"Status: Halt", BSP_LCD_Color565(255,0,0));
	BSP_LCD_DrawString(1, 1,"Temp C:", BSP_LCD_Color565(255,0,0));
	BSP_LCD_DrawString(0, 4,"Light Sensor", BSP_LCD_Color565(255,0,0));
	BSP_LCD_DrawString(1, 5,"Luminosity", BSP_LCD_Color565(255,0,0));
	BSP_LCD_DrawString(4, 6,"lx:", BSP_LCD_Color565(255,0,0));
		
	/*Empty loop */
	while(1){
		__ASM("wfi");
	}
}