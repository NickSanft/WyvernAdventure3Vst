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
    };

    static const std::vector<Preset>& getPresets();
    static void applyPreset(juce::AudioProcessorValueTreeState& apvts, int presetIndex);
    static int getNumPresets();
    static juce::String getPresetName(int index);
};
