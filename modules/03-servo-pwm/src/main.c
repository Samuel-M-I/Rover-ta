/*
 * File:   main.c
 * Author: Samuel Marroquín Isaza
 *
 * Created on May 18, 2026, 8:16 PM
 */
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
#include "Servo.h"

void __interrupt() ISR(void) {
    ISR_Servo();
}

void main(void) {
    Servo_Init(0.47e-3,2.5e-3);
    TRISB=0xFF;
    while(1){
        if((RB0==0)&&(RB1==0)&&(RB2==0)){
            Servo_Ang(0);
        }else if((RB0==1)&&(RB1==0)&&(RB2==0)){
            Servo_Ang(45);
        }else if((RB0==0)&&(RB1==1)&&(RB2==0)){
            Servo_Ang(90);
        }else if((RB0==1)&&(RB1==1)&&(RB2==0)){
            Servo_Ang(180);
        }else if((RB0==0)&&(RB1==0)&&(RB2==1)){
            Servo_Ang(250);
        }
    }
}
