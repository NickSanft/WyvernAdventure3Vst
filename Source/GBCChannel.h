#pragma once

#include <cstdint>
#include <cmath>

// Abstract base class for a single GBC APU channel
class GBCChannel
{
public:
    virtual ~GBCChannel() = default;

    virtual void reset() = 0;
    virtual void setSampleRate(double sampleRate) = 0;
    virtual void noteOn(int period, float velocity) = 0;
    virtual void noteOff() = 0;
    virtual float processSample() = 0;
    virtual bool isActive() const = 0;

    // Stereo panning: 0 = left only, 1 = center (both), 2 = right only
    void setPan(int panMode) { pan = panMode; }
    int getPan() const { return pan; }

    void getStereoGain(float& left, float& right) const
    {
        switch (pan)
        {
            case 0: left = 1.0f; right = 0.0f; break;
            case 2: left = 0.0f; right = 1.0f; break;
            default: left = 1.0f; right = 1.0f; break;
        }
    }

    // --- Vibrato LFO ---
    // rateHz: LFO frequency (typically 4-8 Hz for chiptune)
    // depthCents: pitch deviation in cents (100 = 1 semitone)
    void setVibrato(bool enabled, float rateHz, float depthCents)
    {
        vibratoEnabled = enabled;
        vibratoRateHz = rateHz;
        vibratoDepthCents = depthCents;
    }

protected:
    // Returns a multiplier for frequency based on LFO phase, advances the phase.
    // 1.0 = no modulation, <1.0 = flat, >1.0 = sharp
    float tickVibrato(double hostSampleRate)
    {
        if (!vibratoEnabled || vibratoDepthCents <= 0.0f)
            return 1.0f;

        vibratoPhase += 2.0 * 3.14159265358979323846 * vibratoRateHz / hostSampleRate;
        if (vibratoPhase > 2.0 * 3.14159265358979323846)
            vibratoPhase -= 2.0 * 3.14159265358979323846;

        // Cents to frequency ratio: 2^(cents/1200)
        float cents = std::sin(static_cast<float>(vibratoPhase)) * vibratoDepthCents;
        return std::pow(2.0f, cents / 1200.0f);
    }

    int pan = 1;

    // Vibrato state
    bool vibratoEnabled = false;
    float vibratoRateHz = 6.0f;
    float vibratoDepthCents = 25.0f;
    double vibratoPhase = 0.0;
};
