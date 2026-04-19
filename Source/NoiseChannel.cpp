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
    active = false;
    noteHeld = false;
    adsr.reset();
    adsr.setPeakLevel(15.0f);
}

void NoiseChannel::setSampleRate(double sampleRate)
{
    hostSampleRate = sampleRate;
    adsr.setSampleRate(sampleRate);
}

void NoiseChannel::noteOn(int /*period*/, float /*velocity*/)
{
    // Reset LFSR on trigger
    lfsr = 0x7FFF;
    clockAccumulator = 0.0;
    outputBit = 0;

    // Calculate noise clock frequency
    int divisor = NOISE_DIVISORS[divisorCode];
    if (clockShift < 14)
        noiseClockHz = GBC_CLOCK_HZ / static_cast<double>(divisor << clockShift);
    else
        noiseClockHz = 0.0;

    adsr.noteOn();
    active = (noiseClockHz > 0.0);
    noteHeld = true;
}

void NoiseChannel::noteOff()
{
    noteHeld = false;
    adsr.noteOff();
}

float NoiseChannel::processSample()
{
    if (!active)
        return 0.0f;

    float envLevel = adsr.tick();
    if (!adsr.isActive())
    {
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

    float sample = outputBit ? 1.0f : -1.0f;
    sample *= (envLevel / 15.0f);
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

void NoiseChannel::setPeakLevel(int peak)
{
    adsr.setPeakLevel(static_cast<float>(std::clamp(peak, 0, 15)));
}

void NoiseChannel::setADSR(float attackMs, float decayMs, float sustainLevel, float releaseMs)
{
    adsr.setParams(attackMs, decayMs, sustainLevel, releaseMs);
}

int NoiseChannel::clockLFSR()
{
    int bit = (lfsr ^ (lfsr >> 1)) & 1;
    lfsr = static_cast<uint16_t>((lfsr >> 1) | (bit << 14));
    if (narrowMode)
        lfsr = static_cast<uint16_t>((lfsr & ~(1 << 6)) | (bit << 6));
    return ~lfsr & 1;
}
