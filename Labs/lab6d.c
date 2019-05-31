// MUHAMMAD TAREQUZZAMAN || 100964008
#include <stdint.h>
#include "../inc/BSP.h"
#include "../inc/msp432p401r.h"
#include "../inc/CortexM.h" 

volatile uint32_t b2N =0; 
volatile uint32_t b1N =0; 

void bsp_buttonInit(void){
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
} 

void Initialize_Interrupt(void){
		//only Port3.5 and PORT5.1 for the Button on BSP
		P3IES |= BIT5;
		P3IFG &=~ BIT5;
		P3IE |= BIT5;
	
		P5IES |= BIT1;
		P5IFG &=~ BIT1;
		P5IE |= BIT1;
	
		NVIC_IPR9 = (NVIC_IPR9&0x00FF00FF)|0x60006000; // Priority 3 
		NVIC_ISER1 = 0x000000A0; // Enable IRQ 37  and IRQ 39 
	
		__ASM("CPSIE I");
}

void PORT3_IRQHandler(void){ //PORT 3 Interrupt
		b2N ++; // count B2
		if((P3IFG & 0x20)!=0 ){
			P3IFG &= ~BIT5; 
			BSP_LCD_DrawString(0,1,"B2", BSP_LCD_Color565(255,0,0));
			BSP_LCD_SetCursor(10,1);
			BSP_LCD_OutUDec5(b2N, BSP_LCD_Color565(0,0,255));
			
		}
}

void PORT5_IRQHandler(void){// PORT 5 Interrupt 
		b1N ++; // count B1 
		if((P5IFG & 0x02)!=0 ){
			P5IFG &= ~BIT1; 
			BSP_LCD_DrawString(0,0,"B1", BSP_LCD_Color565(255,0,0));
			BSP_LCD_SetCursor(10,0);
			BSP_LCD_OutUDec5(b1N, BSP_LCD_Color565(0,0,255));
			
}
}


int main(void) {
	// Initialize Intrupt, LCD, Button //
		BSP_LCD_Init();// LCD
		bsp_buttonInit();// Button
		Initialize_Interrupt(); // Interrupt
	
	
	while(1){//emptyloop
		}

}
