// Muhammad TareqUzzaman |100954008|

#include <stdint.h>
#include "../inc/msp432p401r.h"
#include "../inc/CortexM.h"  
// Needed for SYSPRI3
// Note: This program does not use any FUNCTIONs in CortexM.h.
// If a function is used, you also need add Corresponding  CortexM.c to the project

// Built from  Keil_v5\MSP432ValvanoWare\SysTickInts_MSP432

volatile uint32_t counts = 0;		// Global variables for tracking
volatile uint32_t H = 0;
volatile uint32_t L = 0;
volatile uint32_t i = 0;
volatile uint32_t delay = 0;
volatile uint16_t FLAG = 0;
volatile uint16_t D= 5;

/*Port2 initialize here*/
void Port2_Init(void){ //configure P2.2 - P2.0
  P2SEL0 &= ~0x07;
  P2SEL1 &= ~0x07;	  // 1) configure P2.2-P2.0 as GPIO
  P2DIR |= 0x07;	  // 2) make P2.2-P2.0 out
  P2DS |= 0x07;		  // 3) activate increased drive strength
  P2OUT &= ~0x07;	  //	all LEDs off
}

/* Initialize RED LED*/
void RED_LED_Int(void){
	P1SEL0 &= ~BIT0;
	P1SEL1 &= ~BIT0;	// 1) configureout put GPIO
	P1DIR |= BIT0;
	P1OUT &= ~BIT0; //off
	}



void SysTick_Init(void){
	
		SYSTICK_STCSR = 0;		// Disable SysTick during setup
		SYSTICK_STRVR = 4-1;   //300000-1;	// If clock is 3 MHz, period is 333ns. Maximum is 2^24-1. Minimum = length of ISR
	//1sec == 3330000-1 // 3 == 1uSec 
		SYSTICK_STCVR = 0;	// Any write clear the COUNT bit
		
		SYSPRI3 = (SYSPRI3&0x00FFFFFF)|0x40000000; // priority 2
		SYSTICK_STCSR = BIT0 | BIT1 | BIT2;   		// 0x00000007 to enable interrupts
		__ASM("CPSIE I");
	
	}

void LaunchPad_Init(void){
	Port2_Init();	// Configure Port2
	RED_LED_Int();	// Configure the Red LED on P1.0
	/*Systic config*/
	
}


const uint32_t PulseBuf[100]={
	 5000, 5308, 5614, 5918, 6219, 6514, 6804, 7086,
	 7361, 7626, 7880, 8123, 8354, 8572, 8776, 8964,
	 9137, 9294, 9434, 9556, 9660, 9746, 9813, 9861,
	 9890, 9900, 9890, 9861, 9813, 9746, 9660, 9556,
	 9434, 9294, 9137, 8964, 8776, 8572, 8354, 8123,
	 7880, 7626, 7361, 7086, 6804, 6514, 6219, 5918,
	 5614, 5308, 5000, 4692, 4386, 4082, 3781, 3486,
	 3196, 2914, 2639, 2374, 2120, 1877, 1646, 1428,
	 1224, 1036, 863, 706, 566, 444, 340, 254,
	 187, 139, 110, 100, 110, 139, 187, 254,
	 340, 444, 566, 706, 863, 1036, 1224, 1428,
	 1646, 1877, 2120, 2374, 2639, 2914, 3196, 3486,
	 3781, 4082, 4386, 4692};

 void count_uSec(uint32_t delay){ // count_uSec counts microsec 

	 SYSTICK_STRVR =(delay*5);
	
}
 
// Interrupt service routine
// Executed every 333ns*(period)
void SysTick_Handler(void){   // Name must be identical to that in startupxxxx.s file in Keil
	
	//P2OUT ^=0x02;
  //count_uSec(1000000); // use this fo test the time
	
//Look up a new H= PulseBuf [i] value
//Calculate L = 10000-H
//Wait H µs using your SysTick_Wait1us function
	
	H = PulseBuf[i];  
	L = 10000-H;
	
		if (FLAG == 0 ) {  
	
			P1OUT |= BIT0; // RED LED on /off
			count_uSec(H);
			FLAG = 1;
			//after 0, it will run L
		}
		else if(FLAG == 1){//Wait L µs using your SysTick_Wait1us function
			P1OUT &= ~BIT0;
			count_uSec(L);
			//FLAG to default value, after 0, it will run H
			FLAG = 0;
		}
		if(i <99 ){i++;}// Set i ++ and always bellow 100 : array size is 100. 
		else{ i = 0;}
		

}



int main() {
	//Initialize LunchPad and systick   
	LaunchPad_Init();
	SysTick_Init();
		
		while(1){
		}
}