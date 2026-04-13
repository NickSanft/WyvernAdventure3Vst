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
    envInitialVolume = 15;
    envDirection = false;
    envPeriod = 0;
    currentVolume = 0;
    envTimer = 0.0;
    sweepPeriod = 0;
    sweepNegate = false;
    sweepShift = 0;
    shadowFrequency = 0;
    sweepTimer = 0.0;
    sweepActive = false;
    active = false;
    noteHeld = false;
}

void PulseChannel::setSampleRate(double sampleRate)
{
    hostSampleRate = sampleRate;
}

void PulseChannel::noteOn(int period, float /*velocity*/)
{
    periodRegister = std::clamp(period, 0, 2047);
    frequencyHz = pulseFrequency(periodRegister);

    // Reset phase
    phaseAccumulator = 0.0;
    dutyStep = 0;

    // Reset envelope
    currentVolume = envInitialVolume;
    envTimer = 0.0;

    // Reset sweep (CH1 only)
    if (sweepEnabled)
    {
        shadowFrequency = periodRegister;
        sweepTimer = 0.0;
        sweepActive = (sweepPeriod > 0 && sweepShift > 0);

        // Overflow check on trigger
        if (sweepShift > 0)
        {
            int newFreq = calculateSweepFrequency();
            if (newFreq > 2047)
            {
                active = false;
                return;
            }
        }
    }

    active = true;
    noteHeld = true;
}

void PulseChannel::noteOff()
{
    noteHeld = false;
    // Channel stays active until envelope reaches 0 (if decreasing)
    // or we just deactivate immediately for simplicity matching GBC behavior
    // On real GBC, noteOff isn't a concept — the game writes to NR14/NR24
    // For VST usability, we let the envelope finish naturally
}

float PulseChannel::processSample()
{
    if (!active)
        return 0.0f;

    // Clock the envelope (64 Hz)
    clockEnvelope();

    // Clock the sweep (128 Hz, CH1 only)
    if (sweepEnabled)
        clockSweep();

    // If volume is 0 and envelope is decreasing (or note released), deactivate
    if (currentVolume == 0 && !envDirection)
    {
        if (!noteHeld)
            active = false;
        return 0.0f;
    }

    // Advance the duty cycle phase
    // The pulse waveform has 8 steps per cycle
    double phaseIncrement = (frequencyHz * 8.0) / hostSampleRate;
    phaseAccumulator += phaseIncrement;

    while (phaseAccumulator >= 8.0)
    {
        phaseAccumulator -= 8.0;
    }

    dutyStep = static_cast<int>(phaseAccumulator) & 7;

    // Look up duty table
    int dutyOutput = DUTY_TABLE[dutyMode][dutyStep];

    // Scale by volume (0-15) and normalize to -1.0 to +1.0
    float sample = dutyOutput ? 1.0f : -1.0f;
    sample *= (static_cast<float>(currentVolume) / 15.0f);

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

void PulseChannel::setEnvelope(int initialVol, bool direction, int period)
{
    envInitialVolume = std::clamp(initialVol, 0, 15);
    envDirection = direction;
    envPeriod = std::clamp(period, 0, 7);
}

void PulseChannel::setSweep(int period, bool negate, int shift)
{
    if (!sweepEnabled) return;
    sweepPeriod = std::clamp(period, 0, 7);
    sweepNegate = negate;
    sweepShift = std::clamp(shift, 0, 7);
}

void PulseChannel::clockEnvelope()
{
    if (envPeriod == 0) return;

    envTimer += 1.0 / hostSampleRate;
    double envClockInterval = 1.0 / ENVELOPE_CLOCK_HZ;
    double envStepInterval = envClockInterval * envPeriod;

    while (envTimer >= envStepInterval)
    {
        envTimer -= envStepInterval;

        if (envDirection)
        {
            // Increase volume
            if (currentVolume < 15)
                currentVolume++;
        }
        else
        {
            // Decrease volume
            if (currentVolume > 0)
                currentVolume--;
        }
    }
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
            // Overflow — silence the channel
            active = false;
            return;
        }

        if (sweepShift > 0)
        {
            shadowFrequency = newFreq;
            periodRegister = newFreq;
            frequencyHz = pulseFrequency(periodRegister);

            // Second overflow check after write-back
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
