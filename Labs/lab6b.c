// MUHAMMAD TAREQUZZAMAN || 100964008


#include <stdint.h>
#include <msp.h>
#define OUTER_SPIN 500
#define INNER_SPIN 100

void delay(void) {
	
	uint32_t i,j,k;
	for(i=OUTER_SPIN; i>0; i--)
	    for (j=INNER_SPIN;j>0; j--)                  // Delay
			            k = 1;
}
void bsp_LED_Init(void){
	// RED GREEN BLUE 
		P2SEL1 &= ~(BIT4 | BIT6);
		P2SEL0 &= ~(BIT4 | BIT6);
		P2DIR |= BIT4 | BIT6;

		P5SEL1 &= ~BIT6;
		P5SEL0 &= ~BIT6;
		P5DIR |= BIT6;

		P2OUT &= ~(BIT4 | BIT6);
		P5OUT &= ~BIT6;
}

void bsp_buttonInit(void){
// J4.32 user Button2 (bottom) (digital) {TM4C123 PD7, MSP432 P3.5}
// J4.33 user Button1 (top) (digital)    {TM4C123 PD6, MSP432 P5.1}

	//B2 
		P3SEL0 &= ~ BIT5;
		P3SEL1 &= ~ BIT5;
		P3DIR 	&= ~ BIT5;
		P3REN		&=  ~BIT5;
	// B1
		P5SEL0 &= ~ BIT1;
		P5SEL1 &= ~ BIT1;
		P5DIR 	&= ~ BIT1;
		P5REN		&= ~BIT5;
} 
int main(void) {
	
		WDTCTL = WDTPW | WDTHOLD; // watchdohg timer off;
		bsp_LED_Init();// BSP LED init : user defined 
		bsp_buttonInit();// BSP button init : user defined 
			
	while(1){
		delay(); // delay for SW bouncing 
		if ((P5IN & 0x02)==0 ){//B1
			P2OUT ^= BIT6; //RED
			P2OUT &= ~ BIT4; //GREEN
			 }
		if((P3IN & 0x020)==0){//B2
			P2OUT &=~ BIT6; //RED
			P2OUT ^= BIT4;//GREEN
			
			}
	
		}
	
 }

