#ifdef USE_MAIN_1 // TODO: define 'USE_MAIN_1' to compile this one

#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>

#define get_ubrr(baud) (uint16_t)(((F_CPU) / (16ULL * (baud))) - 1)

uint8_t putchr(uint8_t chr);
void begin(int16_t baud_rate);
uint16_t print(const char *str);
uint8_t print_int16(int16_t num);

int main(void) {
    begin(9600);

    int16_t sec = 0;
    int16_t mins = 0;
    while (1 && mins != INT16_MAX) {
        print("Counter: ");

        print_int16(mins);
        putchr(':');
        print_int16(sec);

        print("\r\n\n");
        _delay_ms(1000);
        sec++;

        if (sec == 60) {
            sec = 0;
            mins++;
        }
    }

}

uint8_t print_int16(int16_t num) {
    char buf[7];
    int8_t i = 0;

    if (num < 0) {
        buf[0] = '-';
        i++;
    }

    do {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    } while (num);

    while (i--)
        putchr(buf[i]);

    return i;
}

void begin(int16_t baud_rate) {
    uint16_t ubrr = get_ubrr(baud_rate);

    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;

    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
}

uint8_t putchr(uint8_t chr) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = chr;
    return sizeof(uint8_t);
}

uint16_t print(const char *str) {
    uint16_t bytes = 0;

    while (*str && bytes != UINT16_MAX)
        bytes += putchr(*str++);

    return bytes;
}
#endif