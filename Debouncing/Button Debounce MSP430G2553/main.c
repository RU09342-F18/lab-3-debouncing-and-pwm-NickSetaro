#include <msp430.h>

/*
 * @author Nick Setaro
 * @version 10.4.2018
 *
 * This code uses a button interrupt to toggle and LED. Each button interrupt disables the interrupt. The timer
 * then re-enables the interrupt after a delay to debounce the button.
 *
 * MSP430G2553
 */

int main(void)
{
    //gpio
    WDTCTL = WDTPW | WDTHOLD;           // stop watchdog timer
    P1DIR  |= BIT0;                     // sets the output direction

    //button set up
    P1REN  |= BIT3;                     // enable resistor on pin 1.3
    P1OUT  = BIT3;                      // pull up resistor on pin 1.3
    P1IE |= BIT3;                       // enable button interrupt
    P1IES = 0;                          // set edge to high
    P1IFG &= ~BIT3;                     // clear interrupt flag

    //timer setup
    TA0CCTL0 = CCIE;                  // enable timer a interrupt
    TA0CCR0 = 60000;                    // overflow value for timer a
    TA0CTL = TASSEL_2 + MC_0;           // connect timer a to SMCLK and turn off

        __enable_interrupt();               //enables the interrupt
        __bis_SR_register(LPM0 + GIE);      // Low Power Mode w/ general interrupts enabled
        while(1);
    }

    // button interrupt
    #pragma vector = PORT1_VECTOR
    __interrupt void Port_1(void)
    {
        P1OUT ^= BIT0;                      // toggle pin 1.0 red LED
        P1IE &= ~BIT3;                      // disable interrupt
        TA0CTL = TASSEL_2 + MC_1;           // connect timer a to SMCLK in up mode
    }
    // timer interrupt
    #pragma vector=TIMER0_A0_VECTOR
    __interrupt void Timer_A0(void)
    {
        TA0CTL = MC_0;                      // disable timer
        P1IE |= BIT3;                       // enable button interrupt
        P1IFG &= ~BIT3;                     // clear interrupt flag
        TA0R = 0;                           //reset timer
    }
