# Software Debouncing
Buttons are mechanical devices that are prone to mechanical errors. When the button is pressed it can sometimes "bounce" between on and off states before resting on off. These rapid switches can be registered and one button press can be registered multiple times. This code uses a timer to disable the button for a short period of time until the bounce period is over. 

#Button Interrupt
The following code is the button interrupt that occurs when the button is pressed.
```c
  #pragma vector = PORT1_VECTOR
    __interrupt void Port_1(void)
    {
        P1OUT ^= BIT0;                      // toggle pin 1.0 red LED
        P1IE &= ~BIT3;                      // disable interrupt
        TA0CTL = TASSEL_2 + MC_1;           // connect timer a to SMCLK in up mode
    }
```
When this interrupt occurs it disables the button interrupt to prevent bouncing. It then starts the debounce timer.

# Timer Interrupt
The follwing code is the timer interrupt that occurs when the timer register reaches a certain value.
```c
#pragma vector=TIMER0_A0_VECTOR
    __interrupt void Timer_A0(void)
    {
        TA0CTL = MC_0;                      // disable timer
        P1IE |= BIT3;                       // enable button interrupt
        P1IFG &= ~BIT3;                     // clear interrupt flag
        TA0R = 0;                           //reset timer
    }
```
When the interrupt occurs it stops the clock and resets the debounce timer. It also enables the button interrupt now that it has delayed it.
