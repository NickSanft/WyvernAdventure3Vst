#include "PulseChannel.h"
#include <cmath>
#include <algorithm>

PulseChannel::PulseChannel(bool hasSweep)
    : sweepEnabled(hasSweep)
{
    reset();
}

void PulseChannel::reset()
{
    dutyMode = 2;
    dutyStep = 0;
    phaseAccumulator = 0.0;
    periodRegister = 0;
    frequencyHz = 0.0;
    sweepPeriod = 0;
    sweepNegate = false;
    sweepShift = 0;
    shadowFrequency = 0;
    sweepTimer = 0.0;
    sweepActive = false;
    active = false;
    noteHeld = false;
    adsr.reset();
}

void PulseChannel::setSampleRate(double sampleRate)
{
    hostSampleRate = sampleRate;
    adsr.setSampleRate(sampleRate);
}

void PulseChannel::noteOn(int period, float /*velocity*/)
{
    periodRegister = std::clamp(period, 0, 2047);
    frequencyHz = pulseFrequency(periodRegister);

    phaseAccumulator = 0.0;
    dutyStep = 0;

    // Reset sweep (CH1 only)
    if (sweepEnabled)
    {
        shadowFrequency = periodRegister;
        sweepTimer = 0.0;
        sweepActive = (sweepPeriod > 0 && sweepShift > 0);

        // Overflow check on trigger — disable sweep rather than silence
        if (sweepShift > 0 && calculateSweepFrequency() > 2047)
            sweepActive = false;
    }

    adsr.noteOn();
    active = true;
    noteHeld = true;
}

void PulseChannel::noteOff()
{
    noteHeld = false;
    adsr.noteOff();
}

float PulseChannel::processSample()
{
    if (!active)
        return 0.0f;

    // Advance the GBCEnvelope envelope
    float envLevel = adsr.tick();
    if (!adsr.isActive())
    {
        active = false;
        return 0.0f;
    }

    // Clock the sweep (128 Hz, CH1 only)
    if (sweepEnabled)
        clockSweep();

    // Advance the duty cycle phase
    float vibratoMul = tickVibrato(hostSampleRate);
    double phaseIncrement = (frequencyHz * vibratoMul * 8.0) / hostSampleRate;
    phaseAccumulator += phaseIncrement;

    while (phaseAccumulator >= 8.0)
        phaseAccumulator -= 8.0;

    dutyStep = static_cast<int>(phaseAccumulator) & 7;

    int dutyOutput = DUTY_TABLE[dutyMode][dutyStep];

    float sample = dutyOutput ? 1.0f : -1.0f;
    sample *= (envLevel / 15.0f);

    return sample;
}

bool PulseChannel::isActive() const
{
    return active;
}

void PulseChannel::setDutyCycle(int duty)
{
    dutyMode = std::clamp(duty, 0, 3);
}

void PulseChannel::setPeakLevel(int peak)
{
    adsr.setPeakLevel(static_cast<float>(std::clamp(peak, 0, 15)));
}

void PulseChannel::setADSR(float attackMs, float decayMs, float sustainLevel, float releaseMs)
{
    adsr.setParams(attackMs, decayMs, sustainLevel, releaseMs);
}

void PulseChannel::setSweep(int period, bool negate, int shift)
{
    if (!sweepEnabled) return;
    sweepPeriod = std::clamp(period, 0, 7);
    sweepNegate = negate;
    sweepShift = std::clamp(shift, 0, 7);
}

void PulseChannel::clockSweep()
{
    if (!sweepActive || sweepPeriod == 0) return;

    sweepTimer += 1.0 / hostSampleRate;
    double sweepClockInterval = 1.0 / SWEEP_CLOCK_HZ;
    double sweepStepInterval = sweepClockInterval * sweepPeriod;

    while (sweepTimer >= sweepStepInterval)
    {
        sweepTimer -= sweepStepInterval;

        int newFreq = calculateSweepFrequency();

        if (newFreq > 2047)
        {
            active = false;
            return;
        }

        if (sweepShift > 0)
        {
            shadowFrequency = newFreq;
            periodRegister = newFreq;
            frequencyHz = pulseFrequency(periodRegister);

            int checkFreq = calculateSweepFrequency();
            if (checkFreq > 2047)
            {
                active = false;
                return;
            }
        }
    }
}

int PulseChannel::calculateSweepFrequency()
{
    int delta = shadowFrequency >> sweepShift;
    int newFreq = sweepNegate ? (shadowFrequency - delta) : (shadowFrequency + delta);
    return newFreq;
}
