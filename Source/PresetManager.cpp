#include "PresetManager.h"

const std::vector<PresetManager::Preset>& PresetManager::getPresets()
{
    static const std::vector<Preset> presets = {
        // Field Theme — CH1 50% duty, moderate envelope decay, melodic
        {
            "Field Theme",
            0,        // Pulse 1
            2,        // 50% duty
            12,       // envInitVol
            0,        // envDir: Down
            3,        // envPeriod (moderate decay)
            0,        // sweepPeriod (off)
            false,    // sweepNegate
            0,        // sweepShift
            1,        // waveVolume: 100%
            0,        // wavePreset: Triangle
            0, 0, false, 15, 0, 0,  // noise defaults
            1,        // pan: Center
            0.75f     // masterVolume
        },
        // Battle Theme — CH1 fast duty, aggressive, with sweep
        {
            "Battle Theme",
            0,        // Pulse 1
            1,        // 25% duty (brighter, more aggressive)
            15,       // envInitVol: max
            0,        // envDir: Down
            1,        // envPeriod: fast decay
            2,        // sweepPeriod: active
            false,    // sweepNegate
            1,        // sweepShift: subtle pitch rise
            1,        // waveVolume: 100%
            4,        // wavePreset: DW3 Bass
            0, 0, false, 15, 0, 0,
            1,        // pan: Center
            0.85f     // masterVolume
        },
        // Town Theme — light pulse melody, 25% duty
        {
            "Town Theme",
            0,        // Pulse 1
            1,        // 25% duty (softer)
            10,       // envInitVol: moderate
            0,        // envDir: Down
            4,        // envPeriod: slow gentle decay
            0,        // no sweep
            false, 0,
            1, 0,     // wave defaults
            0, 0, false, 15, 0, 0,
            1,        // pan: Center
            0.65f     // masterVolume: softer
        },
        // Dungeon Theme — Wave channel with DW3 bass, darker feel
        {
            "Dungeon Theme",
            2,        // Wave channel
            2,        // duty defaults
            15, 0, 0, // env defaults
            0, false, 0,
            1,        // waveVolume: 100%
            4,        // wavePreset: DW3 Bass
            0, 0, false, 15, 0, 0,
            1,        // pan: Center
            0.70f
        },
        // Percussion Hit — Noise channel, short 7-bit metallic
        {
            "Percussion Hit",
            3,        // Noise
            2,        // duty defaults
            15, 0, 0,
            0, false, 0,
            1, 0,
            2,        // noiseClockShift: mid-range
            0,        // noiseDivisor: base
            true,     // noiseWidth: 7-bit (metallic)
            15,       // noiseEnvInitVol: max
            0,        // noiseEnvDir: Down
            2,        // noiseEnvPeriod: short snappy decay
            1,        // pan: Center
            0.80f
        },
        // White Noise SFX — CH4, 15-bit wide noise, longer
        {
            "White Noise SFX",
            3,        // Noise
            2, 15, 0, 0, 0, false, 0, 1, 0,
            4,        // noiseClockShift: higher
            1,        // noiseDivisor
            false,    // 15-bit wide mode
            13,       // noiseEnvInitVol: moderate
            0,        // noiseEnvDir: Down
            5,        // noiseEnvPeriod: slow fade
            1,
            0.60f
        }
    };
    return presets;
}

void PresetManager::applyPreset(juce::AudioProcessorValueTreeState& apvts, int presetIndex)
{
    const auto& presets = getPresets();
    if (presetIndex < 0 || presetIndex >= static_cast<int>(presets.size()))
        return;

    const auto& p = presets[presetIndex];

    auto setParam = [&](const juce::String& id, float value) {
        if (auto* param = apvts.getParameter(id))
            param->setValueNotifyingHost(param->convertTo0to1(value));
    };

    setParam("channelSelect", float(p.channel));
    setParam("duty", float(p.duty));
    setParam("envInitVol", float(p.envInitVol));
    setParam("envDir", float(p.envDir));
    setParam("envPeriod", float(p.envPeriod));
    setParam("sweepPeriod", float(p.sweepPeriod));
    setParam("sweepNegate", p.sweepNegate ? 1.0f : 0.0f);
    setParam("sweepShift", float(p.sweepShift));
    setParam("waveVolume", float(p.waveVolume));
    setParam("wavePreset", float(p.wavePreset));
    setParam("noiseClockShift", float(p.noiseClockShift));
    setParam("noiseDivisor", float(p.noiseDivisor));
    setParam("noiseWidth", p.noiseWidth ? 1.0f : 0.0f);
    setParam("noiseEnvInitVol", float(p.noiseEnvInitVol));
    setParam("noiseEnvDir", float(p.noiseEnvDir));
    setParam("noiseEnvPeriod", float(p.noiseEnvPeriod));
    setParam("pan", float(p.pan));
    setParam("masterVolume", p.masterVolume);
}

int PresetManager::getNumPresets()
{
    return static_cast<int>(getPresets().size());
}

juce::String PresetManager::getPresetName(int index)
{
    const auto& presets = getPresets();
    if (index >= 0 && index < static_cast<int>(presets.size()))
        return presets[index].name;
    return "Unknown";
}
