#include "..\inc\msp432p401r.h"
#include "..\inc\CortexM.h"


// Array used in this lab to create sine wave
const uint32_t PulseBuf[100]={
5000, 5308, 5614, 5918, 6219, 6514, 6804, 7086,
7361, 7626,
7880, 8123, 8354, 8572, 8776, 8964,
9137, 9294, 9434, 9556,
9660, 9746, 9813, 9861,
9890, 9900, 9890, 9861, 9813, 9746,
9660, 9556,
9434, 9294, 9137, 8964, 8776, 8572, 8354, 8123,
7880, 7626, 7361, 7086, 6804, 6514, 6219, 5918,
5614, 5308,
5000, 4692, 4386, 4082, 3781, 3486,
3196, 2914, 2639, 2374,
2120, 1877, 1646, 1428,
1224, 1036,  863,  706,  566,  444,
340,  254,
187,  139,  110,  100,  110,  139,  187,  254,
340,  444,  566,  706,  863, 1036, 1224, 1428,
1646, 1877,
2120, 2374, 2639, 2914, 3196, 3486,
3781, 4082, 4386, 4692};

void SysTick_Init(void) {

	// Polled
	SYSTICK_STRVR = 0x00FFFFFF;	//maximum reload value (SysTick->LOAD)
  SYSTICK_STCSR = 0x00000005;	// enable, no interrupts (SysTick->CTL)
	
	// Interrupt
	SYSTICK_STCSR = 0;		// Disable SysTick during setup
	SYSTICK_STRVR = 300000-1;	// If clock is 3 MHz, period is 333ns. Maximum is 2^24-1. Minimum = length of ISR
	SYSTICK_STCVR = 0;	// Any write clear the COUNT bit
		
	SYSPRI3 = (SYSPRI3&0x00FFFFFF)|0x40000000; // priority 2
	SYSTICK_STCSR = BIT0 | BIT1 | BIT2;   		// 0x00000007 to enable interrupts
		
	EnableInterrupts();
}


void LaunchPad_Init(void) {
	  // Configure the Red LED on P1.0
		P1DIR |= 0x01;			
	  P1SEL1 &= ~0x01;
	  P1SEL0 &= ~0x00;
}

volatile uint32_t counts = 0;		// Global variables for tracking
uint8_t i = 0;
#define HIGH 0
#define LOW  1
uint8_t state = HIGH;

// Executed every 333ns*(period) if using 3 MHz
void SysTick_Handler(void){   // Name must be identical to that in startupxxxx.s file. 

  counts = counts + 1;
	if (state == HIGH) {
		state = LOW;
		SYSTICK_STRVR = (10000-PulseBuf[i])*16;
		SYSTICK_STCVR = 0;		
		P1OUT &= ~BIT0;
	} else {
		state = HIGH;
		i = i+1;
		if (i == 100) i = 0;
		SYSTICK_STRVR = (PulseBuf[i]*16);
		SYSTICK_STCVR = 0;
		P1OUT |= BIT0;
	}
}


int main(void) {
	//	Clock_Init48MHz();		// Program does not work if used. TBD: Why?
  SysTick_Init();
	LaunchPad_Init();
	
	while(1) { 	}
}
	