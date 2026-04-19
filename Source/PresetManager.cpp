#include "PresetManager.h"

// ============================================================================
// Preset definitions
//
// Field layout reminder (see PresetManager.h):
//   name, channel, duty, envInitVol, envDir, envPeriod,
//   sweepPeriod, sweepNegate, sweepShift,
//   waveVolume, wavePreset,
//   noiseClockShift, noiseDivisor, noiseWidth, noiseEnvInitVol, noiseEnvDir, noiseEnvPeriod,
//   pan, masterVolume,
//   channelMode, vibratoOn, vibratoRate, vibratoDepth, arpOn, arpRate, arpPattern
//
// Legend for condensed comments:
//   CH1 = Pulse 1, CH2 = Pulse 2, CH3 = Wave, CH4 = Noise
//   Duty: 0=12.5%, 1=25%, 2=50%, 3=75%
//   envDir: 0=Down, 1=Up ; Mode: 0=Single, 1=Stack
// ============================================================================

const std::vector<PresetManager::Preset>& PresetManager::getPresets()
{
    static const std::vector<Preset> presets = {
        // --------------------------------------------------------------
        // Pack 0 — Dragon Warrior 3 originals
        // --------------------------------------------------------------

        // Field Theme — CH1 50% duty, moderate decay
        { "Field Theme",
          0, 2, 12, 0, 3, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 },

        // Battle Theme — CH1 25% duty, tight staccato (no sweep)
        { "Battle Theme",
          0, 1, 15, 0, 2, 0, false, 0,
          1, 4, 0, 0, false, 15, 0, 0,
          1, 0.85f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 },

        // Town Theme — CH1 25% duty, soft & gentle
        { "Town Theme",
          0, 1, 10, 0, 4, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.65f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 },

        // Dungeon Theme — CH3 wave DW3 bass, brooding
        { "Dungeon Theme",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 4, 0, 0, false, 15, 0, 0,
          1, 0.70f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 },

        // Percussion Hit — CH4 7-bit narrow, short snappy metallic
        { "Percussion Hit",
          3, 2, 15, 0, 0, 0, false, 0,
          1, 0, 2, 0, true, 15, 0, 2,
          1, 0.80f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 },

        // White Noise SFX — CH4 15-bit wide, slow fade
        { "White Noise SFX",
          3, 2, 15, 0, 0, 0, false, 0,
          1, 0, 4, 1, false, 13, 0, 5,
          1, 0.60f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 },

        // --------------------------------------------------------------
        // Pack A — NES Classic
        // --------------------------------------------------------------

        // NES Mario Lead — CH1 50%, staccato bouncy
        { "NES Mario Lead",
          0, 2, 14, 0, 3, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 },

        // NES Zelda Overworld — CH1 50%, smooth with gentle vib
        { "NES Zelda Overworld",
          0, 2, 13, 0, 4, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, true, 5.0f, 20.0f, false, 8.0f, 0 },

        // NES Mega Man Lead — CH2 25%, bright & aggressive
        { "NES Mega Man Lead",
          1, 1, 15, 0, 2, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.80f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 },

        // NES Castlevania — CH1 12.5%, thin nasal with deep vibrato
        { "NES Castlevania",
          0, 0, 12, 0, 5, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, true, 6.0f, 40.0f, false, 8.0f, 0 },

        // NES Triangle Bass — CH3 triangle, sustained bass
        { "NES Triangle Bass",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.80f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 },

        // NES Mega Man Bass — CH3 triangle with octave arp
        { "NES Mega Man Bass",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.80f,
          0, false, 6.0f, 25.0f, true, 6.0f, 0 },

        // NES Snare — CH4 7-bit narrow, snappy
        { "NES Snare",
          3, 2, 15, 0, 0, 0, false, 0,
          1, 0, 1, 0, true, 14, 0, 1,
          1, 0.75f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 },

        // NES Hi-Hat — CH4 15-bit wide, higher pitch
        { "NES Hi-Hat",
          3, 2, 15, 0, 0, 0, false, 0,
          1, 0, 6, 1, false, 8, 0, 1,
          1, 0.60f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 },

        // --------------------------------------------------------------
        // Pack C — Arcade SFX (10)
        // --------------------------------------------------------------

        // Coin Pickup — CH1 25%, fast decay, rising arp
        { "Coin Pickup",
          0, 1, 15, 0, 2, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.80f,
          0, false, 6.0f, 25.0f, true, 24.0f, 0 },

        // Jump — CH1 50%, short envelope
        { "Jump",
          0, 2, 15, 0, 2, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 },

        // Laser Shot — CH1 12.5%, downward sweep, very fast decay
        { "Laser Shot",
          0, 0, 15, 0, 1, 3, true, 3,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.80f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 },

        // Power-Up — CH2 25%, fast up arp
        { "Power-Up",
          1, 1, 15, 0, 2, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.80f,
          0, false, 6.0f, 25.0f, true, 20.0f, 0 },

        // 1-Up Jingle — Stack mode with arp up
        { "1-Up Jingle",
          1, 2, 14, 0, 2, 0, false, 0,
          1, 0, 0, 0, false, 12, 0, 1,
          1, 0.75f,
          1, false, 6.0f, 25.0f, true, 16.0f, 0 },

        // Explosion — CH4 wide, long decay
        { "Explosion",
          3, 2, 15, 0, 0, 0, false, 0,
          1, 0, 2, 0, false, 15, 0, 5,
          1, 0.85f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 },

        // Hit / Bump — CH4 narrow, instant
        { "Hit / Bump",
          3, 2, 15, 0, 0, 0, false, 0,
          1, 0, 4, 0, true, 10, 0, 0,
          1, 0.75f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 },

        // Alarm — CH1 50%, extreme vibrato, no decay
        { "Alarm",
          0, 2, 14, 0, 0, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.70f,
          0, true, 8.0f, 80.0f, false, 8.0f, 0 },

        // Menu Select — CH2 50%, clean click
        { "Menu Select",
          1, 2, 12, 0, 1, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.70f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 },

        // Menu Error — CH1 25%, downward sweep
        { "Menu Error",
          0, 1, 14, 0, 2, 1, true, 2,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 },

        // --------------------------------------------------------------
        // Pack D — Chiptune Instruments (7, Pad requires ADSR — skipped)
        // --------------------------------------------------------------

        // 8-Bit Square Lead — CH1 50%, long sustain
        { "8-Bit Square Lead",
          0, 2, 15, 0, 6, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.80f,
          0, true, 5.5f, 15.0f, false, 8.0f, 0 },

        // Hollow Lead — CH1 12.5%, thin whistle
        { "Hollow Lead",
          0, 0, 13, 0, 5, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 },

        // Fat Lead (Stack) — stack mode with all channels layered
        { "Fat Lead (Stack)",
          0, 2, 13, 0, 5, 0, false, 0,
          1, 1, 0, 0, false, 12, 0, 1,
          1, 0.70f,
          1, true, 5.0f, 18.0f, false, 8.0f, 0 },

        // Bell Pluck — CH3 DW3 Bass with fast decay (via noteOff behavior)
        { "Bell Pluck",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 4, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 },

        // Soft Triangle Bass — CH3 triangle
        { "Soft Triangle Bass",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 },

        // Sawtooth Bass — CH3 sawtooth, gritty
        { "Sawtooth Bass",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 1, 0, 0, false, 15, 0, 0,
          1, 0.80f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 },

        // Arp Melody — CH2 25% with up-down arp
        { "Arp Melody",
          1, 1, 14, 0, 4, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, false, 6.0f, 25.0f, true, 12.0f, 2 },

        // --------------------------------------------------------------
        // Pack E — DW3 Expansion (5)
        // --------------------------------------------------------------

        // DW3 Overworld Reprise — CH2 25%, gentle vibrato
        { "DW3 Overworld Reprise",
          1, 1, 13, 0, 4, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.70f,
          0, true, 4.5f, 18.0f, false, 8.0f, 0 },

        // DW3 Cave Echo — CH3 triangle, slow decay
        { "DW3 Cave Echo",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          2, 0.65f,
          0, true, 3.0f, 20.0f, false, 8.0f, 0 },

        // DW3 Boss Battle — Stack mode, aggressive
        { "DW3 Boss Battle",
          0, 1, 15, 0, 2, 0, false, 0,
          1, 4, 3, 0, true, 14, 0, 1,
          1, 0.85f,
          1, true, 6.5f, 30.0f, false, 8.0f, 0 },

        // DW3 Victory Fanfare — CH1 50% with up-arp, stack
        { "DW3 Victory Fanfare",
          0, 2, 15, 0, 3, 0, false, 0,
          1, 0, 0, 0, false, 14, 0, 2,
          1, 0.80f,
          1, false, 6.0f, 25.0f, true, 14.0f, 0 },

        // DW3 Game Over — CH3 DW3 bass, very slow decay
        { "DW3 Game Over",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 4, 0, 0, false, 15, 0, 0,
          1, 0.70f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 },

        // --------------------------------------------------------------
        // Pack F — Showcase of new waveforms (Phase 2)
        // --------------------------------------------------------------

        // Organ Lead — CH3 Half-Sine, warm sustained organ tone with slow vibrato
        { "Organ Lead",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 5,  // wavePreset 5 = Half-Sine
          0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, true, 4.5f, 15.0f, false, 8.0f, 0 },

        // Acid Bass Lead — CH3 Acid Bass waveform, gritty and resonant
        { "Acid Bass Lead",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 9,  // wavePreset 9 = Acid Bass
          0, 0, false, 15, 0, 0,
          1, 0.80f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 },

        // Bell Arp — CH3 Bell waveform with up-arp for glockenspiel runs
        { "Bell Arp",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 8,  // wavePreset 8 = Bell
          0, 0, false, 15, 0, 0,
          1, 0.70f,
          0, false, 6.0f, 25.0f, true, 14.0f, 0 },

        // Thick Lead — CH3 Detuned Saw, chorus-like stacked lead
        { "Thick Lead",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 7,  // wavePreset 7 = Detuned Saw
          0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, true, 5.5f, 20.0f, false, 8.0f, 0 },

        // Wave Pulse Lead — CH3 Pulse 25%, bright & cutting
        { "Wave Pulse Lead",
          2, 1, 15, 0, 0, 0, false, 0,
          1, 6,  // wavePreset 6 = Pulse 25%
          0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0 }
    };
    return presets;
}

void PresetManager::applyPreset(juce::AudioProcessorValueTreeState& apvts, int presetIndex)
{
    const auto& presets = getPresets();
    if (presetIndex < 0 || presetIndex >= static_cast<int>(presets.size()))
        return;

    const auto& p = presets[presetIndex];

    auto setParam = [&](const juce::String& id, float value)
    {
        if (auto* param = apvts.getParameter(id))
            param->setValueNotifyingHost(param->convertTo0to1(value));
    };

    // Core channel & envelope
    setParam("channelSelect", float(p.channel));
    setParam("duty", float(p.duty));
    setParam("envInitVol", float(p.envInitVol));
    setParam("envDir", float(p.envDir));
    setParam("envPeriod", float(p.envPeriod));

    // Sweep
    setParam("sweepPeriod", float(p.sweepPeriod));
    setParam("sweepNegate", p.sweepNegate ? 1.0f : 0.0f);
    setParam("sweepShift", float(p.sweepShift));

    // Wave channel
    setParam("waveVolume", float(p.waveVolume));
    setParam("wavePreset", float(p.wavePreset));

    // Noise channel
    setParam("noiseClockShift", float(p.noiseClockShift));
    setParam("noiseDivisor", float(p.noiseDivisor));
    setParam("noiseWidth", p.noiseWidth ? 1.0f : 0.0f);
    setParam("noiseEnvInitVol", float(p.noiseEnvInitVol));
    setParam("noiseEnvDir", float(p.noiseEnvDir));
    setParam("noiseEnvPeriod", float(p.noiseEnvPeriod));

    // Output
    setParam("pan", float(p.pan));
    setParam("masterVolume", p.masterVolume);

    // Channel mode + modulation (extended fields)
    setParam("channelMode", float(p.channelMode));
    setParam("vibratoOn", p.vibratoOn ? 1.0f : 0.0f);
    setParam("vibratoRate", p.vibratoRate);
    setParam("vibratoDepth", p.vibratoDepth);
    setParam("arpOn", p.arpOn ? 1.0f : 0.0f);
    setParam("arpRate", p.arpRate);
    setParam("arpPattern", float(p.arpPattern));
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
