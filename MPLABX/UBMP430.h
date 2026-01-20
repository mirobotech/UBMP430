/* =============================================================================
 * File: UBMP430.h
 * Date: January 20, 2026
 * 
 * UBMP4.3 hardware definitions and function prototypes (PIC16F1459 in USB mode)
 * ===========================================================================*/

#ifndef UBMP430_H
#define UBMP430_H

#define _XTAL_FREQ  48000000UL      // Set clock frequency for timing functions

/* =====================================
 * UBMP4 hardware definitions
 * ===================================== */

// PORTA
#define MCLR        PORTAbits.RA3   // SW1/PROG/Reset (MCLR) pushbutton input
#define SW1         PORTAbits.RA3   // SW1/PROG/Reset (MCLR) pushbutton input
#define BEEPER      LATAbits.LATA4  // Piezo beeper (LS1) output
#define	LS1         LATAbits.LATA4  // Piezo beeper (LS1) output
#define LED1        LATAbits.LATA5  // LED D1/Run LED output (active-low)

// PORTB I/O
#define SW2         PORTBbits.RB4   // Pushbutton SW2 input
#define SW3         PORTBbits.RB5   // Pushbutton SW3 input
#define SW4         PORTBbits.RB6   // Pushbutton SW4 input
#define SW5         PORTBbits.RB7   // Pushbutton SW5 input

// PORTC
#define H1IN        PORTCbits.RC0   // External I/O header H1 input
#define H1OUT       LATCbits.LATC0  // External I/O header H1 output

#define H2IN        PORTCbits.RC1   // External I/O header H2 input
#define H2OUT       LATCbits.LATC1  // External I/O header H2 output

#define H3IN        PORTCbits.RC2   // External I/O header H3 input
#define H3OUT       LATCbits.LATC2  // External I/O header H3 output 
#define U2          PORTCbits.RC2   // IR demodulator (U2) input

#define H4IN        PORTCbits.RC3   // External I/O header H4 input
#define H4OUT       LATCbits.LATC3  // External I/O header H4 output
#define Q1          PORTCbits.RC3   // Phototransistor/ambient light sensor (Q1) input

#define H5IN        PORTCbits.RC4   // External I/O header H5 input
#define H5OUT       LATCbits.LATC4  // External I/O header H5 output
#define LED2        LATCbits.LATC4  // LED D2 output

#define H6IN        PORTCbits.RC5   // External I/O header H6 input
#define H6OUT       LATCbits.LATC5  // External I/O header H6 output
#define LED3        LATCbits.LATC5  // LED D3 output
#define LED6        LATCbits.LATC5  // LED D6/IR LED output

#define H7IN        PORTCbits.RC6   // External I/O header H7 input
#define H7OUT       LATCbits.LATC6  // External I/O header H7 output
#define LED4        LATCbits.LATC6  // LED D4 output

#define H8IN        PORTCbits.RC7   // External I/O header H8 input
#define H8OUT       LATCbits.LATC7  // External I/O header H8 output
#define LED5        LATCbits.LATC7  // LED D5 output

/* =====================================
 * ADC channel definitions
 * ===================================== */

#define A3          0b00001100      // A-D converter channel 3 input (LS1)
#define AH1         0b00010000      // External H1 header analogue input (Ch4))
#define A4          0b00010000      // A-D converter channel 4 input
#define AH2         0b00010100      // External H2 header analogue input (Ch5)
#define A5          0b00010100      // A-D converter channel 5 input
#define AH3         0b00011000      // External H3 header analogue input (Ch6)
#define A6          0b00011000      // A-D converter channel 6 input
#define AH4         0b00011100      // External H4 header analogue input (Ch7)
#define A7          0b00011100      // A-D converter channel 7 input
#define AQ1         0b00011100      // Q1 phototransistor A-D input (Ch7)
#define AH7         0b00100000      // External H7 header analogue input (Ch8)
#define A8          0b00100000      // A-D converter channel 8 input
#define AH8         0b00100100      // External H8 header analogue input (Ch9)
#define A9          0b00100100      // A-D converter channel 9 input
#define A10         0b00101000      // A-D converter channel 10 input (SW2 - not used)
#define A11         0b00101100      // A-D converter channel 11 input (SW3 - not used)
#define ATIM        0b01110100      // On-die temperature indicator module input
#define ADAC        0b01111000      // Internal DAC (digital-to-analog converter)
#define AFVR        0b01111100      // Internal fixed voltage reference buffer

/* =====================================
 * Core UBMP4/PIC16F1459 initialization
 * ===================================== */

void UBMP4_Init(void);


/* =====================================
 * Timing functions
 * ===================================== */

void delay_ms(uint16_t ms);
/* Delays for specified number of milliseconds.
 * parameter(s): ms (unsigned int) - number of milliseconds to delay (blocking)
 * returns: nothing
 */

void delay_us(uint16_t us);
/* Delays for specified number of microseconds.
 * parameter(s): us (unsigned int) - number of microseconds to delay (blocking)
 * returns: nothing
 */

uint32_t millis(void);
/* Interrupt-drive milliseconds counter.
 * parameter(s): none
 * returns: milliseconds (unsigned long) since microcontroller start-up
 */


/* =====================================
 * ADC functions
 * ===================================== */

void ADC_Init(void);
/* Configures ADC converter and GPIO to read from on-board phototransistor Q1 (AN7).
 * parameter(s): none
 * returns: nothing
 */

void ADC_Set_Channel(uint8_t channel);
/* Sets ADC input mux to the selected channel.
 * parameter(s): channel constant (unsigned char) encoded with channel select (CHS) bits
 * returns: nothing
 */
uint16_t ADC_Read(void);
/* Reads 10-bit result from currently selected ADC input channel.
 * parameter(s): nothing
 * returns: digital value equivalent to analog input level (10-bits, unsigned int)
 */

uint8_t ADC_Read_8bits(void);
/* Reads 8-bit result from currently selected ADC input channel.
 * parameter(s): nothing
 * returns: digital value equivalent to analog input level (8-bits, unsigned char)
 */

uint16_t ADC_Read_Channel(uint8_t channel);
/* Sets ADC input mux to selected ADC imput channel and reads the 10-bit result.
 * parameter(s): channel constant (unsigned char) encoded with channel select (CHS) bits
 * returns: digital value equivalent to analog input level (10-bits, unsigned int)
 */


/* =====================================
 * PWM functions
 * ===================================== */

void PWM1_Start(uint16_t freq);
/* Starts 50% duty cycle PWM1 output using Timer 2 hardware module (designed for
 * producing 38kHz output for IR transmission).
 * parameter(s): freq (unsigned int) - PWM frequency in Hz (minimum 11765)
 * returns: nothing
 */

void PWM1_On(void);
/* Restarts PWM1 output.
 * parameter(s): none
 * returns: nothing
 */

void PWM1_Off(void);
/* Stops PWM1 output.
 * parameter(s): none
 * returns: nothing
 */

void PWM1_Stop(void);
/* Stops PWM1 output and disables Timer 2.
 * parameter(s): none
 * returns: nothing
 */


/* =====================================
 * Sound function
 * ===================================== */

void play_Freq(uint16_t freq, uint32_t ms);
/* Plays a tone on the piezo buzzer for the requested amount of time.
 * parameter(s): freq (unsigned int) - tone frequency in Hertz
 *               ms (unsigned int) - tone duration in milliseconds (blocking)
 * returns: nothing
 */


/* =====================================
 * Map function
 * ===================================== */

int16_t map(int16_t value, int16_t min, int16_t max, int16_t newMin, int16_t newMax);
/* Maps a numeric value from an intial input range into a new output range.
 * parameter(s): value (int) - initial value
 *               min (int) - input range minimum value
 *               max (int) - input range maximum value
 *               newMin (int) - output range minimum value
 *               newMax (int) - output range maximum value
 * returns: output result (int)
 */

#endif
