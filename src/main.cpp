#include "setup/pwm.h"
#include <util/delay.h>

int main(void) {

    servo_init(PWM_TIMER1, PWM_CH_A, SERVO_MG996R);

    while(1) {
        servo_set_degrees(PWM_TIMER1, PWM_CH_A, 0);    // center
        _delay_ms(1000);

        servo_set_degrees(PWM_TIMER1, PWM_CH_A, 60);   // full right
        _delay_ms(1000);

        servo_set_degrees(PWM_TIMER1, PWM_CH_A, -60);  // full left
        _delay_ms(1000);
    }
}