#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>

#define baud_rate_reg(baud_rate) (uint16_t)(((F_CPU) / (16UL * (baud_rate))) - 1)

uint8_t uart_tx(char c);
void uart_init(uint16_t n);
uint16_t uart_println(const char *s);

int main(void) {
    uart_init(9600);

    while (1) {
        uart_println("Hello, World!");
        _delay_ms(1000);
    }
}

void uart_init(uint16_t n) {
    uint16_t ubrr = baud_rate_reg(n);
    
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;

    UCSR0B = (1 << TXEN0);       // enables tx (transmission)
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);     // prepares the micro-controller to receive 8 bits each time 
}

uint8_t uart_tx(char c) {
    while (!(UCSR0A & (1 << UDRE0))); // checks if the 5-th bit of UCSR0A is occupied
    UDR0 = c;
    return sizeof(c);
}

uint16_t uart_println(const char *s) {
    uint16_t bytes = 0;
    while (*s) {
        bytes += uart_tx(*s++);
    }
    uart_tx('\r');
    uart_tx('\n');

    return bytes + 2;
}
