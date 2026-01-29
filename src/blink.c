#ifdef USE_MAIN_2 // TODO: define 'USE_MAIN_2' to compile this one 

#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>

#define TIMER1_OCR_FROM_SEC(sec) ((uint32_t)(((F_CPU) / 1024U) * (sec) - 1))
#define BAUD_TO_UBRR(baud) ((uint16_t)((F_CPU) / (16U * (baud))) - 1)

void init_timer(void);
uint8_t putchr(uint8_t chr);
void init(uint16_t baud_rate);
uint16_t println(const char *str);

volatile uint8_t led_state = 0;
volatile uint8_t tick = 0;

int main(void) {
    init(9600);
    init_timer();
    DDRB |= (1 << DDB5);

    while (1) {
        if (tick) {
            tick = 0;
            println(led_state ? "Led: on" : "Led: off");
        }
    }
}

void init_timer(void) {
    cli(); // disable global interrupts (ignores ISR)

    TCCR1A = 0; // reset Timer1 channel control (no PWM, no pin actions)
    TCCR1B = 0; // stop Timer1 and clear mode/prescaler settings
    TCNT1 = 0; // initial value of the timer counter

    OCR1A = TIMER1_OCR_FROM_SEC(0.5); // compare match value (top value in CTC mode) 31249 - 0.5s
    
    TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10);
    /*
        WGM12 = 1
        -> activates CTC (Clear Timer on Compare Match)
        -> TCNT1 counts 0 to OCR1A and goes back to 0

        CS12 = 1
        CS11 = 0
        CS10 = 1
        -> prescaler = 1024
        -> time clock = F_CPU / 1024
    */

    TIMSK1 |= (1 << OCIE1A);
    /*
        OCIE1A = Output Compare A Match Interrupt Enable
        -> enables TCNT1 == OCR1A to ask for interruptions
        -> sei() dependant
    */
    
    sei(); // enables global interrupts (ISR works now)
}

ISR(TIMER1_COMPA_vect) {
    led_state ^= 1;
    PORTB ^= (1 << PB5);  // blinks the led d13
    tick = 1;
}

void init(uint16_t baud_rate) {
    uint16_t ubrr = BAUD_TO_UBRR(baud_rate);

    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;

    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
}

uint8_t putchr(uint8_t chr) {
    while (!(UCSR0A & (1 << UDRE0)));

    UDR0 = chr;
    return 1U;
}

uint16_t println(const char *str) {
    uint16_t bytes = 0;
    while (*str && bytes != UINT16_MAX)
        putchr(*str++);

    bytes += putchr('\r');
    bytes += putchr('\n');

    return bytes;
}

#endif