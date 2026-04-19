#pragma once

#include "GBCChannel.h"
#include "GBCConstants.h"
#include "GBCEnvelope.h"

class PulseChannel : public GBCChannel
{
public:
    PulseChannel(bool hasSweep = false);

    void reset() override;
    void setSampleRate(double sampleRate) override;
    void noteOn(int period, float velocity) override;
    void noteOff() override;
    float processSample() override;
    bool isActive() const override;

    // --- Parameter setters ---

    // Duty cycle: 0 = 12.5%, 1 = 25%, 2 = 50%, 3 = 75%
    void setDutyCycle(int duty);

    // Peak volume (0-15) — the level the envelope reaches at the end of attack
    void setPeakLevel(int peak);

    // Full GBCEnvelope envelope parameters. attackMs/decayMs/releaseMs in milliseconds,
    // sustainLevel in 0..15 range.
    void setADSR(float attackMs, float decayMs, float sustainLevel, float releaseMs);

    // Frequency sweep (CH1 only): period (0-7), negate (bool), shift (0-7)
    void setSweep(int period, bool negate, int shift);

private:
    void clockSweep();
    int calculateSweepFrequency();

    // Configuration
    bool sweepEnabled;       // true for CH1, false for CH2
    double hostSampleRate = 44100.0;

    // Duty cycle
    int dutyMode = 2;        // Default 50%
    int dutyStep = 0;        // Current step in 8-step duty pattern (0-7)
    double phaseAccumulator = 0.0;

    // Frequency
    int periodRegister = 0;
    double frequencyHz = 0.0;

    // GBCEnvelope envelope (replaces old initVol/direction/period)
    GBCEnvelope adsr;

    // Frequency sweep (CH1 only)
    int sweepPeriod = 0;       // 0 = disabled
    bool sweepNegate = false;
    int sweepShift = 0;
    int shadowFrequency = 0;
    double sweepTimer = 0.0;
    bool sweepActive = false;

    // State
    bool active = false;
    bool noteHeld = false;
};
