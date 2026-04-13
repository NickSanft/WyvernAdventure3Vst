#pragma once

#include "GBCChannel.h"
#include "GBCConstants.h"
#include <cstdint>

class NoiseChannel : public GBCChannel
{
public:
    NoiseChannel();

    void reset() override;
    void setSampleRate(double sampleRate) override;
    void noteOn(int period, float velocity) override;
    void noteOff() override;
    float processSample() override;
    bool isActive() const override;

    // --- Parameter setters ---

    // Clock shift (0-13): higher = lower frequency noise
    void setClockShift(int shift);

    // Divisor code (0-7): selects base divisor from NOISE_DIVISORS table
    void setDivisorCode(int code);

    // Width mode: false = 15-bit (white noise), true = 7-bit (metallic/buzzy)
    void setWidthMode(bool narrow);

    // Volume envelope: initialVol (0-15), direction (true = up), period (0-7)
    void setEnvelope(int initialVol, bool direction, int period);

private:
    int clockLFSR();
    void clockEnvelope();

    double hostSampleRate = 44100.0;

    // LFSR state
    uint16_t lfsr = 0x7FFF;  // 15-bit, all ones initial state
    bool narrowMode = false;  // false = 15-bit, true = 7-bit

    // Clock configuration
    int clockShift = 0;    // 0-13
    int divisorCode = 0;   // 0-7
    double noiseClockHz = 0.0;
    double clockAccumulator = 0.0;

    // Current output bit
    int outputBit = 0;

    // Volume envelope
    int envInitialVolume = 15;
    bool envDirection = false;
    int envPeriod = 0;
    int currentVolume = 0;
    double envTimer = 0.0;

    // State
    bool active = false;
    bool noteHeld = false;
};
