#include "PresetManager.h"

// ============================================================================
// Preset definitions
//
// Field layout (see PresetManager.h for full struct):
//   core:   name, channel, duty, envInitVol, envDir(legacy), envPeriod(legacy),
//           sweepPeriod, sweepNegate, sweepShift,
//           waveVolume, wavePreset,
//           noiseClockShift, noiseDivisor, noiseWidth,
//           noiseEnvInitVol, noiseEnvDir(legacy), noiseEnvPeriod(legacy),
//           pan, masterVolume,
//   mod:    channelMode, vibratoOn, vibratoRate, vibratoDepth,
//           arpOn, arpRate, arpPattern,
//   ADSR:   envAttack, envDecay, envSustain, envRelease,
//           noiseAttack, noiseDecay, noiseSustain, noiseRelease
//
// Legacy envDir/envPeriod are kept for saved-state compatibility but no longer
// drive the DSP — envAttack/Decay/Sustain/Release do.
// ============================================================================

const std::vector<PresetManager::Preset>& PresetManager::getPresets()
{
    static const std::vector<Preset> presets = {
        // --------------------------------------------------------------
        // Pack 0 — Dragon Warrior 3 originals (migrated to ADSR)
        // --------------------------------------------------------------

        // Field Theme
        { "Field Theme",
          0, 2, 12, 0, 3, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          5.0f, 700.0f, 0, 50.0f,     5.0f, 150.0f, 0, 50.0f },

        // Battle Theme
        { "Battle Theme",
          0, 1, 15, 0, 2, 0, false, 0,
          1, 4, 0, 0, false, 15, 0, 0,
          1, 0.85f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          0.0f, 470.0f, 0, 30.0f,     0.0f, 150.0f, 0, 50.0f },

        // Town Theme
        { "Town Theme",
          0, 1, 10, 0, 4, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.65f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          10.0f, 940.0f, 0, 80.0f,    0.0f, 150.0f, 0, 50.0f },

        // Dungeon Theme
        { "Dungeon Theme",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 4, 0, 0, false, 15, 0, 0,
          1, 0.70f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          20.0f, 0.0f, 15, 300.0f,    0.0f, 150.0f, 0, 50.0f },

        // Percussion Hit — noise channel so ADSR for noise matters
        { "Percussion Hit",
          3, 2, 15, 0, 0, 0, false, 0,
          1, 0, 2, 0, true, 15, 0, 2,
          1, 0.80f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          0.0f, 200.0f, 0, 20.0f,     0.0f, 60.0f, 0, 10.0f },

        // White Noise SFX
        { "White Noise SFX",
          3, 2, 15, 0, 0, 0, false, 0,
          1, 0, 4, 1, false, 13, 0, 5,
          1, 0.60f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          0.0f, 200.0f, 0, 20.0f,     50.0f, 1200.0f, 0, 200.0f },

        // --------------------------------------------------------------
        // Pack A — NES Classic
        // --------------------------------------------------------------

        { "NES Mario Lead",
          0, 2, 14, 0, 3, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          3.0f, 500.0f, 8, 80.0f,     0.0f, 150.0f, 0, 50.0f },

        { "NES Zelda Overworld",
          0, 2, 13, 0, 4, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, true, 5.0f, 20.0f, false, 8.0f, 0,
          8.0f, 300.0f, 11, 200.0f,   0.0f, 150.0f, 0, 50.0f },

        { "NES Mega Man Lead",
          1, 1, 15, 0, 2, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.80f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          2.0f, 300.0f, 0, 40.0f,     0.0f, 150.0f, 0, 50.0f },

        { "NES Castlevania",
          0, 0, 12, 0, 5, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, true, 6.0f, 40.0f, false, 8.0f, 0,
          15.0f, 0.0f, 12, 400.0f,    0.0f, 150.0f, 0, 50.0f },

        { "NES Triangle Bass",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.80f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          0.0f, 0.0f, 15, 30.0f,      0.0f, 150.0f, 0, 50.0f },

        { "NES Mega Man Bass",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.80f,
          0, false, 6.0f, 25.0f, true, 6.0f, 0,
          0.0f, 0.0f, 15, 30.0f,      0.0f, 150.0f, 0, 50.0f },

        { "NES Snare",
          3, 2, 15, 0, 0, 0, false, 0,
          1, 0, 1, 0, true, 14, 0, 1,
          1, 0.75f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          0.0f, 200.0f, 0, 20.0f,     0.0f, 100.0f, 0, 5.0f },

        { "NES Hi-Hat",
          3, 2, 15, 0, 0, 0, false, 0,
          1, 0, 6, 1, false, 8, 0, 1,
          1, 0.60f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          0.0f, 200.0f, 0, 20.0f,     0.0f, 80.0f, 0, 5.0f },

        // --------------------------------------------------------------
        // Pack C — Arcade SFX
        // --------------------------------------------------------------

        { "Coin Pickup",
          0, 1, 15, 0, 2, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.80f,
          0, false, 6.0f, 25.0f, true, 24.0f, 0,
          0.0f, 150.0f, 0, 20.0f,     0.0f, 150.0f, 0, 50.0f },

        { "Jump",
          0, 2, 15, 0, 2, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          0.0f, 300.0f, 0, 30.0f,     0.0f, 150.0f, 0, 50.0f },

        { "Laser Shot",
          0, 0, 15, 0, 1, 3, true, 3,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.80f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          0.0f, 100.0f, 0, 10.0f,     0.0f, 150.0f, 0, 50.0f },

        { "Power-Up",
          1, 1, 15, 0, 2, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.80f,
          0, false, 6.0f, 25.0f, true, 20.0f, 0,
          0.0f, 200.0f, 0, 20.0f,     0.0f, 150.0f, 0, 50.0f },

        { "1-Up Jingle",
          1, 2, 14, 0, 2, 0, false, 0,
          1, 0, 0, 0, false, 12, 0, 1,
          1, 0.75f,
          1, false, 6.0f, 25.0f, true, 16.0f, 0,
          0.0f, 200.0f, 0, 30.0f,     0.0f, 150.0f, 0, 50.0f },

        { "Explosion",
          3, 2, 15, 0, 0, 0, false, 0,
          1, 0, 2, 0, false, 15, 0, 5,
          1, 0.85f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          0.0f, 200.0f, 0, 20.0f,     0.0f, 1200.0f, 0, 200.0f },

        { "Hit / Bump",
          3, 2, 15, 0, 0, 0, false, 0,
          1, 0, 4, 0, true, 10, 0, 0,
          1, 0.75f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          0.0f, 200.0f, 0, 20.0f,     0.0f, 40.0f, 0, 5.0f },

        { "Alarm",
          0, 2, 14, 0, 0, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.70f,
          0, true, 8.0f, 80.0f, false, 8.0f, 0,
          5.0f, 0.0f, 14, 50.0f,      0.0f, 150.0f, 0, 50.0f },

        { "Menu Select",
          1, 2, 12, 0, 1, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.70f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          0.0f, 80.0f, 0, 10.0f,      0.0f, 150.0f, 0, 50.0f },

        { "Menu Error",
          0, 1, 14, 0, 2, 1, true, 2,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          0.0f, 250.0f, 0, 20.0f,     0.0f, 150.0f, 0, 50.0f },

        // --------------------------------------------------------------
        // Pack D — Chiptune Instruments
        // --------------------------------------------------------------

        { "8-Bit Square Lead",
          0, 2, 15, 0, 6, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.80f,
          0, true, 5.5f, 15.0f, false, 8.0f, 0,
          5.0f, 100.0f, 12, 150.0f,   0.0f, 150.0f, 0, 50.0f },

        { "Hollow Lead",
          0, 0, 13, 0, 5, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          8.0f, 200.0f, 10, 150.0f,   0.0f, 150.0f, 0, 50.0f },

        { "Fat Lead (Stack)",
          0, 2, 13, 0, 5, 0, false, 0,
          1, 1, 0, 0, false, 12, 0, 1,
          1, 0.70f,
          1, true, 5.0f, 18.0f, false, 8.0f, 0,
          10.0f, 200.0f, 11, 200.0f,  0.0f, 200.0f, 0, 50.0f },

        { "Bell Pluck",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 4, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          0.0f, 400.0f, 0, 50.0f,     0.0f, 150.0f, 0, 50.0f },

        { "Soft Triangle Bass",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          0.0f, 0.0f, 15, 40.0f,      0.0f, 150.0f, 0, 50.0f },

        { "Sawtooth Bass",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 1, 0, 0, false, 15, 0, 0,
          1, 0.80f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          0.0f, 0.0f, 15, 40.0f,      0.0f, 150.0f, 0, 50.0f },

        { "Arp Melody",
          1, 1, 14, 0, 4, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, false, 6.0f, 25.0f, true, 12.0f, 2,
          0.0f, 120.0f, 0, 30.0f,     0.0f, 150.0f, 0, 50.0f },

        // Pad (NEW — needs sustain > 0 which ADSR now enables)
        { "Chiptune Pad (Stack)",
          2, 2, 13, 0, 0, 0, false, 0,
          1, 5, 0, 0, false, 10, 0, 2,
          1, 0.55f,
          1, true, 3.5f, 15.0f, false, 8.0f, 0,
          400.0f, 300.0f, 10, 800.0f, 200.0f, 200.0f, 8, 400.0f },

        // --------------------------------------------------------------
        // Pack E — DW3 Expansion
        // --------------------------------------------------------------

        { "DW3 Overworld Reprise",
          1, 1, 13, 0, 4, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.70f,
          0, true, 4.5f, 18.0f, false, 8.0f, 0,
          10.0f, 300.0f, 11, 200.0f,  0.0f, 150.0f, 0, 50.0f },

        { "DW3 Cave Echo",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          2, 0.65f,
          0, true, 3.0f, 20.0f, false, 8.0f, 0,
          30.0f, 200.0f, 13, 500.0f,  0.0f, 150.0f, 0, 50.0f },

        { "DW3 Boss Battle",
          0, 1, 15, 0, 2, 0, false, 0,
          1, 4, 3, 0, true, 14, 0, 1,
          1, 0.85f,
          1, true, 6.5f, 30.0f, false, 8.0f, 0,
          0.0f, 400.0f, 0, 30.0f,     0.0f, 80.0f, 0, 10.0f },

        { "DW3 Victory Fanfare",
          0, 2, 15, 0, 3, 0, false, 0,
          1, 0, 0, 0, false, 14, 0, 2,
          1, 0.80f,
          1, false, 6.0f, 25.0f, true, 14.0f, 0,
          0.0f, 700.0f, 0, 100.0f,    0.0f, 400.0f, 0, 50.0f },

        { "DW3 Game Over",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 4, 0, 0, false, 15, 0, 0,
          1, 0.70f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          15.0f, 0.0f, 14, 600.0f,    0.0f, 150.0f, 0, 50.0f },

        // --------------------------------------------------------------
        // Pack F — Waveform showcase (Phase 2)
        // --------------------------------------------------------------

        { "Organ Lead",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 5, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, true, 4.5f, 15.0f, false, 8.0f, 0,
          30.0f, 0.0f, 15, 150.0f,    0.0f, 150.0f, 0, 50.0f },

        { "Acid Bass Lead",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 9, 0, 0, false, 15, 0, 0,
          1, 0.80f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          0.0f, 300.0f, 8, 80.0f,     0.0f, 150.0f, 0, 50.0f },

        { "Bell Arp",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 8, 0, 0, false, 15, 0, 0,
          1, 0.70f,
          0, false, 6.0f, 25.0f, true, 14.0f, 0,
          0.0f, 300.0f, 0, 50.0f,     0.0f, 150.0f, 0, 50.0f },

        { "Thick Lead",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 7, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, true, 5.5f, 20.0f, false, 8.0f, 0,
          5.0f, 200.0f, 12, 150.0f,   0.0f, 150.0f, 0, 50.0f },

        { "Wave Pulse Lead",
          2, 1, 15, 0, 0, 0, false, 0,
          1, 6, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          0.0f, 400.0f, 6, 80.0f,     0.0f, 150.0f, 0, 50.0f },

        // --------------------------------------------------------------
        // Pack B — SNES RPG (Phase 3) — real sustained timbres enabled by ADSR
        // --------------------------------------------------------------

        // SNES Flute — soft attack, long sustain, slow release, gentle vibrato
        { "SNES Flute",
          2, 2, 13, 0, 0, 0, false, 0,
          2, 3, 0, 0, false, 15, 0, 0,   // waveVolume=50% to mellow it
          1, 0.70f,
          0, true, 5.0f, 25.0f, false, 8.0f, 0,
          80.0f, 200.0f, 12, 500.0f,  0.0f, 150.0f, 0, 50.0f },

        // SNES Brass — fast attack swell, strong sustain, short release
        { "SNES Brass",
          2, 2, 14, 0, 0, 0, false, 0,
          1, 1, 0, 0, false, 15, 0, 0,   // Sawtooth wave for brassy edge
          1, 0.75f,
          0, true, 4.0f, 15.0f, false, 8.0f, 0,
          30.0f, 150.0f, 13, 300.0f,  0.0f, 150.0f, 0, 50.0f },

        // SNES Choir Pad — slow swelling pad
        { "SNES Choir Pad",
          2, 2, 12, 0, 0, 0, false, 0,
          2, 3, 0, 0, false, 15, 0, 0,   // Sine at 50%
          1, 0.55f,
          1, true, 4.0f, 30.0f, false, 8.0f, 0,  // Stack for richness
          600.0f, 500.0f, 11, 1000.0f,  0.0f, 150.0f, 0, 50.0f },

        // SNES Harp Pluck — quick attack, long release, triangle
        { "SNES Harp Pluck",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 0, 0, 0, false, 15, 0, 0,
          1, 0.75f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          0.0f, 600.0f, 0, 400.0f,    0.0f, 150.0f, 0, 50.0f },

        // SNES Music Box — bell with gentle vibrato, long decay
        { "SNES Music Box",
          2, 2, 13, 0, 0, 0, false, 0,
          3, 8, 0, 0, false, 15, 0, 0,   // Bell wave at 25%
          1, 0.65f,
          0, true, 7.0f, 10.0f, false, 8.0f, 0,
          0.0f, 400.0f, 2, 600.0f,    0.0f, 150.0f, 0, 50.0f },

        // SNES Bass Pluck — DW3 Bass with fast decay
        { "SNES Bass Pluck",
          2, 2, 15, 0, 0, 0, false, 0,
          1, 4, 0, 0, false, 15, 0, 0,
          1, 0.80f,
          0, false, 6.0f, 25.0f, false, 8.0f, 0,
          0.0f, 500.0f, 4, 100.0f,    0.0f, 150.0f, 0, 50.0f }
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

    // Core channel & envelope (peak level)
    setParam("channelSelect", float(p.channel));
    setParam("duty", float(p.duty));
    setParam("envInitVol", float(p.envInitVol));
    setParam("envDir", float(p.envDir));         // legacy — kept in state
    setParam("envPeriod", float(p.envPeriod));   // legacy

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
    setParam("noiseEnvDir", float(p.noiseEnvDir));         // legacy
    setParam("noiseEnvPeriod", float(p.noiseEnvPeriod));   // legacy

    // Output
    setParam("pan", float(p.pan));
    setParam("masterVolume", p.masterVolume);

    // Channel mode + modulation
    setParam("channelMode", float(p.channelMode));
    setParam("vibratoOn", p.vibratoOn ? 1.0f : 0.0f);
    setParam("vibratoRate", p.vibratoRate);
    setParam("vibratoDepth", p.vibratoDepth);
    setParam("arpOn", p.arpOn ? 1.0f : 0.0f);
    setParam("arpRate", p.arpRate);
    setParam("arpPattern", float(p.arpPattern));

    // ADSR envelopes (Phase 3)
    setParam("envAttack",  p.envAttack);
    setParam("envDecay",   p.envDecay);
    setParam("envSustain", float(p.envSustain));
    setParam("envRelease", p.envRelease);
    setParam("noiseAttack",  p.noiseAttack);
    setParam("noiseDecay",   p.noiseDecay);
    setParam("noiseSustain", float(p.noiseSustain));
    setParam("noiseRelease", p.noiseRelease);
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
