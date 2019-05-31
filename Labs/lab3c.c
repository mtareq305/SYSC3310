/* |mT |Muhammad Tarequzzaman|100954008| SYSC3310 - Fall 2018 | */
/*
	Modified InputOutput.c this includes Switch bouncing redundancy 
	using delay by simple for loop for i = 5. 
	~ mT */
	
	


/* 
 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
 http://users.ece.utexas.edu/~valvano/
 
   InputOutput.c - Schramm's version of Valvano's InputOutput.c
   Modifications: Removed Port Mapping (later) 
 Runs on MSP432
 Test the switch initialization functions by setting the LED
 color according to the status of the switches.
 Daniel and Jonathan Valvano
 July 19, 2016
 */

// built-in LED1 connected to P1.0
// negative logic built-in Button 1 connected to P1.1
// negative logic built-in Button 2 connected to P1.4
// built-in red LED connected to P2.0
// built-in green LED connected to P2.1
// built-in blue LED connected to P2.2

#include <stdint.h>
/* include the inc folder in "C:\Keil_v5\ARM\ARMCC\inc "  from Valvanoware */ 
#include "../inc/msp432p401r.h"  

// Color    LED(s) Port2
// dark     ---    0
// red      R--    0x01
// blue     --B    0x04
// green    -G-    0x02
// yellow   RG-    0x03
// sky blue -GB    0x06
// white    RGB    0x07
// pink     R-B    0x05

/*initialize color code define variable*/
#define RED       0x01
#define GREEN     0x02
#define BLUE      0x04
#define white     0x07



void Port1_Init(void){
  P1SEL0 &= ~0x12;
  P1SEL1 &= ~0x12;    // 1) configure P1.4 and P1.1 as GPIO
  P1DIR &= ~0x12;     // 2) make P1.4 and P1.1 in
  P1REN |= 0x12;      // 3) enable pull resistors on P1.4 and P1.1
  P1OUT |= 0x12;      //    P1.4 and P1.1 are pull-up
}
uint8_t Port1_Input(void){
	/* Creating a delay to prevent Switch-Bouncing |mT|*/
	uint8_t i;
	for(i=5; i>0; i--);		// Delay to prevent switch bouncing // ignore the empty warnning! 
  return (P1IN&0x12);   // read P1.4,P1.1 inputs
}
void Port2_Init(void){
  P2SEL0 &= ~0x07;
  P2SEL1 &= ~0x07;    // 1) configure P2.2-P2.0 as GPIO
  P2DIR |= 0x07;      // 2) make P2.2-P2.0 out
  P2DS |= 0x07;       // 3) activate increased drive strength
  P2OUT &= ~0x07;     //    all LEDs off
}

//  Demonstrating memory-based Debugging
#define SIZE 10
uint32_t DataBuf[SIZE];  // debugging dump
uint32_t Last=0xFFFF;    // pattern that will not match
uint32_t Index=0;
void Dump(uint32_t x){   // filter:save data only if different
  if((x!=Last)&&(Index<SIZE)){
    DataBuf[Index] = x;  // save 
    Last = x;            // set up for next time
    Index++;
  }
}
void Port2_Output(uint8_t data){  // write three outputs bits of P2
  P2OUT = (P2OUT&0xF8)|data;
  Dump(((P1IN&0x12)<<8)+(P2OUT&0x07));   // debugging dump
}

int main(void){ uint8_t status;
  Port1_Init();              // initialize P1.1 and P1.4 and make them inputs
  Port2_Init();              // initialize P2.2-P2.0 and make them outputs (P2.2-P2.0 built-in LEDs)
	
	/*Initializing nSW1, nSW2 and nBothSWs are counter for the switches |mT|*/
	
		uint32_t nSW1 = 0;
		uint32_t nSW2 = 0;
		uint32_t nBothSWs = 0;
	
	while(1){
		
    status = Port1_Input();/* Creating a delay to prevent Switch-Bouncing inside Port1_Input() function |mT|*/
		
    switch(status){  // switches are negative logic on P1.1 and P1.4
      case 0x10: 
								Port2_Output(BLUE); 
								/*nSW1 counte SW1 press |mT|*/
								nSW1++; 
								break;   // SW1 pressed
      case 0x02: 
								Port2_Output(RED); 
								/*nSW2 counte SW2 press |mT|*/
								nSW2++;
								break;    // SW2 pressed
      case 0x00: 
								Port2_Output(white); // insted of green its white now! easy to see wthich switch pressed first!!
			
								/*nBothSWs counte SW1 & SW2 altogather press |mT|*/
								nBothSWs++;
								break;  // both switches pressed
      case 0x12: 
								Port2_Output(0); 
								break;      // neither switch pressed
			
    }//End switch function. 
		
  }//End While.

}//end main.
/*Code done by MUHAMMAD TAREQUZZAMAN */
