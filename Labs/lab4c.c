/* |mT |Muhammad Tarequzzaman|100954008| SYSC3310 - Fall 2018 | */
/*
	this includes  Interrupt-Driven Program
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
// increments at least once per button press
volatile uint32_t FallingEdges = 0;



/*initialize color code define variable*/
#define RED       0x01
#define GREEN     0x02
#define BLUE      0x04
#define white     0x07



void Port1_Init(void){  //configure P1.4 and P1.1
  P1SEL0 &= ~0x12;
  P1SEL1 &= ~0x12;    // 1) configure P1.4 and P1.1 as GPIO
  P1DIR &= ~0x12;     // 2) make P1.4 and P1.1 in
  P1REN |= 0x12;      // 3) enable pull resistors on P1.4 and P1.1
  P1OUT |= 0x12;      //    P1.4 and P1.1 are pull-up
}

uint8_t Port1_Input(void){

  return (P1IN&0x12);   // read P1.4,P1.1 inputs
}

void Port2_Init(void){ //configure P2.2 - P2.0
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


void delay1m (int number) {
uint32_t i,j;
for (i=0; i<number;i++) {
for (j=0;j<900;j++);
}
}

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


void EdgeCounter_Init(void){
	__ASM("CPSID I");
  FallingEdges = 0;                // initialize counter
	Port1_Init();            				 // initialize P1.1 and P1.4 and make them inputs
	/* interrupt flag config for P1.1 and P1.4 */
  P1IES |= 0x12;                   // P1.4 and P1.1 are falling edge event
  P1IFG &= ~0x12;                  // clear flag4 and flag1 (reduce possibility of extra interrupt)
  P1IE |= 0x12;                    // arm interrupt on P1.4 and P1.1

  NVIC_IPR8 = (NVIC_IPR8&0x00FFFFFF)|0x40000000; // priority 2
  NVIC_ISER1 = 0x00000008;         // enable interrupt 35 in NVIC

	__ASM("CPSIE I");								// Globally enable interrupts
}

void PORT1_IRQHandler(void){
		FallingEdges++; //  FallingEdges = FallingEdges + 1;
		delay1m (500);
	if ((P1IFG & 0x02) != 0) {
		P1IFG &= ~0x02;		// clear flag1, acknowledge
    Port2_Output(BLUE); // RGB should be BLUE
		
  }

	if ((P1IFG & 0x10) != 0) { 
		P1IFG &= ~0x10;		// clear flag4, acknowledge
    Port2_Output(RED); // RGB should be  RED 
		
  }
}







int main(void){ uint8_t status;
			Port1_Init();              // initialize P1.1 and P1.4 and make them inputs
			Port2_Init();              // initialize P2.2-P2.0 and make them outputs (P2.2-P2.0 built-in LEDs)		
			EdgeCounter_Init();					// initialize P1.4 and P1.1 interrupts
	
			


}//end main.
/*Code done by MUHAMMAD TAREQUZZAMAN */

// Color    LED(s) Port2
// dark     ---    0
// red      R--    0x01
// blue     --B    0x04
// green    -G-    0x02
// yellow   RG-    0x03
// sky blue -GB    0x06
// white    RGB    0x07
// pink     R-B    0x05