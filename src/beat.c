#ifdef USE_MAIN_3 // TODO: define 'USE_MAIN_3' to compile this one 

#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>

#define get_ubrr(baud) (uint16_t)(((F_CPU) / (16ULL * (baud))) - 1)
#define get_time_from_ms(ms) (uint32_t)(((F_CPU / 1024UL) * (ms)) / 1000UL - 1)

uint8_t putchr(uint8_t chr);
void init_timer(uint16_t ms);
void begin(uint16_t baud_rate);
uint16_t print(const char *str);
uint8_t print_int16(int16_t num);

volatile uint16_t tick = 0;

int main(void) {
    begin(9600);
    init_timer(10);
    DDRB |= (1 << DDB5);

    const uint16_t beats_ms[] = {100, 100, 100, 700};
    uint8_t size = sizeof(beats_ms) / sizeof(*beats_ms);

    uint8_t i = 0;
    while (1) {
        if (tick >= beats_ms[i]) {
            tick = 0;
            if (!(i & 1))
                PORTB |=  (1 << PB5);
            else
                PORTB &= ~(1 << PB5);

            print("Beat duration: ");
            print_int16(beats_ms[i]);
            print("\r\n\n");

            i++;

            if (i >= size)
                i = 0;
        }
    }

}

void init_timer(uint16_t ms) {
    cli();

    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    OCR1A = get_time_from_ms(ms);

    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);

    TIMSK1 |= (1 << OCIE1A);

    sei();
}

ISR(TIMER1_COMPA_vect) {
    tick += 10;
}

void begin(uint16_t baud_rate) {
    uint16_t ubrr = get_ubrr(baud_rate);

    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;

    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
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