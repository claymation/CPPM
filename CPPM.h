#ifndef CPPM_H
#define CPPM_H

#include <inttypes.h>
#include <avr/interrupt.h>

#define ICP1    8   // Input capture pin 1

#define CPPM_MAX_CHANNELS       16
#define CPPM_FRAME_SIZE_IN_MS   27      // FrSky D4R-II CPPM frame size

#define CPPM_SYNC_PULSE_WIDTH_FLOOR    10000    // Minimum tolerable value
#define CPPM_SYNC_PULSE_WIDTH_CEIL     50000    // Maximum tolerable value
#define CPPM_CHAN_PULSE_WIDTH_MIN       2000    // Minimum expected value
#define CPPM_CHAN_PULSE_WIDTH_MAX       4000    // Maximum expected value
#define CPPM_CHAN_PULSE_WIDTH_FLOOR     1500    // Minimum tolerable value
#define CPPM_CHAN_PULSE_WIDTH_CEIL      4500    // Maximum tolerable value

ISR(TIMER1_CAPT_vect);

class CPPMReceiver
{
private:
    enum PulseState { SYNC_PULSE, CHAN_PULSE };

    uint8_t _numChannels;
    volatile bool _synced;
    volatile PulseState _state;
    volatile int16_t _channels[CPPM_MAX_CHANNELS];

public:
    CPPMReceiver() : _synced(false), _state(SYNC_PULSE) {};
    void begin(uint8_t numChannels);
    void end();
    bool ok();
    void read(int16_t *channels);

    friend void TIMER1_CAPT_vect();
};

extern CPPMReceiver CPPM;

#endif
