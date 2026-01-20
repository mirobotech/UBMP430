/* =============================================================================
 * File: UBMP430.c
 * Date: January 20, 2026
 * 
 * UBMP4.3 (PIC16F1459) hardware functions
 * ===========================================================================*/

#include "xc.h"                 // XC compiler general include file

#include "stdint.h"             // Include integer definitions
#include "stdbool.h"            // Include Boolean (true/false) definitions

#include "UBMP430.h"

/* =========================================================
 * Millisecond timer (Timer0) interrupt function
 * ========================================================= */

volatile uint32_t millis_count = 0;

/*
 * Timer0 setup: Fosc = 48 MHz
 * Instruction clock = Fosc / 4 = 12 MHz
 * Timer0 pre-scaler = 1:64
 *
 * Timer tick = 64 / 12 MHz = 5.333 us
 * 1 ms / 5.333us = 187.5 ticks
 * 256 - 188 = 68 = pre-load value (won't be precisely 1ms, but close)
 */

void __interrupt() ISR(void) {
    if (TMR0IF) {               // If Timer 0 overflowed:
        TMR0IF = 0;             // Clear Timer 0 interrupt flag
        TMR0 = 68;              // Pre-load Timer 0 for next ms period
        millis_count++;         // Increment milliseconds count
    }
}

/* =========================================================
 * UBMP4 initialization function
 * ========================================================= */

void UBMP4_Init(void) {

    /* Oscillator configuration */
    OSCCON = 0xFC;              // Set 16MHz HFINTOSC with 3x PLL enabled
    ACTCON = 0x90;              // Enable active clock tuning from USB clock
    while(!PLLRDY);             // Wait for PLL lock (disable for simulation)

    /* OPTION register configuration */
    OPTION_REG = 0b01010101;    // Enable port pull-ups, TMR0 div-64 internal

    /* PORTA configuration */
    LATA = 0b00000000;          // Clear Port A latches before configuring PORTA
    TRISA = 0b00001111;         // Set LED_D1 (RUN) and Beeper pins as outputs
    ANSELA = 0b00000000;        // Disable analog input on all PORT A input pins
    WPUA = 0b00001000;          // Enable weak pull-up on SW1 (RESET) input only

    /* PORTB configuration */
    LATB = 0b00000000;          // Clear Port B latches before configuring PORTB
    TRISB = 0b11110000;         // Enable pushbutton pins as inputs (SW2-SW5)
    ANSELB = 0b00000000;        // Disable analog input on all PORT B input pins
    WPUB = 0b11110000;          // Enable weak pull-ups on all pushbutton inputs

    /* PORTC configuration */
    LATC = 0b00000000;          // Clear Port C latches before configuring PORTC
    TRISC = 0b00001111;         // Set LED pins as outputs, H1-H4 pins as inputs
    ANSELC = 0b00000000;        // Disable analog input on all PORT C input pins

    /* Timer0 configuration and startup (used for milliseconds counter) */
    TMR0 = 68;                  // Pre-load Timer0 with millis() offset
    TMR0IF = 0;                 // Ensure Timer0 Interrupt Flag is cleared
    TMR0IE = 1;                 // Enable Timer0 interrupt
    GIE = 1;                    // Enable global interrupts
    
}

/* =========================================================
 * Timing functions
 * ========================================================= */

void delay_us(uint16_t us) {
    if (us <= 1) return;
    us--;
    while (us--) _delay(1);
}

void delay_ms(uint16_t ms) {
    while (ms--) delay_us(992);
}

uint32_t millis(void) {
    return millis_count;
}

/* =========================================================
 * ADC functions
 * ========================================================= */

void ADC_Init(void) {
    // Configure ADC input for on-board phototransistor Q1 (AN7).
    // TODO: Disable digital output driver for any other required ADC input pins
    TRISCbits.TRISC3 = 1;       // Disable output driver (TRISx.bit = 1)
    // TODO: Enable analog input for any other required ADC input pins
    ANSELC = 0b00001000;        // Enable Q1 analog input (ANSELx.bit = 1)
    
    // General ADC setup and configuration
    ADCON0 = 0b00011100;        // Set channel to AN7, leave A/D converter off
    ADCON1 = 0b11100000;        // Right justified result, FOSC/64 clock, +VDD ref
    ADCON2 = 0b00000000;        // Auto-conversion trigger disabled
}

void ADC_Set_Channel(uint8_t channel) {
    ADON = 1;                   // Turn the A-D converter on
    ADCON0 &= 0b10000011;       // AND to clear channel select (CHS) bits
    ADCON0 |= channel;          // OR channel constant to select input channel
}

uint16_t ADC_Read(void) {
    ADCON1bits.ADFM = 1;        // Set right justified result
    GO = 1;                     // Start the conversion by setting Go/^Done bit
	while(GO)                   // Wait for the conversion to finish (GO==0)
        ;                       // (Suppresses warning from empty while loop)
    return ((uint16_t)ADRESH << 8) | ADRESL;    // Return 16-bit result
}

uint8_t ADC_Read_8bits(void) {
    ADCON1bits.ADFM = 0;        // Set left justified result
    GO = 1;                     // Start the conversion by setting Go/^Done bit
	while(GO)                   // Wait for the conversion to finish (GO==0)
        ;                       // (Suppresses warning from empty while loop)
    return (ADRESH);            // Return top 8-bits of result    
}

uint16_t ADC_Read_Channel(uint8_t channel) {
    ADCON1bits.ADFM = 1;        // Set right justified result
    ADON = 1;                   // Turn the A-D converter on
    ADCON0 &= 0b10000011;       // AND to clear channel select (CHS) bits
    ADCON0 |= channel;          // OR channel constant to select input channel
    __delay_us(5);              // Allow input to settle after channel change
    GO = 1;                     // Start the conversion by setting Go/^Done bit
	while(GO)                   // Wait for the conversion to finish (GO==0)
        ;                       // (Suppresses warning from empty while loop)
    ADON = 0;                   // Turn the A-D converter off
    return ((uint16_t)ADRESH << 8) | ADRESL;    // Return 16-bit result
}

/* =========================================================
 * PWM functions (designed to create 38kHz output on LED6)
 * ========================================================= */

void PWM1_Start(uint16_t freq) {
    uint16_t pr;

    if (freq == 0) return;

    T2CON = 0b00000101;         // Turn Timer2 on, pre-scaler 1:4, post-scaler 1:1
    
    /*
     * PR2 = (Fosc / (4 * pre-scaler * freq)) - 1
     * Pre-scaler = 64
     */
    pr = (_XTAL_FREQ / (4UL * 4UL * freq)) - 1;
    if (pr > 255) pr = 255;
    
    PR2 = pr;
    PWM1DCH = (pr + 1) >> 1;   // ~50% duty
    PWM1DCL = 0;
    PWM1CON = 0b11000000;      // Enable PWM1 output
}

void PWM1_On(void) {
    PWM1CON = 0b11000000;       // Enable PWM1 output
}

void PWM1_Off(void) {
    PWM1CON = 0b10000000;       // Disable PWM1 output
}

void PWM1_Stop(void) {
    PWM2CON = 0;                // Disable PWM1
    T2CON = 0;                  // Turn Timer2 Off
}

/* =========================================================
 * Sound function
 * ========================================================= */

void play_Freq(uint16_t freq, uint32_t ms) {
    uint16_t period2 = (500000UL / freq) - 8;   // Calculate 1/2 period
    uint32_t last = millis();
    while ((millis()) - last < ms) {    // Repeat tone for duration
        BEEPER = !BEEPER;
        delay_us(period2);
    }
    BEEPER = 0;                 // Clear beeper pin after sound finishes
}

/* =========================================================
 * Map function
 * ========================================================= */

int16_t map(int16_t value, int16_t min, int16_t max, int16_t newMin, int16_t newMax) {
    int16_t oldRange = max - min;
    int16_t newRange = newMax - newMin;
    int16_t newValue = newValue = ((value - min) * newRange / oldRange) + newMin;
    return(newValue);
}
