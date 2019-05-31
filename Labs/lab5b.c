//#include "../inc/CortexM.h"// And add CortexM.c to your project
#include <stdint.h>
#include "../inc/msp432p401r.h"
/*RGB[Port_2] color settings */
// Color	LED(s) Port2
// dark		---	   0
// red		R--	   0x01
// blue		--B	   0x04
// green	-G-	   0x02
// YELLOW	RG-	   0x03
// sky blue -GB	   0x06
// white	RGB	   0x07
// pink		R-B	   0x05
/*Port1 initialize here*/
/*initialize color code define variable*/
#define RED		  0x01
#define GREEN	  0x02
#define BLUE	  0x04
#define white	  0x07
#define YELLOW	  0x03
#define dark	  0x00
/*Port1 initialize here*/
void Port1_Init(void){	//configure P1.4 and P1.1
  P1SEL0 &= ~0x12;
  P1SEL1 &= ~0x12;	  // 1) configure P1.4 and P1.1 as GPIO
  P1DIR &= ~0x12;	  // 2) make P1.4 and P1.1 in
  P1REN |= 0x12;	  // 3) enable pull resistors on P1.4 and P1.1
  P1OUT |= 0x12;	  //	P1.4 and P1.1 are pull-up
}
/*Port1 input functions define here */
uint8_t Port1_Input(void){
  return (P1IN&0x12);	// read P1.4,P1.1 inputs
}
/*Port2 initialize here*/
void Port2_Init(void){ //configure P2.2 - P2.0
  P2SEL0 &= ~0x07;
  P2SEL1 &= ~0x07;	  // 1) configure P2.2-P2.0 as GPIO
  P2DIR |= 0x07;	  // 2) make P2.2-P2.0 out
  P2DS |= 0x07;		  // 3) activate increased drive strength
  P2OUT &= ~0x07;	  //	all LEDs off
}
//	Demonstrating memory-based Debugging
	#define SIZE 10
	uint32_t DataBuf[SIZE];	 // debugging dump
	uint32_t Last=0xFFFF;	 // pattern that will not match
	uint32_t Index=0;
void Dump(uint32_t x){	 // filter:save data only if different
  if((x!=Last)&&(Index<SIZE)){
	DataBuf[Index] = x;	 // save
	Last = x;			 // set up for next time
	Index++;
  }
}
/* RGB output functions	 */
/* Note: use this for RGB output using input as initialize color code  on defined variable .*/
void RGB_Output(uint8_t data){	// write three outputs bits of P2
	P2OUT = (P2OUT&0xF8)|data;
	Dump(((P1IN&0x12)<<8)+(P2OUT&0x07));	 // debugging dump
}
/* Initialize RED LED*/
void RED_LED_Int(void){
	P1SEL0 &= ~BIT0;
	P1SEL1 &= ~BIT0;	// 1) configureout put GPIO
	P1DIR |= BIT0;
	P1OUT &= ~BIT0; //off
	}
/* RED LED ON */
void RED_LED_ON(void){
	P1OUT |= BIT0 ;
}
/* RED LED OFF */
void RED_LED_OFF(void){
	P1OUT &= ~BIT0;
}
/*SYSTIC CONFIG */
void SysTick_Init(void){
	SYSTICK_STCSR = 0;				// Disable SysTick during setup
	SYSTICK_STRVR = 1;				// Same value used by SysTick_Waitus
									//If clock is 3 MHz, period is 333ns. Maximum is 2^24-1. Minimum = length of ISR
	SYSTICK_STCVR = 0;				// Any write clears the COUNT bit
	SYSTICK_STCSR |= BIT0 | BIT2;	// Core clock Enable; Enable SysTick counter|| 0x000000005
}

void SysTick_Wait1us(uint32_t delay){ // 1 microSec delay: delay >=1;
	SYSTICK_STRVR = delay*3 ;
	SYSTICK_STCVR = 0;
	while ( (SYSTICK_STCSR & 0x00010000) == 0) {}
}
void LaunchPad_Init(void){
	Port2_Init();	// Configure Port2
	RED_LED_Int();	// Configure the Red LED on P1.0
	/*Systic config*/
	SysTick_Init();
}
// Array used in this lab to create sine wave
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
int main() {
	LaunchPad_Init();
	//	/*TEST :: SysTick_Wait1us(1 Sec):: 1 Sec = 1000000 uSec*/
//	while(1){ // RGB should go on off for i Sec interval.
//		RGB_Output(GREEN);
//	  SysTick_Wait1us(1000000);
//		RGB_Output(BLUE);
//	  SysTick_Wait1us(1000000);
//		}

	while(1){
		uint32_t i = 0;
		uint32_t H = 0;
		uint32_t L = 0;
		for (i = 0;i <=100;i++ ){
			H = PulseBuf[i]; //Look up a new H= PulseBuf [i] value
			L = 10000-H; //Calculate L = 10000-H
			RED_LED_ON(); //Set P1.0 high
			SysTick_Wait1us(H); //Wait H µs using your SysTick_Wait1us function
			RED_LED_OFF();//Clear P1.0 low
			SysTick_Wait1us(L);//Wait L µs using your SysTick_Wait1us function
		}
	}
}