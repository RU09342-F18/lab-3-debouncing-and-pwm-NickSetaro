#include <msp432.h>

/*
 * @author Nick Setaro
 * @version 10.4.2018
 *
 * This code uses a button to toggle the state of an LED. A timer is used to disable the button interrupt for a short time.
 * This prevents bounced button interrupts from being registered.
 */
void main(void)
{
    //gpio
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // Disable the watchdog timer
    P2DIR |= BIT2;  // set pin 2.2 LED to output

    //button
    P1DIR &= ~BIT1;  // set pin 1.1 button to input
    P1REN |= BIT1;   // enable resistor on pin 1.1
    P1OUT |= BIT1;   // pull up resistor on pin 1.1
    P1IES |= BIT1;   // set interrupt edge to low
    P1IE |= BIT1;    // enable button interrupt
    P1IFG &= ~BIT1;  // clear interrupt flags

    //interrupt
    NVIC->ISER[1] = 1 << ((PORT1_IRQn) & 31);   // Interrupt enable for P1
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);   // Interrupt enable for TimerA0

    //timer a for debounce
    TA0CCTL0 = CCIE;    // enable ccr interrupt

    __enable_irq(); // Enable global interrupt
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;    // Disable the sleep on exit feature
    __sleep();  // Sleep Mode
    while(1);   // Infinite while loop
}

// button interrupt
void PORT1_IRQHandler(void)
{
    P2OUT ^= BIT2;  // toggle pin 2.2 LED
    P1IE &= ~BIT1;  // disable button interrupt
    P1IFG &= ~BIT1; // clear interrupt flags
    TA0CTL = TASSEL_2 + MC_1 + TACLR;   // connect timer a to SMCLK in up mode and clear
    TA0CCR0 = 10000;    // set timer a overflow
}

// timer a interrupt for debounce
void TA0_0_IRQHandler(void)
{
    TA0CTL = MC_0 + TACLR; // stop timer a and clear
    P1IE |= BIT1;   // enable interrupt on button
    P1IFG &= ~BIT1; // clear interrupt flags
}
