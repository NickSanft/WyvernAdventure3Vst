#pragma once

#include "GBCChannel.h"
#include "GBCConstants.h"
#include <cstdint>
#include <array>

class WaveChannel : public GBCChannel
{
public:
    WaveChannel();

    void reset() override;
    void setSampleRate(double sampleRate) override;
    void noteOn(int period, float velocity) override;
    void noteOff() override;
    float processSample() override;
    bool isActive() const override;

    // --- Parameter setters ---

    // Volume code: 0 = mute, 1 = 100%, 2 = 50%, 3 = 25%
    void setVolumeCode(int code);

    // Set a single nibble in wave RAM (index 0-31, value 0-15)
    void setWaveNibble(int index, int value);

    // Load an entire 32-nibble waveform
    void loadWaveform(const uint8_t* nibbles32);

    // Load a preset waveform by name
    void loadPreset(int presetIndex);

    // Get current wave RAM as 32 nibbles (for display)
    void getWaveform(uint8_t* nibbles32) const;

    // Preset count
    static constexpr int NUM_PRESETS = 10;
    static const char* getPresetName(int index);

private:
    double hostSampleRate = 44100.0;

    // Wave RAM: 16 bytes = 32 nibbles (4-bit samples)
    uint8_t waveRam[16] = {};

    // Playback state
    int periodRegister = 0;
    double samplePointer = 0.0;   // Fractional position in 32-sample waveform
    double frequencyHz = 0.0;

    // Volume: 0=mute, 1=100%, 2=50%, 3=25%
    int volumeCode = 1;
    int volumeShift = 0;  // Derived: 0=mute(4), 1=shift0, 2=shift1, 3=shift2

    // State
    bool active = false;
    bool noteHeld = false;

    // Built-in presets
    static const uint8_t PRESET_TRIANGLE[32];
    static const uint8_t PRESET_SAWTOOTH[32];
    static const uint8_t PRESET_SQUARE[32];
    static const uint8_t PRESET_SINE[32];
    static const uint8_t PRESET_DW3_BASS[32];

    // Phase 2 additions
    static const uint8_t PRESET_HALF_SINE[32];
    static const uint8_t PRESET_PULSE25[32];
    static const uint8_t PRESET_DETUNED_SAW[32];
    static const uint8_t PRESET_BELL[32];
    static const uint8_t PRESET_ACID_BASS[32];
};
