#pragma once
#include <avr/io.h> 

#define CPU_freq 16000000UL

typedef enum {
    PWM_FAST,
    PWM_PHASE_CORRECT,
    PWM_PHASE_FREQ_CORRECT   // mode 8, ICR er TOP -- moden til servoen
} PWM_Mode;

typedef enum {
    PWM_TIMER1,
    PWM_TIMER3,
    PWM_TIMER4,
    PWM_TIMER5
} PWM_Timer;

typedef enum {
    PWM_CH_A,   // Timer1: pin 11 (OC1A) | Timer3: pin 5 (OC3A) | Timer4: pin 6 (OC4A) | Timer5: pin 46 (OC5A)
    PWM_CH_B,   // Timer1: pin 12 (OC1B) | Timer3: pin 2 (OC3B) | Timer4: pin 7 (OC4B) | Timer5: pin 45 (OC5B)
    PWM_CH_C    // Timer1: pin 13 (OC1C) | Timer3: pin 3 (OC3C) | Timer4: pin 8 (OC4C) | Timer5: pin 44 (OC5C)
} PWM_Channel;

typedef enum {
    SERVO_MG90S,   // 1.0ms - 2.0ms, ±90 degrees
    SERVO_MG996R,  // 1.0ms - 2.0ms, ±60 degrees
    SERVO_GENERIC  // 1.0ms - 2.0ms, ±90 degrees standard
} ServoType;

void pwm_init(PWM_Timer timer, PWM_Channel ch, PWM_Mode mode, uint32_t freq_hz, float initial_duty_percent);

// Opdater pulse width direkte
void pwm_set_ocr(PWM_Timer timer, PWM_Channel ch, uint16_t ocr);

// Opdater pulse width gennem duty cycle % (0.0 - 100.0)
void pwm_set_duty(PWM_Timer timer, PWM_Channel ch, float duty_percent);

// Servo specifik setup
void servo_init(PWM_Timer timer, PWM_Channel ch, ServoType type);
void servo_set_ocr(PWM_Timer timer, PWM_Channel ch, uint16_t ocr); // with safety clamp 2000-4000
void servo_set_degrees(PWM_Timer timer, PWM_Channel ch, int8_t degrees); // -60 til +60 grader



/*

Max pulse width: 2.5 ms / 20 ms = 0.125 ms --> 12.5% --> 5V
Min pulse width: 0.5 ms / 20 ms = 0.025 ms --> 2.5 % --> 0V

Default / Center = 1.5 ms / 20 ms = 0.075 ms --> 7.5% --> 2.5V

duty cycle = tid høj / (1/PWM_freg) = OCR/Top 

Frequency = 50Hz

*/