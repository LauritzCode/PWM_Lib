#include "timer_helper.h"
#include <stdbool.h>
#include <avr/interrupt.h>

const uint32_t timer_prescaler[] = {
    1,
    8,
    64,
    256,
    1024
};

uint8_t cs_bits[] = {
    (1 << 0), // prescaler 1 
    (1 << 1), // prescaler 8
    (1 << 1) | (1 << 0), // prescaler 64
    (1 << 2), // prescaler 256
    (1 << 2) | (1 << 0) // prescaler 1024
};

static bool find_prescaler(uint32_t freq, uint8_t *cs_bits_out, uint32_t *ocr_out) {

    for(int i = 0; i < sizeof(timer_prescaler) / sizeof(timer_prescaler[0]); i++) {
        if((CPU_freq / (freq * timer_prescaler[i])) - 1 < 65535) {
            *cs_bits_out = cs_bits[i];
            *ocr_out = (CPU_freq / (freq * timer_prescaler[i])) - 1;
            return 1;
        }
    } 

    return 0;
}

void initialize_timer(TimerID timer, uint32_t freq) {

    uint8_t cs_bits;
    uint32_t ocr;

    if(!find_prescaler(freq, &cs_bits, &ocr)) return; 
    cli();

    switch(timer) {
        case Timer0:
        if(ocr > 255) return;       
        TCCR0B = cs_bits;
        TCCR0A = 0;
        TCCR0B |= (1 << WGM02);
        TCCR0A |= (1 << WGM01);
        OCR0A = (uint8_t)ocr;
        TCNT0 = 0;
        TIMSK0 |= (1 << OCIE0A);
        break;
        case Timer1: 
        TCCR1B = cs_bits;
        TCCR1A = 0;
        TIMSK1 |= (1 << OCIE1A);
        TCCR1B |= (1 << WGM12);
        OCR1A = ocr;
        TCNT1 = 0;
        break;
        case Timer3: 
        TCCR3B = cs_bits;
        TCCR3A = 0;
        TIMSK3 |= (1 << OCIE3A);
        TCCR3B |= (1 << WGM32);
        OCR3A = ocr;
        TCNT3 = 0;
        break;
        case Timer4: 
        TCCR4B = cs_bits;
        TCCR4A = 0;
        TIMSK4 |= (1 << OCIE4A);
        TCCR4B |= (1 << WGM42);
        OCR4A = ocr;
        TCNT4 = 0;
        break;
        case Timer5: 
        TCCR5B = cs_bits;
        TCCR5A = 0;
        TIMSK5 |= (1 << OCIE5A);
        TCCR5B |= (1 << WGM52);
        OCR5A = ocr;
        TCNT5 = 0;
        break;
        
        default: 
        break;
    }

    sei();
}

void start_timer(TimerID timer) {

    switch(timer) {
        case Timer1: 
        TIMSK1 |= (1 << OCIE1A);
        break;
        case Timer3: 
        TIMSK3 |= (1 << OCIE3A);
        break;
        case Timer4: 
        TIMSK4 |= (1 << OCIE4A);
        break;
        case Timer5: 
        TIMSK5 |= (1 << OCIE5A);
        break;
        default:
        break;
    }

}

void stop_timer(TimerID timer) {
    switch(timer) {
        case Timer1: 
        TIMSK1 &= ~(1 << OCIE1A);
        break;
        case Timer3: 
        TIMSK3 &= ~(1 << OCIE3A);
        break;
        case Timer4: 
        TIMSK4 &= ~(1 << OCIE4A);
        break;
        case Timer5: 
        TIMSK5 &= ~(1 << OCIE5A);
        break;
        default:
        break;
 }
}

void reset_timer(TimerID timer) {
    switch(timer) {
        case Timer1: 
        TCNT1 = 0;
        break;
        case Timer3: 
        TCNT3 = 0;
        break;
        case Timer4: 
        TCNT4 = 0;
        break;
        case Timer5: 
        TCNT5 = 0;
        break;
        default: 
        break;
 }
}