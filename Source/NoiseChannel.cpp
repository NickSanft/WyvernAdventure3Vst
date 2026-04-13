#include "NoiseChannel.h"
#include <algorithm>

NoiseChannel::NoiseChannel()
{
    reset();
}

void NoiseChannel::reset()
{
    lfsr = 0x7FFF;
    narrowMode = false;
    clockShift = 0;
    divisorCode = 0;
    noiseClockHz = 0.0;
    clockAccumulator = 0.0;
    outputBit = 0;
    envInitialVolume = 15;
    envDirection = false;
    envPeriod = 0;
    currentVolume = 0;
    envTimer = 0.0;
    active = false;
    noteHeld = false;
}

void NoiseChannel::setSampleRate(double sampleRate)
{
    hostSampleRate = sampleRate;
}

void NoiseChannel::noteOn(int /*period*/, float velocity)
{
    // Noise channel doesn't use period from MIDI note directly.
    // Instead, MIDI velocity maps to envelope initial volume.
    // Clock shift and divisor are set via parameters.
    envInitialVolume = std::clamp(static_cast<int>(velocity * 15.0f), 0, 15);
    currentVolume = envInitialVolume;
    envTimer = 0.0;

    // Reset LFSR
    lfsr = 0x7FFF;
    clockAccumulator = 0.0;
    outputBit = 0;

    // Calculate noise clock frequency
    int divisor = NOISE_DIVISORS[divisorCode];
    if (clockShift < 14)
        noiseClockHz = GBC_CLOCK_HZ / static_cast<double>(divisor << clockShift);
    else
        noiseClockHz = 0.0;  // Shift 14+ effectively silences

    active = (noiseClockHz > 0.0);
    noteHeld = true;
}

void NoiseChannel::noteOff()
{
    noteHeld = false;
    // Let envelope finish naturally
}

float NoiseChannel::processSample()
{
    if (!active)
        return 0.0f;

    // Clock the envelope
    clockEnvelope();

    // If volume reached 0 and decreasing, potentially deactivate
    if (currentVolume == 0 && !envDirection)
    {
        if (!noteHeld)
            active = false;
        return 0.0f;
    }

    // Clock the LFSR at the noise frequency
    clockAccumulator += noiseClockHz / hostSampleRate;
    while (clockAccumulator >= 1.0)
    {
        clockAccumulator -= 1.0;
        outputBit = clockLFSR();
    }

    // Output: LFSR output bit scaled by volume
    float sample = outputBit ? 1.0f : -1.0f;
    sample *= (static_cast<float>(currentVolume) / 15.0f);

    return sample;
}

bool NoiseChannel::isActive() const
{
    return active;
}

void NoiseChannel::setClockShift(int shift)
{
    clockShift = std::clamp(shift, 0, 13);
}

void NoiseChannel::setDivisorCode(int code)
{
    divisorCode = std::clamp(code, 0, 7);
}

void NoiseChannel::setWidthMode(bool narrow)
{
    narrowMode = narrow;
}

void NoiseChannel::setEnvelope(int initialVol, bool direction, int period)
{
    envInitialVolume = std::clamp(initialVol, 0, 15);
    envDirection = direction;
    envPeriod = std::clamp(period, 0, 7);
}

int NoiseChannel::clockLFSR()
{
    // XOR bits 0 and 1
    int bit = (lfsr ^ (lfsr >> 1)) & 1;

    // Shift right, feed back to bit 14
    lfsr = static_cast<uint16_t>((lfsr >> 1) | (bit << 14));

    // In narrow (7-bit) mode, also set bit 6
    if (narrowMode)
    {
        lfsr = static_cast<uint16_t>((lfsr & ~(1 << 6)) | (bit << 6));
    }

    // Output is inverted bit 0
    return ~lfsr & 1;
}

void NoiseChannel::clockEnvelope()
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
            if (currentVolume < 15)
                currentVolume++;
        }
        else
        {
            if (currentVolume > 0)
                currentVolume--;
        }
    }
}
