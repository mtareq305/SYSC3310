#include <stdint.h>
//#include <msp.h>
#include "../inc/msp432p401r.h"
// TA0
// Bai, Page 641 TA0 UP mode (does not use TAxCCR0 in this mode)
//   Using ID = divide by 8 and TAIDEX = divide by 5 to get a total divide by 40.
//   Removes the need for global counter-divide in ISR
		  // Since SMCLK = 3 MHz, the period is 330 ns.
	    // Since using continuous mode, interrupt after 0...FFFF
	    // So the interrupt period is 330 ns * 65535 = 21, 626, 550 ns = 22,000 us = 22 ms (About 1/40th second)
			// With a total divide of 40, that means the interrupt period is now about 1 second

void TA0_N_IRQHandler(void) {
	static uint32_t i=0;
	TA0CTL &= ~0x0001;    	// Acknowledge Interrupt 0

	P1OUT ^= BIT0;
    
}

int main(void) {
	
	WDTCTL = WDTPW | WDTHOLD;
	
	P1SEL1 &= ~(BIT0);
	P1SEL0 &= ~(BIT0);
	P1DIR |= BIT0;	// 1 for Output
		
	// Using the initialization from Valvano Volumne I, page 357, Program 9.8
	TA0CTL &= ~0x0030; 	// User function - Clear Bit 4 and 5 - Stop mode, save power
	TA0CTL = 0x0202;		// COMPARE interrupts, no I/O 
	// Bit 9:8 = 10 - SMCLK
	// Bit 7:6 = 11 - Divide by 8 // note: this value change the interval of caling IRQ, make small divide to create more frequncy 
	// Bit 5:4 = 00 - Going to be Continuous Mode (timer counts up to 0xFFFF) but for now STOP mode
	// Bit 2 TACLR = 1 - Timer_A clear. SETTING this bit reset TAxR
	// Bit 1 TAIE = 1 - Enable interrupt
	// Bit 0 TAIFG = 0 - No interrupt pending.
		
	TA0EX0 = 0x0004;	// TAIDEX = 100 for Divide by 5 Input Divider expansion
  NVIC_IPR2 = (NVIC_IPR2&0xFFFF00FF)|0x00004000; // priority 2 in bits 13-15
  NVIC_ISER0 |= BIT9;				// enable interrupt 9 in NVIC

  TA0CTL |= 0x0024;        // reset and start Timer A0 in CONTINUOUS (MC = 10)
	
	__ASM("CPSIE I");
	
	while(1){
		__ASM("wfi");
	}
}