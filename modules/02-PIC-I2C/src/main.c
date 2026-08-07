#pragma config FOSC  = HS
#pragma config WDTE  = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP   = OFF
#pragma config CPD   = OFF
#pragma config WRT   = OFF
#pragma config CP    = OFF

#include <xc.h>
#include <stdint.h>
#include "I2C.h"

#define MI_DIRECCION 0x30   

void __interrupt() isr(void) {
    ISR_I2C();
}

void main(void) {
    I2C_Init(MI_DIRECCION);
    TRISB=0;

    while(1) {
        // Monitorear de forma no bloqueante si llegó un mensaje completo por interrupción
        if (message_ready) {
            
            // Verificar si el Checksum y formato son correctos
            if (procesar_mensaje(MI_DIRECCION)) {
                // Si es válido, ejecutar la lógica asociada
                Ejecutar_cmd(rx_buffer[1], rx_buffer[2]);
            } else {
                // Manejo de error opcional (ej: encender un LED de error)
            }

            // --- CRUCIAL: Reiniciar variables para la siguiente trama ---
            rx_index = 0;
            message_ready = 0; 
        }
        
        // El microcontrolador puede realizar otras tareas concurrentes aquí
    }
}
