#ifndef I2C_H
#define I2C_H

#include <xc.h>
#include <stdint.h>

extern volatile unsigned char rx_buffer[5];
extern volatile unsigned char rx_index;
extern volatile unsigned char message_ready;
void I2C_Init(unsigned char addr);
unsigned char procesar_mensaje(unsigned char addr_slave);
void ISR_I2C(void);
void Ejecutar_cmd(unsigned char cmd, unsigned char dato);

#endif /* I2C_H */
