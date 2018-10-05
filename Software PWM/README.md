# Software PWM
Pulse width modulation can be done using either software or hardware. This code implements PWM using software and uses it to toggle an LED to change its brightness. Two Capture and Compare Registers are used to interrupt the timer and toggle the LED.
```c
#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer_A1 (void) {
    P1OUT &= ~BIT0;                     //turn pin 1.0 LED off
    TA0IV = 0;                          //reset timer interrupt flag
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A0 (void) {
    P1OUT |= BIT0;                      //turn pin 1.0 LED on
    TA0IV = 0;                          //reset timer interrupt flag
}
```
These two timer interrupts toggle the LED and clear the timer interrupt flag to allow the interrupt to occur again. A second timer is used to debounce the switch. This timer enables the button interrupt after delaying it to account for bouncing.
```c
#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1_A0 (void) {
    TA1CTL = MC_0;                      // disable timer
    P1IE |= BIT3;                       // enable button interrupt
    P1IFG &= ~BIT3;                     // clear interrupt flag
    TA1R = 0;                           //reset timer
}
```
After the interrupt occurs the timer is disabled and reset. It is then started again after the button is pressed.
```c
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
```
When the button is pressed the interrupt is disabled until enabled again by the delay timer and the delay timer is started again. An if statement determines whether to increment or reset the duty cycle.
