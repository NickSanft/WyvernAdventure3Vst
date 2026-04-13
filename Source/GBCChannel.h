#pragma once

#include <cstdint>

// Abstract base class for a single GBC APU channel
class GBCChannel
{
public:
    virtual ~GBCChannel() = default;

    // Reset channel to initial state
    virtual void reset() = 0;

    // Set the host sample rate (e.g. 44100, 48000)
    virtual void setSampleRate(double sampleRate) = 0;

    // Trigger a note with a given GBC 11-bit period register value and velocity
    virtual void noteOn(int period, float velocity) = 0;

    // Release the current note
    virtual void noteOff() = 0;

    // Generate one mono sample (-1.0 to +1.0)
    virtual float processSample() = 0;

    // Check if the channel is currently producing sound
    virtual bool isActive() const = 0;

    // Stereo panning: 0 = left only, 1 = center (both), 2 = right only
    void setPan(int panMode) { pan = panMode; }
    int getPan() const { return pan; }

    // Get stereo gain multipliers based on pan setting
    void getStereoGain(float& left, float& right) const
    {
        switch (pan)
        {
            case 0: left = 1.0f; right = 0.0f; break;  // Left only
            case 2: left = 0.0f; right = 1.0f; break;  // Right only
            default: left = 1.0f; right = 1.0f; break;  // Both
        }
    }

protected:
    int pan = 1; // Default: center (both speakers)
};
