/**
 * Simple demonstration of the Combined PPM (CPPM) receiver library using a multi-color LED.
 *
 * Moving the sticks on the RC transmitter will change the LED's color and brightness.
 */

#include <CPPM.h>

const uint8_t NUM_CHANNELS = 8;

// CPPM channel assignments
const uint8_t Y_CHAN = 2;       // throttle = luminance (brightness)
const uint8_t R_CHAN = 3;       // rudder   = red
const uint8_t G_CHAN = 0;       // aileron  = green
const uint8_t B_CHAN = 1;       // elevator = blue

// OUTPUT: Multi-color LED pins
const uint8_t R_LED_PIN = 5;    // uses timer0
const uint8_t G_LED_PIN = 6;    // uses timer0
const uint8_t B_LED_PIN = 11;   // uses timer2

void setup(void)
{
    CPPM.begin(NUM_CHANNELS);
}

void loop(void)
{
    int16_t channels[NUM_CHANNELS];

    if (CPPM.ok())
    {
        CPPM.read(channels);

        double brightness = channels[Y_CHAN] / 255.0;

        analogWrite(R_LED_PIN, constrain(channels[R_CHAN] * brightness, 0, 255));
        analogWrite(G_LED_PIN, constrain(channels[G_CHAN] * brightness, 0, 255));
        analogWrite(B_LED_PIN, constrain(channels[B_CHAN] * brightness, 0, 255));
    }
    else
    {
        analogWrite(R_LED_PIN, 0);
        analogWrite(G_LED_PIN, 0);
        analogWrite(B_LED_PIN, 0);
    }

    delay(50);
}
