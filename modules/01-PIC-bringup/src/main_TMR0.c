/*
 * File:   main.c
 * Author: Usuario
 *
 * Created on 5 de junio de 2026, 16:13
 */





// PIC16F877A Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#define _XTAL_FREQ 20000000

#include <xc.h>

volatile int contador=0;
volatile char cambio=0; //a diferencia de otros PIC este carece de un registro latch
void __interrupt() ISR(void){
	if (TMR0IF){//Se activa solo cuando 
        contador++;
        if(contador>=625){
            cambio= !cambio;
            contador=0;
        }
        RB0=cambio;
        TMR0IF=0;
        TMR0=6;
    }
}

void main(void) {
    OPTION_REG = 0b10000100; // Prescaler 32
    INTCON     = 0b10100000; //Enables
    TMR0=6; //valor inicial del TMR0
    TRISB=0;
    PORTB=0;
    RB0= cambio;
    while(1){
    }
    
}
