#pragma once

#include <JuceHeader.h>

class GBCSynthProcessor;

// Manages built-in DW3 presets and parameter snapshot loading
class PresetManager
{
public:
    struct Preset
    {
        juce::String name;
        int channel;          // 0=Pulse1, 1=Pulse2, 2=Wave, 3=Noise
        int duty;             // 0-3
        int envInitVol;       // 0-15
        int envDir;           // 0=Down, 1=Up
        int envPeriod;        // 0-7
        int sweepPeriod;      // 0-7
        bool sweepNegate;
        int sweepShift;       // 0-7
        int waveVolume;       // 0-3
        int wavePreset;       // 0-4
        int noiseClockShift;  // 0-13
        int noiseDivisor;     // 0-7
        bool noiseWidth;      // 7-bit mode
        int noiseEnvInitVol;
        int noiseEnvDir;
        int noiseEnvPeriod;
        int pan;              // 0=L, 1=C, 2=R
        float masterVolume;

        // Modulation & mode (extended fields — added in Phase 1 preset expansion)
        int channelMode;      // 0=Single, 1=Stack
        bool vibratoOn;
        float vibratoRate;    // Hz (1.0 - 12.0)
        float vibratoDepth;   // cents (0 - 100)
        bool arpOn;
        float arpRate;        // Hz (2.0 - 32.0)
        int arpPattern;       // 0=Up, 1=Down, 2=Up-Down, 3=Random

        // ADSR envelope (Phase 3) — shared by Pulse1/2 and Wave channels
        float envAttack;      // ms
        float envDecay;       // ms
        int envSustain;       // 0-15
        float envRelease;     // ms

        // Separate ADSR for Noise channel
        float noiseAttack;
        float noiseDecay;
        int noiseSustain;
        float noiseRelease;
    };

    static const std::vector<Preset>& getPresets();
    static void applyPreset(juce::AudioProcessorValueTreeState& apvts, int presetIndex);
    static int getNumPresets();
    static juce::String getPresetName(int index);
};
