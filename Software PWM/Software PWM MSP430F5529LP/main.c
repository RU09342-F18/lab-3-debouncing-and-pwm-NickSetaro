#include <msp430.h> 


/**
 * @author Nick Setaro
 * @version 10.5.2018
 *
 * This code uses two CCR registers on timer A0 to toggle an LED at different times. This allows
 * for a pwm on the output LED. The button is also debounced using timer A1. This timer disables the button
 * interrupt until the bouncing period is over when it re-enables it.
 *
 * MSP430F5529LP
 *
 */

int main(void)
{
    //gpio
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;
	
	P1DIR |= BIT0;              // set pin 1.0 direction to output
	P4DIR |= BIT7;              // set pin 4.7 direction to output

    //button
	P1IES |= BIT1;
	P1DIR &= ~BIT1;
    P1REN |= BIT1;                   //enable resistor on pin 1.1
    P1OUT |= BIT1;                   //set resistor for pin 1.3 to pull up
    P1IE |= BIT1;                    //enable pin 1.1 interrupt
    P1IFG &= ~BIT1;                 //clears interrupt flags

    //Timer B0 for pwm
    TB0CCTL1 = CCIE;                   //enable interrupt on ccr1
    TB0CCTL0 = CCIE;                   //enable interrupt on ccr0
    TB0CCR0 = 1000;                    //set period
    TB0CCR1 = 500;                     //set duty cycle to 50%
    TB0CTL = TBSSEL_2 + MC_1;          //set SMCLK to up mode
}

//Timer B0 CCR0 for pwm
#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer0_B0 (void) {
    P1OUT |= BIT0;                      //turn pin 1.0 LED on
    TB0IV = 0;                          //reset timer interrupt flag
}

//Timer B0 CCR1 for pwm
#pragma vector = TIMER0_B1_VECTOR
__interrupt void Timer0_B1 (void) {
    P1OUT &= ~BIT0;                     //turn pin 1.0 LED off
    TB0IV = 0;                          //reset timer interrupt flag
}

//Timer B1 CCR0 for debounce
#pragma vector = TIMER1_B0_VECTOR
__interrupt void Timer1_B0 (void) {
    TB1CTL = MC_0;                      //disable timer b1
    P1IE |= BIT1;                       //enable button interrupt
    TB1R = 0;                           //reset timer b1
    P1IFG &= ~BIT1;                     //clear interrupt flag
}

//button interrupt
#pragma vector = PORT1_VECTOR
__interrupt void Port_1 (void) {
    P1IE &= ~BIT1;                      //disable button interrupt
    if (P1IES & BIT3) {                 //if low to high edge
        if (TB0CCR1 <= TB0CCR0){        //if duty cycle is past 100%
            TB0CCR1 += 100;             //increment duty cycle 10%
        } else {
            TB0CCR1 = 0;                //reset duty cycle to 0
        }
        TB1CTL = TBSSEL_2 + MC_1;           // connect timer a to SMCLK in up mode
        P4OUT ^= BIT7;                      //toggle pin 4.7 LED
        P1IFG &= ~BIT3;                     //clear interrupt flags
    }
}


