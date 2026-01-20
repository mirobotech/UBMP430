/*==============================================================================
 * Project: UBMP430test.c
 * Version: 4.3                 Date: January 20, 2026
 * 
 * Simple UBMP4.3 test program.
 * ===========================================================================*/

#include "xc.h"                 // Microchip XC8 compiler include file
#include "stdint.h"             // Include integer definitions
#include "stdbool.h"            // Include Boolean (true/false) definitions

#include "UBMP430.h"            // Include UBMP4 constant symbols and functions

// The following settings must be applied when using the USB uC bootloader:
// TODO: Set linker ROM ranges to 'default,-0-7ff' under "Memory model" pull-down.
// TODO: Set linker Code offset to '0x800' under "Additional options" pull-down.

unsigned int rawLight;          // Raw light level from ambient light sensor
unsigned char pwmVal;           // LED2 PWM value

void pwm_LED2(unsigned char pwmOn)
{
    for(unsigned char i = 255; i != 0; i--)
    {
        if(i == pwmOn)
        {
            LED2 = 1;
        }
        __delay_us(10);
    }
    LED2 = 0;
}

/*==============================================================================
    Main program. The main() function is called first by the compiler.
==============================================================================*/

int main(void)                  // Run this code on every startup
{
    UBMP4_Init();               // Configure UBMP4 and on-board I/O devices
    
    ADC_Init();                 // Configure ADC for on-board ambient light sensor
        
    while(1)                    // Code in this while loop repeats forever
	{
        if (SW2 == 0)
        {
            // Blink lights (blocking)
            LED2 = 1;
            delay_ms(100);
            LED3 = 1;
            delay_ms(100);
            LED4 = 1;
            delay_ms(100);
            LED5 = 1;
            delay_ms(100);
            LED2 = 0;
            delay_ms(100);
            LED3 = 0;
            delay_ms(100);
            LED4 = 0;
            delay_ms(100);
            LED5 = 0;
            delay_ms(100);
        }
        
        if (SW3 == 0)
        {
            // Display analog level on LED2
            rawLight = ADC_Read_Channel(AQ1);
            pwmVal = map(rawLight, 0, 1023, 0, 255);
            pwm_LED2(pwmVal);            
        }
        
        if (SW4 == 0)
        {
            play_Freq(880, 500);
        }
        
        if (SW5 == 0)
        {
            play_Freq(1046, 500);
        }
        
        if(SW1 == 0)            // Enter the bootloader if S1 is pressed.
        {
            RESET();
        }
    }
}
