#include <Arduino.h>
#include <avr/io.h>
#include "CPPM.h"


/**
 * Enable the input capture interrupt.
 */
void CPPMReceiver::begin(uint8_t numChannels)
{
    _numChannels = numChannels;

    // Configure the input capture pin
    pinMode(ICP1, INPUT);

    // Configure timer1: disable PWM, set prescale = 8x (0.5 usec ticks)
    TCCR1A = 0;
    TCCR1B = _BV(CS11);
    TCCR1C = 0;

    // Enable the interrupt and clear the mask
    bitSet(TIFR1, ICF1);
    bitSet(TIMSK1, ICIE1);
}

/**
 * Disable the input capture interrupt.
 */
void CPPMReceiver::end()
{
    bitClear(TIMSK1, ICIE1);
}

/**
 * Return true if a sane CPPM signal has recently been detected on the input pin.
 */
bool CPPMReceiver::ok(void)
{
    // Need to devise a watchdog; this is never cleared when signal is lost
    return CPPM._synced;
}

/**
 * Atomically read the current CPPM channel values into the array pointed to by `channels`.
 *
 * Each channel value is mapped from a raw pulse width to an int16_t value (nominally 0-255).
 */
void CPPMReceiver::read(int16_t *channels)
{
    noInterrupts();
    for (int i = 0; i < _numChannels; ++i)
    {
        channels[i] = map(_channels[i],
                          CPPM_CHAN_PULSE_WIDTH_MIN,
                          CPPM_CHAN_PULSE_WIDTH_MAX,
                          0, 255);
    }
    interrupts();
}

/**
 * TIMER1_CAPT_vect is invoked by the AVR timer hardware when a pulse appears on the input pin.
 *
 * On entry, the width of the pulse (as measured by timer1) will have been moved into ICR1.
 *
 * As usual, interrupts are disabled inside the handler.
 */
ISR(TIMER1_CAPT_vect)
{
    static uint8_t channel;

    // Reset counter to measure next pulse
    TCNT1 = 0;

    // Read the captured pulse width
    uint16_t width = ICR1;

    switch (CPPM._state)
    {
      case CPPMReceiver::SYNC_PULSE:
        // If we've received a tolerable sync pulse, prepare
        // to read the channel pulses (starting with channel 0)
        if ((width >= CPPM_SYNC_PULSE_WIDTH_FLOOR) && (width <= CPPM_SYNC_PULSE_WIDTH_CEIL))
        {
            CPPM._state = CPPMReceiver::CHAN_PULSE;
            channel = 0;
        }
        // If we've received an untolerable sync pulse,
        // we may have suffered a framing error
        else
        {
            CPPM._synced = false;
        }
        break;

      case CPPMReceiver::CHAN_PULSE:
        // If we've received a tolerable channel pulse,
        // record it and advance to the next channel
        if ((width >= CPPM_CHAN_PULSE_WIDTH_FLOOR) && (width <= CPPM_CHAN_PULSE_WIDTH_CEIL))
        {
            CPPM._channels[channel] = width;
            channel++;

            // If the complete frame has been received,
            // prepare to read the sync pulse for the next frame
            if (channel == CPPM._numChannels)
            {
                CPPM._synced = true;
                CPPM._state = CPPMReceiver::SYNC_PULSE;
            }
        }
        // If we've received an untolerable channel pulse,
        // we may have suffered a framing error
        else
        {
            CPPM._synced = false;
            CPPM._state = CPPMReceiver::SYNC_PULSE;
        }
        break;
    }
}

// Preinstantiate object
CPPMReceiver CPPM;
