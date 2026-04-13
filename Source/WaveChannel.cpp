#include "WaveChannel.h"
#include <algorithm>
#include <cmath>

// --- Built-in waveform presets (32 nibble values, 0-15) ---

const uint8_t WaveChannel::PRESET_TRIANGLE[32] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
};

const uint8_t WaveChannel::PRESET_SAWTOOTH[32] = {
    0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 6, 7, 7, 8,
    8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 15
};

const uint8_t WaveChannel::PRESET_SQUARE[32] = {
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Sine approximation using 4-bit quantization
const uint8_t WaveChannel::PRESET_SINE[32] = {
    8, 9, 11, 12, 13, 14, 14, 15, 15, 15, 14, 14, 13, 12, 11, 9,
    8, 6, 4, 3, 2, 1, 1, 0, 0, 0, 1, 1, 2, 3, 4, 6
};

// Dragon Warrior 3 bass: descending sawtooth (as described in spec)
const uint8_t WaveChannel::PRESET_DW3_BASS[32] = {
    15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
    15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
};

WaveChannel::WaveChannel()
{
    reset();
}

void WaveChannel::reset()
{
    periodRegister = 0;
    samplePointer = 0.0;
    frequencyHz = 0.0;
    volumeCode = 1;
    volumeShift = 0;
    active = false;
    noteHeld = false;

    // Load triangle as default waveform
    loadPreset(0);
}

void WaveChannel::setSampleRate(double sampleRate)
{
    hostSampleRate = sampleRate;
}

void WaveChannel::noteOn(int period, float /*velocity*/)
{
    periodRegister = std::clamp(period, 0, 2047);

    // Wave channel frequency: the 32-sample waveform plays at
    // GBC_CLOCK_HZ / (2 * (2048 - period)) Hz for raw sample rate,
    // which means the full 32-sample cycle frequency is:
    // GBC_CLOCK_HZ / (2 * (2048 - period) * 32)
    double rawRate = waveFrequency(periodRegister);  // This is the per-sample rate
    frequencyHz = rawRate / 32.0;  // Full cycle frequency

    samplePointer = 0.0;
    active = true;
    noteHeld = true;
}

void WaveChannel::noteOff()
{
    noteHeld = false;
    active = false;  // Wave channel has no envelope — stops immediately
}

float WaveChannel::processSample()
{
    if (!active)
        return 0.0f;

    if (volumeCode == 0)
        return 0.0f;  // Muted

    // Read the current nibble from wave RAM
    int sampleIndex = static_cast<int>(samplePointer) & 31;
    int byteIndex = sampleIndex / 2;
    uint8_t nibbleValue;

    if ((sampleIndex & 1) == 0)
        nibbleValue = (waveRam[byteIndex] >> 4) & 0x0F;  // Upper nibble first
    else
        nibbleValue = waveRam[byteIndex] & 0x0F;          // Lower nibble

    // Apply volume shift
    // volumeCode: 1=100% (shift 0), 2=50% (shift 1), 3=25% (shift 2)
    int shifted = nibbleValue >> volumeShift;

    // Advance sample pointer
    double increment = (frequencyHz * 32.0) / hostSampleRate;
    samplePointer += increment;
    while (samplePointer >= 32.0)
        samplePointer -= 32.0;

    // Normalize: nibble range is 0-15, center at 7.5, scale to -1..+1
    float sample = (static_cast<float>(shifted) - 7.5f) / 7.5f;
    return sample;
}

bool WaveChannel::isActive() const
{
    return active;
}

void WaveChannel::setVolumeCode(int code)
{
    volumeCode = std::clamp(code, 0, 3);
    switch (volumeCode)
    {
        case 0: volumeShift = 4; break;  // Effectively mutes (shift 4-bit value by 4 = 0)
        case 1: volumeShift = 0; break;  // 100%
        case 2: volumeShift = 1; break;  // 50%
        case 3: volumeShift = 2; break;  // 25%
    }
}

void WaveChannel::setWaveNibble(int index, int value)
{
    if (index < 0 || index >= 32) return;
    int byteIdx = index / 2;
    uint8_t val = static_cast<uint8_t>(std::clamp(value, 0, 15));

    if ((index & 1) == 0)
    {
        // Upper nibble
        waveRam[byteIdx] = (waveRam[byteIdx] & 0x0F) | (val << 4);
    }
    else
    {
        // Lower nibble
        waveRam[byteIdx] = (waveRam[byteIdx] & 0xF0) | val;
    }
}

void WaveChannel::loadWaveform(const uint8_t* nibbles32)
{
    for (int i = 0; i < 32; ++i)
        setWaveNibble(i, nibbles32[i]);
}

void WaveChannel::loadPreset(int presetIndex)
{
    switch (presetIndex)
    {
        case 0: loadWaveform(PRESET_TRIANGLE); break;
        case 1: loadWaveform(PRESET_SAWTOOTH); break;
        case 2: loadWaveform(PRESET_SQUARE); break;
        case 3: loadWaveform(PRESET_SINE); break;
        case 4: loadWaveform(PRESET_DW3_BASS); break;
        default: loadWaveform(PRESET_TRIANGLE); break;
    }
}

void WaveChannel::getWaveform(uint8_t* nibbles32) const
{
    for (int i = 0; i < 32; ++i)
    {
        int byteIdx = i / 2;
        if ((i & 1) == 0)
            nibbles32[i] = (waveRam[byteIdx] >> 4) & 0x0F;
        else
            nibbles32[i] = waveRam[byteIdx] & 0x0F;
    }
}

const char* WaveChannel::getPresetName(int index)
{
    static const char* names[] = {
        "Triangle", "Sawtooth", "Square", "Sine", "DW3 Bass"
    };
    if (index >= 0 && index < NUM_PRESETS)
        return names[index];
    return "Unknown";
}
