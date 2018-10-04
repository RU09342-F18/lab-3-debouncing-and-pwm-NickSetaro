#include <msp430.h>


/*
 *@author Nick Setaro
 *@version 10.4.2018
 *
 *This code uses a timer to drive a pwm signal to an LED to control the brightness. A button is then used to increase the duty cycle
 *by 10% or reset to 0% if its already at 100%. The button is debounced using another timer. This button disables its interrupt when it is pressed
 *and turns the timer on. The timer then re-enables the button interrupt after a delay debouncing the button.
 */

int main(void)
{
    //gpio
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    P1DIR |= BIT0;              // set pin 1.0 direction to output
    P2DIR |= BIT0;              // set pin 2.0 direction to output
    P2SEL0 |= (BIT0);              //sets pin 2.0 as output
    P2SEL1 &= ~BIT0;

    //timer B1 for PWM
    TB1CTL = TBSSEL_2 + MC_1 + ID_1;    // set Timer B to SMLCK in up mode with a clock divider
    TB1CCR0 = 1000;                         // set period
    TB1CCR1 = 500;                         // set duty cycle to 50%
    TB1CCTL1 = OUTMOD_7;                   // set timer to set/reset mode

    //timer B0 for debounce
    TB0CCTL0 = CCIE;                    //enable ccr interrupt
    TB0CCR0 = 10000;                    //set timer overflow
    TB0CTL = TBSSEL_2 + MC_0;           // connects timer b to SMCLK and turn off

    //button
    P1DIR &= ~BIT1;                      //set pin 1.3 direction to input
    P1REN |= BIT1;                       //enable resistor on pin 1.3
    P1OUT |= BIT1;                       //set resistor to pull up
    P1IE |= BIT1;                       //enable interrupt on button
    P1IES |= BIT1;                      //set interrupt edge to low
    P1IFG &= ~BIT1;                     //clear interrupt flags

    __bis_SR_register(GIE); // LPM0 with general interrupts enabled
    while(1);
}

//button interrupt
#pragma vector=PORT1_VECTOR         //indicate interrupt vector being used
__interrupt void Port_1(void)
{
    P1IE &= ~BIT1;                          //disable button interrupt
    if(P1IES & BIT1){
            if (TB1CCR1<1000) {             //if duty cycle did not overflow
                TB1CCR1+=100;               //increment duty cycle by 10%
            } else if (TB1CCR1 >=1000) {    //if the duty cycle overflows (goes over 100%)
                TB1CCR1 = 0;                //reset duty cycle
            }
    }
           P1IES ^= BIT1;                   //toggle edge
           P1OUT ^= BIT0;                   //toggle pin 1.0 LED
           P1IFG &= ~BIT1;                  //clear flags
           TB0CTL = TBSSEL_2 + MC_1;        // connect timer b0 to SMCLK in up mode
}

//timer B0 interrupt for debounce
#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer_B (void) {
    TB0CTL = MC_0;                          //stop timer
    TB0R = 0;                               //reset timer b0
    P1IE |= BIT1;                           //enable button interrupt
    P1IFG &= ~BIT1;                         //clear interrupt flag
}
