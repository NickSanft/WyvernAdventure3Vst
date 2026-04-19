#pragma once

#include "GBCChannel.h"
#include "GBCConstants.h"
#include "GBCEnvelope.h"
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

    void setClockShift(int shift);
    void setDivisorCode(int code);
    void setWidthMode(bool narrow);

    void setPeakLevel(int peak);
    void setADSR(float attackMs, float decayMs, float sustainLevel, float releaseMs);

private:
    int clockLFSR();

    double hostSampleRate = 44100.0;

    uint16_t lfsr = 0x7FFF;
    bool narrowMode = false;

    int clockShift = 0;
    int divisorCode = 0;
    double noiseClockHz = 0.0;
    double clockAccumulator = 0.0;

    int outputBit = 0;

    GBCEnvelope adsr;

    bool active = false;
    bool noteHeld = false;
};
