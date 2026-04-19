#pragma once

#include <algorithm>
#include <cmath>

// Four-stage envelope generator: Attack -> Decay -> Sustain -> Release.
// All levels are in the 0..15 range (GBC 4-bit volume convention).
//
// Usage:
//   ADSR env;
//   env.setSampleRate(44100.0);
//   env.setPeakLevel(15.0f);
//   env.setParams(5.0f, 120.0f, 10.0f, 300.0f);  // attackMs, decayMs, sustainLevel, releaseMs
//   env.noteOn();
//   float lvl = env.tick();  // call once per sample
//   // ... later ...
//   env.noteOff();           // triggers release
class GBCEnvelope
{
public:
    enum Stage { Idle, Attack, Decay, Sustain, Release };

    void setSampleRate(double sr) noexcept
    {
        sampleRate = sr;
        recalcDeltas();
    }

    void setPeakLevel(float peak) noexcept
    {
        peakLevel = std::clamp(peak, 0.0f, 15.0f);
        recalcDeltas();
    }

    // attackMs: 0..5000ms, decayMs: 0..5000ms, sustainLevel: 0..15, releaseMs: 0..5000ms
    void setParams(float attackMs, float decayMs, float sustainLvl, float releaseMs) noexcept
    {
        attackMsCached = std::max(0.0f, attackMs);
        decayMsCached = std::max(0.0f, decayMs);
        sustainLevel = std::clamp(sustainLvl, 0.0f, 15.0f);
        releaseMsCached = std::max(0.0f, releaseMs);
        recalcDeltas();
    }

    void noteOn() noexcept
    {
        stage = Attack;
        // Don't zero level — starting from current value allows smooth legato
    }

    void noteOff() noexcept
    {
        if (stage == Idle || stage == Release)
            return;

        stage = Release;
        // Release delta is proportional to current level so release time is
        // consistent regardless of where in the envelope we were when released
        float releaseSamples = std::max(1.0f, releaseMsCached * 0.001f * static_cast<float>(sampleRate));
        releaseDelta = level / releaseSamples;
    }

    // Advance one sample and return current level (0..peakLevel)
    float tick() noexcept
    {
        switch (stage)
        {
            case Attack:
                level += attackDelta;
                if (level >= peakLevel)
                {
                    level = peakLevel;
                    stage = Decay;
                }
                break;

            case Decay:
                level -= decayDelta;
                if (level <= sustainLevel)
                {
                    level = sustainLevel;
                    stage = Sustain;
                }
                break;

            case Sustain:
                level = sustainLevel;
                // If sustain level is effectively zero, auto-end — this matches
                // classic "decay only" envelope behavior
                if (sustainLevel <= 0.001f)
                {
                    level = 0.0f;
                    stage = Idle;
                }
                break;

            case Release:
                level -= releaseDelta;
                if (level <= 0.0f)
                {
                    level = 0.0f;
                    stage = Idle;
                }
                break;

            case Idle:
            default:
                level = 0.0f;
                break;
        }
        return level;
    }

    bool isActive() const noexcept { return stage != Idle; }

    void reset() noexcept
    {
        stage = Idle;
        level = 0.0f;
    }

private:
    void recalcDeltas() noexcept
    {
        float attackSamples = std::max(1.0f, attackMsCached * 0.001f * static_cast<float>(sampleRate));
        attackDelta = peakLevel / attackSamples;

        float decaySamples = std::max(1.0f, decayMsCached * 0.001f * static_cast<float>(sampleRate));
        decayDelta = (peakLevel - sustainLevel) / decaySamples;
        if (decayDelta < 0.0f) decayDelta = 0.0f;  // If sustain >= peak, no decay
    }

    Stage stage = Idle;
    float level = 0.0f;

    float peakLevel = 15.0f;
    float sustainLevel = 0.0f;

    float attackDelta = 100.0f;
    float decayDelta = 100.0f;
    float releaseDelta = 100.0f;

    float attackMsCached = 0.0f;
    float decayMsCached = 200.0f;
    float releaseMsCached = 50.0f;

    double sampleRate = 44100.0;
};
