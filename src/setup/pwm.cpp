#include "pwm.h"
#include <avr/interrupt.h>
#include <stdbool.h>
#include "macros.h"

static ServoType servo_types[4]; // index 0 = Timer1, 1 = Timer3, 2 = Timer4, 3 = Timer5
static const uint32_t timer_prescaler[] = {1, 8, 64, 256, 1024};
static const uint8_t cs_bits[] = {
    (1 << CS10),                    // prescaler 1
    (1 << CS11),                    // prescaler 8
    (1 << CS11) | (1 << CS10),     // prescaler 64
    (1 << CS12),                    // prescaler 256
    (1 << CS12) | (1 << CS10)      // prescaler 1024
};

static bool find_prescaler_pwm(uint32_t freq, PWM_Mode mode, uint8_t *cs_out, uint16_t *top_out) {
    for(int i = 0; i < sizeof(timer_prescaler) / sizeof(timer_prescaler[0]); i++) {
        uint32_t top;
        if(mode == PWM_FAST)
            top = CPU_freq / (freq * timer_prescaler[i]);
        else
            top = CPU_freq / (2 * freq * timer_prescaler[i]);
        if(top < 65535) {
            *cs_out = cs_bits[i];
            *top_out = (uint16_t)top;
            return true;
        }
    }
    return false;
}

void pwm_init(PWM_Timer timer, PWM_Channel ch, PWM_Mode mode, uint32_t freq_hz, float initial_duty_percent) {
    uint8_t cs;
    uint16_t top;
  
    if(!find_prescaler_pwm(freq_hz, mode, &cs, &top)) return;

    uint16_t initial_ocr = (uint16_t)((initial_duty_percent / 100.0f) * top);

    cli();
    switch(timer) {
        case PWM_TIMER1:
            
            switch(ch) {
                case PWM_CH_A:
                SETBIT(DDRB, PB5);
                SETBIT(TCCR1A, COM1A1);
                CLRBIT(TCCR1A, COM1A0);
                OCR1A = initial_ocr;
                break;
                case PWM_CH_B:
                SETBIT(DDRB, PB6);
                SETBIT(TCCR1A, COM1B1);
                CLRBIT(TCCR1A, COM1B0);
                OCR1B = initial_ocr;
                break;
                case PWM_CH_C:
                SETBIT(DDRB, PB7);
                SETBIT(TCCR1A, COM1C1);
                CLRBIT(TCCR1A, COM1C0);
                OCR1C = initial_ocr;
                break;
                default:
                break;
            }
            switch(mode) {
                case PWM_FAST:
                CLRBIT(TCCR1A, WGM10);
                SETBIT(TCCR1A, WGM11);
                SETBIT(TCCR1B, WGM12);
                SETBIT(TCCR1B, WGM13);
                break;
                case PWM_PHASE_CORRECT: 
                CLRBIT(TCCR1A, WGM10);
                SETBIT(TCCR1A, WGM11);
                CLRBIT(TCCR1B, WGM12);
                SETBIT(TCCR1B, WGM13);
                break;
                case PWM_PHASE_FREQ_CORRECT:
                CLRBIT(TCCR1A, WGM10);
                CLRBIT(TCCR1A, WGM11);
                CLRBIT(TCCR1B, WGM12);
                SETBIT(TCCR1B, WGM13);
                break;
                default:
                break;
            }

                TCCR1B |= cs;
                ICR1 = top;    
                TCNT1 = 0;
            break;

        case PWM_TIMER3:
        switch(ch) {
                case PWM_CH_A:
                SETBIT(DDRE, PE3);
                SETBIT(TCCR3A, COM3A1);
                CLRBIT(TCCR3A, COM3A0);
                OCR3A = initial_ocr;
                break;
                case PWM_CH_B:
                SETBIT(DDRE, PE4);
                SETBIT(TCCR3A, COM3B1);
                CLRBIT(TCCR3A, COM3B0);
                OCR3B = initial_ocr;
                break;
                case PWM_CH_C:
                SETBIT(DDRE, PE5);
                SETBIT(TCCR3A, COM3C1);
                CLRBIT(TCCR3A, COM3C0);
                OCR3C = initial_ocr;
                break;
                default:
                break;
            }
            switch(mode) {
                case PWM_FAST:
                CLRBIT(TCCR3A, WGM30);
                SETBIT(TCCR3A, WGM31);
                SETBIT(TCCR3B, WGM32);
                SETBIT(TCCR3B, WGM33);
                break;
                case PWM_PHASE_CORRECT: 
                CLRBIT(TCCR3A, WGM30);
                SETBIT(TCCR3A, WGM31);
                CLRBIT(TCCR3B, WGM32);
                SETBIT(TCCR3B, WGM33);
                break;
                case PWM_PHASE_FREQ_CORRECT:
                CLRBIT(TCCR3A, WGM30);
                CLRBIT(TCCR3A, WGM31);
                CLRBIT(TCCR3B, WGM32);
                SETBIT(TCCR3B, WGM33);
            break;
            default:
            break;
            }
                TCCR3B |= cs;
                ICR3 = top;    
                TCNT3 = 0;
                break;

        case PWM_TIMER4:
            switch(ch) {
                case PWM_CH_A:
                SETBIT(DDRH, PH3);
                SETBIT(TCCR4A, COM4A1);
                CLRBIT(TCCR4A, COM4A0);
                OCR4A = initial_ocr;
                break;
                case PWM_CH_B:
                SETBIT(DDRH, PH4);
                SETBIT(TCCR4A, COM4B1);
                CLRBIT(TCCR4A, COM4B0);
                OCR4B = initial_ocr;
                break;
                case PWM_CH_C:
                SETBIT(DDRH, PH5);
                SETBIT(TCCR4A, COM4C1);
                CLRBIT(TCCR4A, COM4C0);
                OCR4C = initial_ocr;
                break;
                default:
                break;
            }
            switch(mode) {
                case PWM_FAST:
                CLRBIT(TCCR4A, WGM40);
                SETBIT(TCCR4A, WGM41);
                SETBIT(TCCR4B, WGM42);
                SETBIT(TCCR4B, WGM43);
                break;
                case PWM_PHASE_CORRECT: 
                CLRBIT(TCCR4A, WGM40);
                SETBIT(TCCR4A, WGM41);
                CLRBIT(TCCR4B, WGM42);
                SETBIT(TCCR4B, WGM43);
                break;
                case PWM_PHASE_FREQ_CORRECT:
                CLRBIT(TCCR4A, WGM40);
                CLRBIT(TCCR4A, WGM41);
                CLRBIT(TCCR4B, WGM42);
                SETBIT(TCCR4B, WGM43);
            break;
            default:
            break;
            }
                TCCR4B |= cs;
                ICR4 = top;    
                TCNT4 = 0;
            break;

        case PWM_TIMER5:
        switch(ch) {
                case PWM_CH_A:
                SETBIT(DDRL, PL3);
                SETBIT(TCCR5A, COM5A1);
                CLRBIT(TCCR5A, COM5A0);
                OCR5A = initial_ocr;
                break;
                case PWM_CH_B:
                SETBIT(DDRL, PL4);
                SETBIT(TCCR5A, COM5B1);
                CLRBIT(TCCR5A, COM5B0);
                OCR5B = initial_ocr;
                break;
                case PWM_CH_C:
                SETBIT(DDRL, PL5);
                SETBIT(TCCR5A, COM5C1);
                CLRBIT(TCCR5A, COM5C0);
                OCR5C = initial_ocr;
                break;
                default:
                break;
            }
            switch(mode) {
                case PWM_FAST:
                CLRBIT(TCCR5A, WGM50);
                SETBIT(TCCR5A, WGM51);
                SETBIT(TCCR5B, WGM52);
                SETBIT(TCCR5B, WGM53);
                break;
                case PWM_PHASE_CORRECT: 
                CLRBIT(TCCR5A, WGM50);
                SETBIT(TCCR5A, WGM51);
                CLRBIT(TCCR5B, WGM52);
                SETBIT(TCCR5B, WGM53);
                break;
                case PWM_PHASE_FREQ_CORRECT:
                CLRBIT(TCCR5A, WGM50);
                CLRBIT(TCCR5A, WGM51);
                CLRBIT(TCCR5B, WGM52);
                SETBIT(TCCR5B, WGM53);
            break;
            default:
            break;
    }
                TCCR5B |= cs;
                ICR5 = top;    
                TCNT5 = 0;
                break;
                default:
                break;
}
        sei();
}


// Opdater pulse width direkte
void pwm_set_ocr(PWM_Timer timer, PWM_Channel ch, uint16_t ocr) {

    switch(timer) {
        case PWM_TIMER1:

        switch(ch) {
            case PWM_CH_A:
            OCR1A = ocr;
            break;
            case PWM_CH_B:
            OCR1B = ocr;
            break;
            case PWM_CH_C:
            OCR1C = ocr;
            break;
            default:
            break;
        }

        break;
        case PWM_TIMER3:
        switch(ch) {
            case PWM_CH_A:
            OCR3A = ocr;
            break;
            case PWM_CH_B:
            OCR3B = ocr;
            break;
            case PWM_CH_C:
            OCR3C = ocr;
            break;
            default:
            break;
        }
        break;
        case PWM_TIMER4:
        switch(ch) {
            case PWM_CH_A:
            OCR4A = ocr;
            break;
            case PWM_CH_B:
            OCR4B = ocr;
            break;
            case PWM_CH_C:
            OCR4C = ocr;
            break;
            default:
            break;
        }
        break;
        case PWM_TIMER5:
        switch(ch) {
            case PWM_CH_A:
            OCR5A = ocr;
            break;
            case PWM_CH_B:
            OCR5B = ocr;
            break;
            case PWM_CH_C:
            OCR5C = ocr;
            break;
            default:
            break;
        }
        break;

        default:
        break;
    }

}

// Opdater pulse width gennem duty cycle % (0.0 - 100.0)
void pwm_set_duty(PWM_Timer timer, PWM_Channel ch, float duty_percent) {

    if(duty_percent < 0.0f || duty_percent > 100.0f) return;

    uint16_t top = 0;

    switch(timer) {
        case PWM_TIMER1: 
        top = ICR1;
        break;
        case PWM_TIMER3: 
        top = ICR3;
        break;
        case PWM_TIMER4: 
        top = ICR4;
        break;
        case PWM_TIMER5: 
        top = ICR5;
        break;
        default:
        break;
    }

    uint16_t initial_ocr = (uint16_t)((duty_percent / 100.0f) * top);
    pwm_set_ocr(timer, ch , initial_ocr);

}

// Servo specifik setup
void servo_init(PWM_Timer timer, PWM_Channel ch, ServoType type) {

    pwm_init(timer, ch, PWM_PHASE_FREQ_CORRECT, 50, 7.5);
    servo_types[timer] = type;  // Stores the type into the array
}

void servo_set_ocr(PWM_Timer timer, PWM_Channel ch,  uint16_t ocr) {

    if(ocr > 2000) ocr = 2000;
    if(ocr < 1000) ocr = 1000;

    pwm_set_ocr(timer, ch, ocr);

}

void servo_set_degrees(PWM_Timer timer, PWM_Channel ch, int8_t degrees) {
    int8_t max_degrees;
    ServoType type = servo_types[timer];
    switch(type) {
    case SERVO_MG90S:  
    max_degrees = 90; break;
    case SERVO_MG996R: 
    max_degrees = 60; break;
    default:           
    max_degrees = 90; break;
   }

   uint16_t ocr = 1500 + ((int16_t)degrees * 500 / max_degrees);
   servo_set_ocr(timer, ch, ocr);
}





/*

Max pulse width: 2.5 ms / 20 ms = 0.125 ms --> 12.5% --> 5V
Min pulse width: 0.5 ms / 20 ms = 0.025 ms --> 2.5 % --> 0V

Default / Center = 1.5 ms / 20 ms = 0.075 ms --> 7.5% --> 2.5V

duty cycle = tid høj / (1/PWM_freg) = OCR/Top 

Frequency = 50Hz

*/