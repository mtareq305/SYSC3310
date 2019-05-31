/* |mT |Muhammad Tarequzzaman|100954008| SYSC3310 - Fall 2018 | */
/*
	this version includes  Interrupt-Driven Program using DRA mode 
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
/*timeremaining for the lights or each state.*/
volatile uint32_t timeRemaining= 10;


/*state is define by 1(GREEN), 2(YELLOW), 3(RED)*/	
volatile uint32_t state = 1 ;

/*initialize color code define variable*/
	#define RED       0x01
	#define GREEN     0x02
	#define BLUE      0x04
	#define white     0x07
	#define YELLOW    0x03
	
/*Port1 initialize here*/
void Port1_Init(void){  //configure P1.4 and P1.1
  P1SEL0 &= ~0x12;
  P1SEL1 &= ~0x12;    // 1) configure P1.4 and P1.1 as GPIO
  P1DIR &= ~0x12;     // 2) make P1.4 and P1.1 in
  P1REN |= 0x12;      // 3) enable pull resistors on P1.4 and P1.1
  P1OUT |= 0x12;      //    P1.4 and P1.1 are pull-up
}

/*Port1 input functions define here */ 
uint8_t Port1_Input(void){

  return (P1IN&0x12);   // read P1.4,P1.1 inputs
}
*Port2 initialize here*/
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

void Dump(uint32_t x){   // filter:save data only if different
  if((x!=Last)&&(Index<SIZE)){
    DataBuf[Index] = x;  // save 
    Last = x;            // set up for next time
    Index++;
  }
}


/* RGB output functions  */
/* Note: use this for RGB output using input as initialize color code  on defined variable .*/
void Port2_Output(uint8_t data){  // write three outputs bits of P2
 
	P2OUT = (P2OUT&0xF8)|data;
  Dump(((P1IN&0x12)<<8)+(P2OUT&0x07));   // debugging dump
}

/* Initialize RED LED*/
void LED1_int(void){
	P1SEL0 &= ~BIT0;
  P1SEL1 &= ~BIT0;    // 1) configure P1.4 and P1.1 as GPIO
  P1DIR |= BIT0; 
}
/* RED LED ON */	
void LED1_ON(void){
	 
	P1OUT |= BIT0 ; 
}
/* RED LED OFF */
void LED1_OFF(void){
		P1OUT &= ~BIT0;
}
/* count the edge of the time signal */
void Initialize_Interrupt(void){
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



/* Need this to counting time insted of system timer */
void delay1m (int number) {
		uint32_t i,j;
		for (i=0; i<number;i++) {
			for (j=0;j<900;j++);
		}
}

/*count down from timeRemaining Global variable */
void countTime(void){
	// Assuming nSeconds is set to your intended delay		
		while (timeRemaining > 0) {
			delay1m (1000); // 1000 ms = 1 second
					timeRemaining--;	
	}
}

/* Use TurnGreen(void) and TurnYELLOW(void) in  PORT1_IRQHandler(void)  */
void TurnGreen(void){
	/*if SW2 while the light is RED, the light
	should turn GREEN no more than 2 seconds later */
		if(state == 3){
			//Port2_Output(white); // if SW2 == 1;  test RGB should be white.
			timeRemaining = 3 ;//n+1 offset; timeRemaining = 2 + offset = 3 
			state = 1; // next GREEN	
		}
	}
/*Use TurnGreen(void) and TurnYELLOW(void) in  PORT1_IRQHandler(void)  */
void TurnYELLOW(void){
	/* If SW1 button while the light is GREEN, 
	the light should immediately turn YELLOW */	
	if(state==1){
		//Port2_Output(white); // If SW1 ==1;  test RGB should be white. 
		Port2_Output(YELLOW); 
		
		timeRemaining = 1; // ; timeRemaining = 0 + offset = 1 
		state = 2; // next  YELLOW
		}
	}
	
	
void PORT1_IRQHandler(void){
	
	FallingEdges++; //  FallingEdges = FallingEdges + 1; /*We dont use this in this part, its here to see the IRQ test*/	
		
	if ((P1IFG & 0x02) != 0) { //SW1
		P1IFG &= ~0x02;		// clear flag1, acknowledge
    /*TurnYELLOW() defined on top*/ 
		TurnYELLOW(); 		// RGB should be YELLOW
  }

	if ((P1IFG & 0x10) != 0) { //SW2
		P1IFG &= ~0x10;		// clear flag4, acknowledge
    /*TurnGreen() defined on top*/  
		TurnGreen(); 			// RGB should be  GREEN 
  }
}


int main(void){ 
			
			Port1_Init();              // initialize P1.1 and P1.4 and make them inputs
			Port2_Init();              // initialize P2.2-P2.0 and make them outputs (P2.2-P2.0 built-in LEDs)		
			Initialize_Interrupt();					// initialize P1.4 and P1.1 interrupts /*We dont use this in this part, its here to see the IRQ test*/	
			LED1_int();
	/*	NOTE: SW1 and SW2  are negative logic on P1.1 and P1.4*/
	/*	NOTE: state is define by 1=>(GREEN), 2=>(YELLOW), 3=>(RED)*/
	
	while (1) {
	/* While in the switch function, RGB will be changing in between Green, Yellow and Red.
			Unless Interrupt happen, Which will define the state as it was defined in  PORT1_IRQHandler(void) */
    switch(state){  
      case 1: //GREEN
								LED1_OFF();
								Port2_Output(GREEN); 		
								countTime();
								state = 2; // next State 
								
								break;    
			
      case 2: 	//YELLOW
								
								timeRemaining =6; //; timeRemaining = 5 + offset = 6 
								Port2_Output(YELLOW); 
								LED1_ON();
								countTime();
								state = 3;
								
								break;    
			
      case 3: 	// RED
								//state = 3;
								timeRemaining =6; //; timeRemaining = 5 + offset = 6 
								Port2_Output(RED);
								LED1_ON();	
								countTime();
								timeRemaining =10; // used here for Green light to be default value for 10 sec 
								state = 1;
								
								break;  // both switches pressed
      
			
    }//End switch function. 
	
	}// end of while(1)
			
}//end main.

/*Code done by MUHAMMAD TAREQUZZAMAN */

/*RGB[Port_2] color settings */
// Color    LED(s) Port2
// dark     ---    0
// red      R--    0x01
// blue     --B    0x04
// green    -G-    0x02
// YELLOW   RG-    0x03
// sky blue -GB    0x06
// white    RGB    0x07
// pink     R-B    0x05