#include <msp430.h>

/*
 * @author Nick Setaro
 * @version 10.4.2018
 *
 * This code uses two CCR registers on timer A0 to toggle an LED at different times. This allows
 * for a pwm on the output LED. The button is also debounced using timer A1. This timer disables the button
 * interrupt until the bouncing period is over when it re-enables it.
 *
 * MSP430G2553
 *
 */

int main(void)
{
    //gpio
	 WDTCTL = WDTPW | WDTHOLD;	        // stop watchdog timer
	 P1DIR |= BIT0;                     //set pin 1.0 LED to output
	 P1DIR |= BIT6;                     //set pin 1.6 LED to output
	 P1OUT &= ~(BIT0);                  //turn pin 1.0 LED off
	 P1OUT &= ~(BIT6);                  //turn pin 1.6 LED off

	 //button
	 P1REN |= BIT3;                     //enable resistor on pin 1.3
	 P1DIR &= ~BIT3;                    //set pin 1.3 to input
	 P1OUT |= BIT3;                     //set pin 1.3 resistor to pull up
	 P1IE |= BIT3;                      //enable button interrupt
	 P1IES |= BIT3;                    //set button edge to low
	 P1IFG &= ~BIT3;                    //clear interrupt flags

	 //timer a0 for pwm
	 TA0CCTL1 = CCIE;                   //enable interrupt on ccr1
	 TA0CCTL0 = CCIE;                   //enable interrupt on ccr0
	 TA0CCR0 = 1000;                    //set period
	 TA0CCR1 = 500;                     //set duty cycle to 50%
	 TA0CTL = TASSEL_2 + MC_1;          //set SMCLK to up mode

	 //timer a1 for debounce
	 TA1CCTL0 = CCIE;                  // enable timer a interrupt
	 TA1CCR0 = 60000;                    // overflow value for timer a
	 TA1CTL = TASSEL_2 + MC_0;           // connect timer a to SMCLK and turn off
	 __enable_interrupt();
	 while(1);

}

//Timer A0 CCR1 for pwm
#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer_A1 (void) {
    P1OUT &= ~BIT0;                     //turn pin 1.0 LED off
    TA0IV = 0;                          //reset timer interrupt flag
}

//Timer A0 CCR0 for pwm
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A0 (void) {
    P1OUT |= BIT0;                      //turn pin 1.0 LED on
    TA0IV = 0;                          //reset timer interrupt flag
}

//Timer A1 CCR1 interrupt for button debounce
#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1_A0 (void) {
    TA1CTL = MC_0;                      // disable timer
    P1IE |= BIT3;                       // enable button interrupt
    P1IFG &= ~BIT3;                     // clear interrupt flag
    TA1R = 0;                           //reset timer
}



//button interrupt
#pragma vector=PORT1_VECTOR
__interrupt void Port_1 (void){
    P1IE &= ~BIT3;                      //disable button interrupt
    if(P1IES & BIT3){
    if (TA0CCR1 <= TA0CCR0){            //if duty cycle is not above 100%
        TA0CCR1 += 100;                 //increment ccr0
    } else {
        TA0CCR1 = 1;                    //reset ccr1
    }
    }
    TA1CTL = TASSEL_2 + MC_1;           // connect timer a to SMCLK in up mode
    P1OUT ^= BIT6;                      //toggle pin 1.6 LED
    P1IFG &= ~BIT3;                     //clear interrupt flags
}

