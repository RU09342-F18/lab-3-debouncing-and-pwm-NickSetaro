# Software Debouncing
Buttons are mechanical devices that are prone to mechanical errors. When the button is pressed it can sometimes "bounce" between on and off states before resting on off. These rapid switches can be registered and one button press can be registered multiple times. This code uses a timer to disable the button for a short period of time until the bounce period is over. 

# Timer Interrupt
The follwing code is the timer interrupt that occurs when the timer register reaches a certain value.
'''c
#pragma vector=TIMER0_A0_VECTOR
    __interrupt void Timer_A0(void)
    {
        TA0CTL = MC_0;                      // disable timer
        P1IE |= BIT3;                       // enable button interrupt
        P1IFG &= ~BIT3;                     // clear interrupt flag
    }
'''
When the interrupt occurs 
