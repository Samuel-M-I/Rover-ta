#include <xc.h>
#include "Servo.h"
volatile float b=0;
volatile float m=0;

void ISR_Servo(){        //Es necesario poner esta función en el en el bloque de interrupciones 
    if (TMR1IF) {
        TMR1IF = 0;
        TMR1 = 15536;   // Recarga para periodo de 20ms
        RC2 = 1;        // Inicia el pulso
    }
    if (CCP1IF) {
        CCP1IF = 0;
        RC2 = 0;        // Termina el pulso
    }
}

void Servo_Set(int porcentaje){ //    acepta valores de 0 a 100 que seran el porcentaje de audio
    // Limitar el rango
    if (porcentaje < 0) porcentaje = 0;
    if (porcentaje > 100) porcentaje = 100;
    
    CCPR1 = (int)(m * porcentaje + b);  // Cast explícito
}
void Servo_Ang(int angulo){
    if (angulo < 0) angulo = 0;
    if (angulo > 250) angulo = 250;
    
    Servo_Set((angulo * 100) / 250);
    
}

void Servo_Init(float Tmin, float Tmax) {
    TRISC2  = 0;              // RC2 como salida
    RC2     = 0;

    // Fosc=20MHz, prescaler 1:2 ? 0.4µs/tick
    T1CON   = 0b00010001;       // TMR1ON, prescaler 1:2
    CCP1CON = 0b00001010;       // Compare, software interrupción
    TMR1    = 15536;            //valor inicial del TMR1    
    TMR1IF  = 0;
    CCP1IF  = 0;
    PIE1    = 0b00000101;     // TMR1IE + CCP1IE 
    INTCON  = 0b11000000;     // GIE + PEIE
    b=Tmin/(0.4e-6) + 15536;   // Hallo el valor minimo del servo
    m=((Tmax)/(0.4e-6)+ 15536 - b)/100; //Hallo la pendiente  
    Servo_Set(0);
}
