#include <xc.h>

// Variables globales de la librería (Encapsuladas para el manejo de la trama)
volatile unsigned char rx_buffer[5]; 
volatile unsigned char rx_index = 0;
volatile unsigned char message_ready = 0;

const unsigned char crc8_table[256] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};


void I2C_Init(unsigned char addr) {
    // 1. Configurar pines SDA (RC4) y SCL (RC3) como entradas
    TRISC3 = 1; 
    TRISC4 = 1;

    // 2. Configurar la dirección del esclavo (Bits 7-1, el bit 0 es indiferente en modo 7 bits)
    SSPADD = addr << 1; 

    // 3. SSPSTAT: Modo Standard/Fast Speed y limpiar estados
    // Bit 7 (SMP): 0 para modo 400kHz (o 1 si es 100kHz, para 400kHz se recomienda 0)
    // Bit 6 (CKE): 0 para cumplir especificaciones de niveles I2C
    SSPSTAT = 0x00;

    // 4. SSPCON: 
    // Bit 7 (WCOL): 0 (Sin colisión)
    // Bit 6 (SSPOV): 0 (Sin desbordamiento)
    // Bit 5 (SSPEN): 1 (Habilitar el puerto síncrono MSSP)
    // Bit 4 (CKP): 1 (Liberar el reloj SCL)
    // Bits 3-0 (SSPM): 0110 -> Modo Esclavo I2C, 7 bits de dirección con interrupción en START/STOP deshabilitada
    SSPCON = 0x36;

    // 5. Configurar Interrupciones
    SSPIF = 0;  // Limpiar bandera de interrupción del MSSP
    SSPIE = 1;  // Habilitar interrupción del MSSP
    PEIE  = 1;  // Habilitar interrupciones periféricas
    GIE   = 1;  // Habilitar interrupciones globales
    
    // Reset de variables de control
    rx_index = 0;
    message_ready = 0;
}



// Retorna 1 si el mensaje es válido, 0 si falló el chequeo
unsigned char procesar_mensaje(unsigned char addr_slave) {
    unsigned char cmd = rx_buffer[1];
    unsigned char data = rx_buffer[2];
    unsigned char checksum_recibido = rx_buffer[3];
    unsigned char fin_linea = rx_buffer[4];
    
    // 1. Validar el delimitador final
    if (fin_linea != '\n') {
        return 0; 
    }

    // 2. Calcular Checksum local (Misma ecuación matemática del Maestro)
    unsigned char crc = 0x00;
    crc = crc8_table[crc ^ addr_slave];
    crc = crc8_table[crc ^ cmd];
    crc = crc8_table[crc ^ data];
    // 3. Comparar
    if (crc == checksum_recibido) {
        return 1; // Trama íntegra
    }

    return 0; // Error de Checksum
}

void ISR_I2C(void) {
    if (SSPIF) {
        unsigned char dummy;
        // CAMBIO AQUÍ: Cambiar máscara de 0x2D a 0x25 para ignorar el bit START (S)
        unsigned char status = SSPSTAT & 0x25; 

        // Estado 1: Recibió Dirección de Escritura (DA = 0, RW = 0, BF = 1)
        if ((status ^ 0x01) == 0) {
            dummy = SSPBUF; 
            rx_index = 0;   
            SSPCONbits.CKP = 1; // Libera el reloj SCL
        }
        // Estado 2: Recibió Dato (DA = 1, RW = 0, BF = 1)
        else if ((status ^ 0x21) == 0) {
            unsigned char received_byte = SSPBUF; 

            if (!message_ready) { 
                if (rx_index == 0 && received_byte != ':') {
                    SSPCONbits.CKP = 1;
                    SSPIF = 0;
                    return;
                }

                rx_buffer[rx_index] = received_byte;
                rx_index++;

                if (rx_index >= 5) {
                    message_ready = 1; 
                }
            }
            
            SSPCONbits.CKP = 1; // Libera el reloj para el siguiente byte
        }
        else {
            // POR SEGURIDAD: Si entra un estado no mapeado, liberar el reloj para no colgar el bus
            SSPCONbits.CKP = 1; 
        }
        
        SSPIF = 0; 
    }
}

void Ejecutar_cmd(unsigned char cmd, unsigned char dato) {
    switch(cmd) {
        case 0x01:
            RB0=dato;
            break;
            
        case 0x02:
            // Ejemplo: Modificar el duty cycle de un PWM
            // CCPR1L = dato;
            break;
            
        default:
            // Comando desconocido
            break;
    }
}
