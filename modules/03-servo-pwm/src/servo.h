// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef Servo_H
#define	PWM_H

#include <xc.h>
void Servo_Init(float Tmin, float Tmax);
void Servo_Set(int porcentaje);//Se debe ingresar no un número como tal sino el porcentaje de lo que vale 
void ISR_Servo();
void Servo_Ang(int angulo);//sedebe ingresar un numero entero desde 0 a 250

#endif	/* Servo_H */

