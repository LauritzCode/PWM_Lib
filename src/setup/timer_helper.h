#pragma once 

#include <avr/io.h> 

#define CPU_freq 16000000UL

typedef enum TimerID {
    Timer0,
    Timer1,
    Timer3,
    Timer4,
    Timer5,
} TimerID;


void initialize_timer(TimerID timer, uint32_t freq);

void start_timer(TimerID timer);

void stop_timer(TimerID timer);

void reset_timer(TimerID timer);