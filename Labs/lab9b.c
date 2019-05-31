/*Muhammad Tarequzzaman | 100954008*/

//*****************************************************************************
//	  TA0CCR2 is the length of the low pulse
//    Smaller the value, the shorter the low pulse and the longer the high pulse
//    Theory: Need 2 ms high-pulse for 90 degrees, 1 ms high-pulse for -90 degrees, 1.5 ms high-pulse for 0 degrees
//		Practice: Somewhat different.
//    Note: worked reliably when reprogrammed to 8 MHz
//*****************************************************************************


#include "..\inc\msp432p401r.h"
uint32_t selectInterupt = 0, b2N =0, b1N = 0  ;


#define PWM_PERIOD 40000





#define PWM_PULSE_MAX  20000		  // Experienced Value = Maximum high pulse is 2 ms for +90 degrees // Calculated Value = (1 / 8 MHz ) * 20000 = 2.5 ms
#define PWM_PULSE_MIN	 4500		// Experienced Value = Maximum high pulse is 2 ms for -90 degrees // Calculated Value = (1 / 8 MHz ) * 4500 = .5625 ms

#define DELAY 500


void bsp_buttonInit_UserDif(void){
// J4.32 user Button2 (bottom) (digital) {TM4C123 PD7, MSP432 P3.5}
// J4.33 user Button1 (top) (digital)    {TM4C123 PD6, MSP432 P5.1}

	//B2 
		P3SEL0 &= ~ BIT5;
		P3SEL1 &= ~ BIT5;
		P3DIR 	&= ~ BIT5;
		P3REN	&=  ~BIT5;
	// B1
		P5SEL0 &= ~ BIT1;
		P5SEL1 &= ~ BIT1;
		P5DIR 	&= ~ BIT1;
		P5REN	&= ~BIT1;
	// select
	
		P4SEL0 &= ~BIT1;
		P4SEL1 &= ~BIT1;                 // configure P4.1 as GPIO
		P4DIR &= ~BIT1;                  // make P4.1 in
		P4REN &= ~BIT1;
} 


void PORT3_IRQHandler(void){ //PORT 3 Interrupt
		b2N ++; // count B2
		
			P3IFG &= ~BIT5; 
			
			
		
}
void PORT4_IRQHandler(void){
		
		selectInterupt++; // debug step
		
		P4IFG &= ~ BIT1; // clear the P4.1 IFG flag 
	
}
void PORT5_IRQHandler(void){// PORT 5 Interrupt 
		b1N ++; // count B1 
		P5IFG &= ~BIT1; 
			
			
}

void Initialize_Interrupt(void){
// J4.32 user Button2 (bottom) (digital) {TM4C123 PD7, MSP432 P3.5}
// J4.33 user Button1 (top) (digital)    {TM4C123 PD6, MSP432 P5.1}
	
//  Port3.5 for the Button 2 on BSP
		P3IES |= BIT5;
		P3IFG &=~ BIT5;
		P3IE |= BIT5;
	
//	PORT5.1 for the Button 1 on BSP
		P5IES |= BIT1;
		P5IFG &=~ BIT1;
		P5IE |= BIT1;

//  JOYSTIC select  P4.1 == select // @JOYSTIC 
		P4IES |= 0x02;
		P4IFG &= ~0x02;
		P4IE |=0x02;
	
	// NVIC priority ans IRQ set up 
		NVIC_IPR9 = (NVIC_IPR9&0x000000FF)|0x40404000; // Priority 2 for (Port3),(PORT4) and(PORT5)
		NVIC_ISER1 = 0x000000E0; // Enable IRQ 37(Port3), IRQ 38(PORT4)  and IRQ 39(PORT5) 

		__ASM("CPSIE I"); //  wait for Interrupts: Enable 

}

void TA0_Timer_Init(void){

	P2DIR |= BIT5;			// Configure P2.5 as output
	P2SEL1 &= ~BIT5;	
	P2SEL0 |= BIT5;			// Set P2.5 as TA0.2 PWM output pin (Primary module function)

	CSKEY = CSKEY_VAL;
	CSCTL0 = 0;
	CSCTL0 = DCOEN | DCORSEL_2 | 0x485;     	 // (From Bia, page 667) // 8MHz DCO enable 

	TA0CCTL2 = OUTMOD_3 | CCIE;
	TA0CTL = TASSEL_2 | MC_1 | TACLR;		// Divide by 1, Up Mode
	TA0CCR0 = PWM_PERIOD;
	TA0CCR2 = PWM_PULSE_MAX;

}

int main(void)
{
   
	
		WDTCTL = WDTPW | WDTHOLD;
	// init here 
		bsp_buttonInit_UserDif();     
		TA0_Timer_Init();
		Initialize_Interrupt();
	uint32_t i;
	uint32_t high = PWM_PERIOD/10;
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