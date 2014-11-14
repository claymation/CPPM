Combined PPM (CPPM) Receiver Library for Arduino
================================================

This library provides a simple interface for reading
up to 16 channels of RC input from a single CPPM signal:

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

            // do something fun with the channel values, like fly a quadcopter...
        }
    }

In its current form, it most likely only works on Arduino Uno. It requires
exclusive access to timer1, preventing the use of PWM on digital output pins
9 & 10 (whose waveform generator also use timer1).
