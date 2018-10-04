#include <msp430.h> 


/**
 * @author Nick Setaro
 * @version 10.4.2018
 *
 * This code uses a timer to create a pwm signal. This signal drives the LED and controls its brightness.
 * When the button is pressed the duty cycle of the pwm is increased by 10% until it reaches 100%.
 * If the button is pressed at 100% duty cycle the duty cycle is reset to 0%.
 */
int main(void)
{
    //gpio
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	P1DIR |= BIT0;              //set pin 1.0 direction to output
	P1DIR |= BIT6;              //set pin 1.6 direction to output
	P1SEL |= (BIT6);              //sets port 1 for output

	//timer A0 for PWM
	TA0CTL = TASSEL_2 + MC_1 + ID_1;    //set Timer A to up mode
	TA0CCR0 = 1000;                         //set period
	TA0CCR1 = 100;                         //set duty cycle to 10%
	TA0CCTL1 = OUTMOD_7;                   //set timer to set/reset mode

	//button
	P1DIR &= ~BIT3;                      //set pin 1.3 direction to input
	P1REN |= BIT3;                       //enable resistor on pin 1.3
	P1OUT |= BIT3;                       //set resistor to pull up
	P1IE |= BIT3;                       //enable interrupt on button
	P1IES |= BIT3;                      //set interrupt edge
	P1IFG &= ~BIT3;                     //clear interrupt flags

	__bis_SR_register(GIE);              // LPM0 with general interrupts enabled
	while(1);
}

//button interrupt
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    if(P1IES & BIT3){
            if (TA0CCR1<1000) {             //if duty cycle did not overflow
                TA0CCR1+=100;               //increment duty cycle by 10%
            } else if (TA0CCR1 >=1000) {    //if the duty cycle overflows
                TA0CCR1 = 0;                //reset duty cycle
            }
    }
           P1IES ^= BIT3;                   //toggle interrupt edge
           P1OUT ^= BIT0;                   //toggle LED on pin 1.0
           P1IFG &= ~BIT3;                  //clear flags on button interrupt
}




