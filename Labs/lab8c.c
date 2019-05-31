/* Muhammad Tarequzzaman || 100954008 */

#include <stdint.h>
#include "../inc/msp432p401r.h"
#include "../inc/BSP.h"				// Need to also Add Corresponding  BSP.c to the project
#include "../inc/CortexM.h"   // Need to also Add Corresponding  CortexM.c to the project

uint32_t lux,l; // pointer value for LightSensor
int32_t Volt, localTemp, tempC,oneV; // pointer values for TempSensor
/*---------------Dont use this value for something else----------------------*/
volatile uint8_t deltaT =0; // Global variable to count for TA2 at 2Hz 


void Port1_init(void){
	P1SEL1 &= ~(BIT0);
	P1SEL0 &= ~(BIT0);
	P1DIR |= BIT0;	// 1 for Output
}

/* TA0 N IRQ Handler*/
void TA0_N_IRQHandler(void) {
	//Toggle the Red LED at 1 Hz, using Timer_A0

	TA0CTL &= ~0x0001;    	// Acknowledge Interrupt 0
	P1OUT ^= BIT0; // red led o/off
	deltaT++; // Global variable to count for TA2 at 2Hz 
	
}

/* TA1 N IRQ Handler*/
void TA1_N_IRQHandler(void){
	//Display the updated light level at 2 Hz, using Timer_A1
	TA1CTL &= ~0x0001; // Flag Clear
	
	/* Light sensor  output*/													
	BSP_LightSensor_End(&lux); // light sensor 
	l = lux/100;
	BSP_LCD_DrawString(8, 6, "              ", BSP_LCD_Color565(0,255,0));  // To erase larger number before display of smaller number
	BSP_LCD_SetCursor(8, 6);
	BSP_LCD_OutUDec(l/100, BSP_LCD_Color565(0,255,0));
}

/* TA2 N IRQ Handler*/
void TA2_N_IRQHandler(void){ 
	//Display the updated temperature at 0.5 Hz, using Timer_A2
	

	if(deltaT == 2  ){/* idea is to count 2 sec then interrupt once so 1/2sec = 0.5 Hz  */
		
		TA2CTL &= ~0x0001;// Flag Clear
		
		BSP_TempSensor_End(&Volt, &localTemp); // TempSensor 
		tempC= localTemp/100000; //local temperature (units 100,000*C)

		/* temperature output*/
		
		BSP_LCD_DrawString(8, 1, "              ", BSP_LCD_Color565(0,255,0));  // To erase larger number before display of smaller number
		BSP_LCD_SetCursor(8, 1);
		BSP_LCD_OutUDec(tempC, BSP_LCD_Color565(0,255,0));
		deltaT = 0;	//reset delta time 
}
}
void Timer_A0_Init(void){
/* 8*6 = 48 ; 22mS *48 =1056 ms = 1.056 sec == aprox. 1Hz */
	
	TA0CTL &= ~0x0030; 	// User function - Clear Bit 4 and 5 - Stop mode, save power
	TA0CTL = 0x02C2;		// COMPARE interrupts, no I/O 
	// Bit 9:8 = 10 - SMCLK
	// Bit 7:6 = 11 - Divide by 8 // note: this value change the interval of caling IRQ, make small divide to create more frequncy 
	// Bit 5:4 = 00 - Going to be Continuous Mode (timer counts up to 0xFFFF) but for now STOP mode
	// Bit 2 TACLR = 1 - Timer_A clear. SETTING this bit reset TAxR
	// Bit 1 TAIE = 1 - Enable interrupt
	// Bit 0 TAIFG = 0 - No interrupt pending.
	
	TA0EX0 = 0x0005;	// TAIDEX = 100 for Divide by 6 Input Divider expansion
	NVIC_IPR2 = (NVIC_IPR2&0xFFFF00FF)|0x00004000; // priority 2 in bits 13-15
	NVIC_ISER0 |= BIT9;				// enable interrupt 9 in NVIC
	TA0CTL |= 0x0024;        // reset and start Timer A0 in CONTINUOUS (MC = 10)
	
	__ASM("CPSIE I");
} 


void Timer_A1_Init(void){
/* 22mS x 4x 6 = 528mS = 0.528 sec == aprox. 2 Hz*/
	
	TA1CTL &= ~0x0030; 	// User function - Clear Bit 4 and 5 - Stop mode, save power
	TA1CTL = 0x0282;		// COMPARE interrupts, no I/O 
	// Bit 9:8 = 10 - SMCLK
	// Bit 7:6 = 10 - Divide by 4 // note: this value change the interval of caling IRQ, make small divide to create more frequncy 
	// Bit 5:4 = 00 - Going to be Continuous Mode (timer counts up to 0xFFFF) but for now STOP mode
	// Bit 2 TACLR = 1 - Timer_A clear. SETTING this bit reset TAxR
	// Bit 1 TAIE = 1 - Enable interrupt
	// Bit 0 TAIFG = 0 - No interrupt pending.	
	
	TA1EX0 = 0x0002;	// TAIDEX = 010 for Divide by 3 Input Divider expansion
  NVIC_IPR2 = (NVIC_IPR2&0x00FFFFFF)|0x40000000; // priority 2 in bits 31-29
  NVIC_ISER0 |= BITB;				// enable interrupt 11 in NVIC
	TA1CTL |= 0x0024;        // reset and start Timer A0 in CONTINUOUS (MC = 10)
	
	__ASM("CPSIE I");
}

void Timer_A2_Init(void){
	//Display the updated temperature at 0.5 Hz, using Timer_A2
	/* 22mS x 8x 5 = 880mS = 0.88 sec == aprox. 1 Hz */
	/* idea is to use Software divide for count 2 sec then interrupt once so 1/2sec = 0.5 Hz  */
	
	TA2CTL &= ~0x0030; 	// User function - Clear Bit 4 and 5 - Stop mode, save power
	TA2CTL = 0x02C2;		// COMPARE interrupts, no I/O 
	// Bit 9:8 = 10 - SMCLK
	// Bit 7:6 = 11 - Divide by 8 // note: this value change the interval of caling IRQ, make small divide to create more frequncy 
	// Bit 5:4 = 00 - Going to be Continuous Mode (timer counts up to 0xFFFF) but for now STOP mode
	// Bit 2 TACLR = 1 - Timer_A clear. SETTING this bit reset TAxR
	// Bit 1 TAIE = 1 - Enable interrupt
	// Bit 0 TAIFG = 0 - No interrupt pending.	
	
	TA2EX0 = 0x0004;	// TAIDEX = 100 for Divide by 5 Input Divider expansion
  NVIC_IPR3 = (NVIC_IPR3&0xFFFF00FF)|0x00004000; // priority 2 in bits 15-13
  NVIC_ISER0 |= BITD;				// enable interrupt 13 in NVIC
	TA2CTL |= 0x0024;        // reset and start Timer A0 in CONTINUOUS (MC = 10)
	
	__ASM("CPSIE I");
}
int main(void) {
	
	WDTCTL = WDTPW | WDTHOLD; // watch dog timer 
	/*Initialization setup */
	BSP_LCD_Init();
	BSP_TempSensor_Init();
	BSP_LightSensor_Init();
	Port1_init();
	
	/* calling Timer AX , 0,1,2 Init */
	Timer_A0_Init();	
	Timer_A1_Init();
	Timer_A2_Init();
	
	/*LCD display setup*/
	BSP_LCD_DrawString(0,0,"Temp Sensor", BSP_LCD_Color565(255,0,0));
 
	BSP_LCD_DrawString(1, 1,"Temp C:", BSP_LCD_Color565(255,0,0));
	BSP_LCD_DrawString(0, 4,"Light Sensor", BSP_LCD_Color565(255,0,0));
	BSP_LCD_DrawString(1, 5,"Luminosity", BSP_LCD_Color565(255,0,0));
	BSP_LCD_DrawString(4, 6,"lx:", BSP_LCD_Color565(255,0,0));
	
	/*Empty loop */
	while(1){
		__ASM("wfi");
	}
}