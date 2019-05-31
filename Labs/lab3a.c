#include "msp.h"
/*| mT | MUHAMMAD TAREQUZZAMAN |100954008|SYSC3310 FALL2018|*/
/* DRA mode stye setup  */
int main(void)
{
			
	
			//WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // Stop watchdog timer
			WDTCTL = WDTPW | WDTHOLD; 
			
			// By default P1SEL0 P1SEL1 is 0x00 .
			/*Setting up the action mode for port 1 as default GPIO |mT|*/
			P1SEL0 = 0x00; /*Setting up the action mode for port 1 as default GPIO |mT|*/
			P1SEL1 = 0x00;
	
			// The following code toggles P1.0 port
			// Configure P1.0 as output
			P1DIR |= BIT0 ; // DRA mode set for P1DIR bit0 to 1;  
			uint32_t i; // for loop counter setup. 
				while(1) // while system is active, indefinitely will toggle the LED  
				{
					P1OUT ^= BIT0; // Toggle P1.0, P1 output of bit0 will toggle inbetween 1/0;  DRA mode 
					for(i=100000; i>0; i--); // Delay i = 10000 vs 100000;  will delay the Toggle rate. 
				}
				
}
/*Code done by MUHAMMAD TAREQUZZAMAN */