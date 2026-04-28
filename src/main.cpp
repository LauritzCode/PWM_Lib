#include "setup/pwm.h"
#include <util/delay.h>


int main(void) {

    
    servo_init(PWM_TIMER1, PWM_CH_A, SERVO_GENERIC);

    while(1) {
        
      servo_set_degrees(PWM_TIMER1, PWM_CH_A, 0);
      _delay_ms(1000);
      servo_set_degrees(PWM_TIMER1, PWM_CH_A, 90);
      _delay_ms(1000);
      servo_set_degrees(PWM_TIMER1, PWM_CH_A, -90);
      _delay_ms(1000);
    }
}