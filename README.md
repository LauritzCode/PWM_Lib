# AVR PWM Library (ATmega2560)

A bare-metal PWM library for the ATmega2560. It wraps Timer1, Timer3, Timer4 and Timer5 (the 16-bit timers) and gives you a clean API for setting up PWM channels, controlling servos, and dimming LEDs or driving motors.

## What it is

The library covers three abstraction levels:

1. **Raw PWM:** Pick a timer, channel, mode (Fast / Phase Correct / Phase & Frequency Correct), frequency in Hz, and an initial duty cycle. The library calculates the right prescaler and TOP value automatically.
2. **Duty cycle control:** Update the duty as a float percentage (0.0 to 100.0).
3. **Servo helpers:** Pick a servo type (MG90S, MG996R, generic) and command it in either raw OCR microseconds or degrees.

All four 16-bit timers on the ATmega2560 are supported across channels A, B and C. So in theory you can drive up to 12 PWM outputs at the same time.

## What it can be used for

- Hobby servos (tested on MG90S, MG996R, and a generic MS18 type)
- LED dimming, including high frequency PWM if you want to avoid visible flicker
- DC motor speed control via an H-bridge or transistor driver
- Driving solenoids or relays in a soft-start fashion
- Generating test signals at a known frequency for scope work
- Anything else that needs hardware PWM with proper timing

The servo functions are just thin wrappers around the same `pwm_set_ocr` calls you can use directly for anything else.

## Example: servo sweep

Standard "is it alive" test. Centers the servo, swings full right, then full left, on Timer1 channel A (pin 11 on the Arduino Mega).

```cpp
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
```

`servo_init` sets up Phase & Frequency Correct PWM at 50 Hz with the pulse already at 1.5 ms (neutral). `servo_set_degrees` then maps the angle to the right OCR value based on the servo type you passed in. If you want to skip the type system and just set raw microseconds, use `pwm_set_ocr` instead.

## Example: LED dimming and motor control

Same library, different use case. LED on Timer3 at 20 kHz so the eye does not see the flicker, motor on Timer4 at 1 kHz through a MOSFET.

```cpp
#include "setup/pwm.h"
#include <util/delay.h>

int main(void) {

    // LED on pin 5 (OC3A), 20 kHz Fast PWM
    pwm_init(PWM_TIMER3, PWM_CH_A, PWM_FAST, 20000, 0.0f);

    // DC motor on pin 6 (OC4A), 1 kHz Fast PWM
    pwm_init(PWM_TIMER4, PWM_CH_A, PWM_FAST, 1000, 0.0f);

    while(1) {

        pwm_set_duty(PWM_TIMER3, PWM_CH_A, 50.0f);
        pwm_set_duty(PWM_TIMER4, PWM_CH_A, 50.0f);
        _delay_ms(2000);

        pwm_set_duty(PWM_TIMER3, PWM_CH_A, 75.0f);
        pwm_set_duty(PWM_TIMER4, PWM_CH_A, 75.0f);
        _delay_ms(2000);
    }
}
```

The LED and motor are on different timers because they need different frequencies. 20 kHz for the LED keeps it above the audible and visible range, 1 kHz for the motor is a typical sweet spot for small brushed DC motors.

## Lessons learned

While testing the servo sweep on a generic MS18 servo, the sweep refused to reach the extremes. Neutral worked fine, but ±90 degrees just nudged slightly off center. Raw `pwm_set_ocr` calls with 500 and 2500 worked perfectly, so I knew the timer setup itself was fine.

The bug was inside `servo_set_degrees`:

```cpp
int16_t ocr = neutral + ((int16_t)degrees * range / max_degrees);
```

I then figured that given the variable is `int16_t`, the maximum value it can hold is 32767. For a generic servo with `range = 1000` and `degrees = 90`, the intermediate product `90 * 1000 = 90000` overflows that limit. The result wraps around to something near neutral, which is exactly why the symptom looked like "goes to neutral fine but does not reach the extremes". On AVR, `int` is 16 bits anyway, so even without the cast the multiplication still overflows.

The fix was luckily simple. I just forced the multiplication into 32-bit:

```cpp
int16_t ocr = neutral + ((int32_t)degrees * range / max_degrees);
```

That keeps full precision through the multiply, and the divide at the end brings it back into a safe range for `int16_t`. After that the sweep hit both extremes properly. Lesson for me: on a 16-bit MCU, every multiply-then-divide chain needs a quick mental check on the intermediate product. ADC scaling, encoder math, mm-per-tick, all of it can trip on the same thing.

## Potential future expansions

The library works but is pretty bare. Some directions it could grow in:

**LED helpers**
- `led_init(timer, ch)` that picks a sensible default frequency (something like 20 kHz to avoid flicker)
- `led_set_brightness(timer, ch, percent)` with optional gamma correction, since human perception of brightness is logarithmic and a linear duty cycle does not look linear to the eye
- A `led_fade(timer, ch, from, to, duration_ms)` that handles the ramp internally

**Motor helpers**
- `motor_init(timer, ch_forward, ch_reverse)` for H-bridge setups, where two channels work as a pair
- `motor_set_speed(motor, signed_percent)` where negative values reverse direction
- Optional soft-start and soft-stop ramps to protect gearboxes
- Brake vs coast handling, since these behave differently depending on H-bridge type
- Support for BLDC motor ESCs, which use the same 50 Hz / 1-2 ms pulse as servos but with different calibration

**More servo types**
- A way to register custom servo profiles at runtime (min pulse, max pulse, angle range) instead of hardcoding the enum
- Continuous rotation servos, where "degrees" means speed and direction instead of position

**Better safety and robustness**
- Bounds checking on the timer-channel combinations that actually exist on the chip
- Proper handling when `servo_init` and `pwm_init` are called on the same timer in conflicting modes
- A `pwm_stop(timer, ch)` function to cleanly disconnect a channel without tearing down the whole timer

**8-bit timer support**
- Currently only the four 16-bit timers are wrapped. Timer0 and Timer2 are 8-bit but still useful for low resolution PWM, and adding them would unlock pins 4, 9, 10 and 13 on the Mega.

The servo helpers are a working example of how to layer a higher level abstraction on top of the raw PWM functions. Same pattern would apply for LEDs and motors.

## File structure

```
pwm.h     // public API, enums, types
pwm.c     // implementation, register-level timer setup
macros.h  // SETBIT / CLRBIT helpers
```

## Hardware

- ATmega2560 at 16 MHz (Arduino Mega 2560 board for development)
- Standard pin mapping per the header comments
