
//#include "../inc/CortexM.h"// And add CortexM.c to your project
#include <stdint.h>
#include "../inc/msp432p401r.h"

/* Initialize RED LED*/
void RED_LED_Int(void){
	P1SEL0 &= ~BIT0;
  P1SEL1 &= ~BIT0;    // 1) configure P1.4 and P1.1 as GPIO
  P1DIR |= BIT0; 
}
/* RED LED ON */	
void RED_LED_ON(void){
	 
	P1OUT |= BIT0 ; 
}
/* RED LED OFF */
void RED_LED_OFF(void){
		P1OUT &= ~BIT0;
}

void SysTick_Init(void){
	SYSTICK_STCSR = 0;						// Disable SysTick during setup
	SYSTICK_STRVR = 300000;			  // Same value used by SysTick_Wait10ms  
																//If clock is 3 MHz, period is 333ns. Maximum is 2^24-1. Minimum = length of ISR
	SYSTICK_STCVR = 0;						// Any write clears the COUNT bit
	SYSTICK_STCSR = BIT0 | BIT2;  // 3
}
void SysTick_Wait10ms() {
	
	/* SRA-style Registers
	SysTick->LOAD = delay;
	SysTick->VAL = 0x00;
	while ( (SysTick->CTRL & 0x00010000) == 0) {} // Wait for Count flag (Bit 16) 
	*/
	
	// DRA-style Registers
	SYSTICK_STRVR = 300000-1;
  SYSTICK_STCVR = 0;
	while ( (SYSTICK_STCSR & 0x00010000) == 0) {} // Wait for Count flag (Bit 16) 
	
}

void SysTick_Wait1s(uint32_t delay) {
	int i;
	for (i=0;i<delay*10;i++) {
		SysTick_Wait10ms(  );
	}
}

int main() {
	  // Configure the Red LED on P1.0
	RED_LED_Int();

	/*  SRA-Style Registers
	SysTick->CTRL = 0;		// Disable SysTick during setup
	SysTick->LOAD = 0x00FFFFFF;	// maximum reload value
	SysTick->VAL = 0;			// Any write clear the COUNT bit
	SysTick->CTRL = 0x00000005; // enable, no interrupts
	*/
	
	// DRA-Style Registers
	SYSTICK_STCSR = 0;
	SYSTICK_STRVR = 300000;			  // Same value used by SysTick_Wait10ms
	SYSTICK_STCVR = 0;						// Any write clears the COUNT bit
	SYSTICK_STCSR = BIT0 | BIT2;   		// Enable, No interrupt
	
	while(1){
	RED_LED_ON(); // red LED on
    SysTick_Wait1s(1);
    RED_LED_OFF();// red LED off
    SysTick_Wait1s(3);
	}
}