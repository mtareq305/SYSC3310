//*****************************************************************************
//
// MSP432 main.c  - P2.5 port incremental-cycle for servo demo on MKII
// TA0CCR2 is the length of the low pulse
//    Smaller the value, the shorter the low pulse and the longer the high pulse
//    Theory: Need 2 ms high-pulse for 90 degrees, 1 ms high-pulse for -90 degrees, 1.5 ms high-pulse for 0 degrees
//		Practice: Somewhat different.
//    Note: Tried with SMCLK = 3 MHz, but worked reliably when reprogrammed to 8 MHz
//
//*****************************************************************************

//#include "msp.h"
#include "..\inc\msp432p401r.h"

#define PWM_PERIOD 40000
/*
//#define PWM_PULSE_MAX	 6000			// Calculated Value = (1 / 3 MHz ) * 6000 = 2 ms
//#define PWM_PULSE_MIN  3000			// Calculated Value = (1 / 3 MHz)  * 3000 = 1 ms
#define PWM_PULSE_MAX  7500		  // Experienced Value = Maximum high pulse is 2 ms for +90 degrees
#define PWM_PULSE_MIN	 1500			// Experienced Value = Maximum high pulse is 2 ms for -90 degrees
*/


//#define PWM_PULSE_MAX	 16000			// Calculated Value = (1 / 8 MHz ) * 16000 = 2 ms
//#define PWM_PULSE_MIN  8000			// Calculated Value = (1 / 8 MHz)  * 8000 = 1 ms
#define PWM_PULSE_MAX  20000		  // Experienced Value = Maximum high pulse is 2 ms for +90 degrees
#define PWM_PULSE_MIN	 4900		// Experienced Value = Maximum high pulse is 2 ms for -90 degrees

#define DELAY 800

int main(void)
{
    uint32_t i;
	  uint32_t high = PWM_PERIOD/10;
	
		WDTCTL = WDTPW | WDTHOLD;
         
	  P2DIR |= BIT5;			// Configure P2.5 as output
	  P2SEL1 &= ~BIT5;	
	  P2SEL0 |= BIT5;			// Set P2.5 as TA0.2 PWM output pin (Primary module function)
	
		CSKEY = CSKEY_VAL;
	  CSCTL0 = 0;
	  CSCTL0 = DCOEN | DCORSEL_2 | 0x485;     	 // (From Bia, page 667)
	
		TA0CCTL2 = OUTMOD_3 | CCIE;
	  TA0CTL = TASSEL_2 | MC_1 | TACLR;		// SMCLK = 3 MHz, Divide by 1, Up Mode
		TA0CCR0 = PWM_PERIOD;
		TA0CCR2 = PWM_PULSE_MAX;

		while (1) {  // Continuous movement
		// If you detect any pauses in movement, it means that you have exceeded the limits of your motor
		// and are burning it out. Change the limits in the loops below
		
			while (high > PWM_PULSE_MIN)		// CW: From -90 to +90
			{
				TA0CCR2 = PWM_PERIOD - high;
				high -= 2;
				for(i=DELAY; i>0; i--);
			}
			
			while(high < PWM_PULSE_MAX)   // CCW: +90 to -90
			{
				TA0CCR2 = PWM_PERIOD - high;
				high += 2;
				for(i=DELAY; i>0; i--);
			}	

		}
}