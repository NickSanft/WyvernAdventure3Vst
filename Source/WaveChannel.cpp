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

// Half-Sine: positive hump in the first half, silent midpoint in the second.
// Strong even harmonics → warm, organ-like tone.
const uint8_t WaveChannel::PRESET_HALF_SINE[32] = {
    8, 10, 12, 13, 14, 15, 15, 15, 15, 15, 15, 14, 13, 12, 10, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
};

// Pulse 25% in wavetable form — sharper lows, extra harmonics vs pulse channel.
const uint8_t WaveChannel::PRESET_PULSE25[32] = {
    15, 15, 15, 15, 15, 15, 15, 15,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

// Detuned Saw: two offset sawtooth ramps → thick, chorus-like lead character.
const uint8_t WaveChannel::PRESET_DETUNED_SAW[32] = {
    0, 2, 3, 5, 6, 8, 9, 11, 12, 14, 15, 13, 11, 9, 7, 5,
    3, 1, 2, 4, 6, 8, 10, 12, 14, 15, 13, 11, 9, 7, 4, 2
};

// Bell: 4 triangular cycles per waveform period → bright bell/glockenspiel tone.
const uint8_t WaveChannel::PRESET_BELL[32] = {
    8, 12, 15, 12, 8, 4, 0, 4, 8, 12, 15, 12, 8, 4, 0, 4,
    8, 12, 15, 12, 8, 4, 0, 4, 8, 12, 15, 12, 8, 4, 0, 4
};

// Acid Bass: rising ramp with a sharp resonant peak → funky 303-ish character.
const uint8_t WaveChannel::PRESET_ACID_BASS[32] = {
    0, 1, 3, 5, 7, 9, 11, 13, 15, 15, 14, 11, 7, 3, 1, 0,
    1, 2, 4, 6, 8, 10, 12, 14, 15, 14, 12, 9, 6, 3, 1, 0
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
    adsr.reset();
    adsr.setPeakLevel(15.0f);

    // Load triangle as default waveform
    loadPreset(0);
}

void WaveChannel::setSampleRate(double sampleRate)
{
    hostSampleRate = sampleRate;
    adsr.setSampleRate(sampleRate);
}

void WaveChannel::noteOn(int period, float /*velocity*/)
{
    periodRegister = std::clamp(period, 0, 2047);

    double rawRate = waveFrequency(periodRegister);
    frequencyHz = rawRate / 32.0;

    samplePointer = 0.0;
    adsr.noteOn();
    active = true;
    noteHeld = true;
}

void WaveChannel::noteOff()
{
    noteHeld = false;
    // Previously stopped immediately (wave had no envelope). Now uses GBCEnvelope
    // release, which will decay naturally and set active = false when done.
    adsr.noteOff();
}

void WaveChannel::setADSR(float attackMs, float decayMs, float sustainLevel, float releaseMs)
{
    adsr.setParams(attackMs, decayMs, sustainLevel, releaseMs);
}

float WaveChannel::processSample()
{
    if (!active)
        return 0.0f;

    if (volumeCode == 0)
        return 0.0f;  // Muted

    // Advance GBCEnvelope envelope
    float envLevel = adsr.tick();
    if (!adsr.isActive())
    {
        active = false;
        return 0.0f;
    }

    // Read the current nibble from wave RAM
    int sampleIndex = static_cast<int>(samplePointer) & 31;
    int byteIndex = sampleIndex / 2;
    uint8_t nibbleValue;

    if ((sampleIndex & 1) == 0)
        nibbleValue = (waveRam[byteIndex] >> 4) & 0x0F;
    else
        nibbleValue = waveRam[byteIndex] & 0x0F;

    int shifted = nibbleValue >> volumeShift;

    float vibratoMul = tickVibrato(hostSampleRate);
    double increment = (frequencyHz * vibratoMul * 32.0) / hostSampleRate;
    samplePointer += increment;
    while (samplePointer >= 32.0)
        samplePointer -= 32.0;

    // Normalize nibble to -1..+1 then scale by GBCEnvelope level (0..15 -> 0..1)
    float sample = (static_cast<float>(shifted) - 7.5f) / 7.5f;
    sample *= (envLevel / 15.0f);
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
        case 0: loadWaveform(PRESET_TRIANGLE);    break;
        case 1: loadWaveform(PRESET_SAWTOOTH);    break;
        case 2: loadWaveform(PRESET_SQUARE);      break;
        case 3: loadWaveform(PRESET_SINE);        break;
        case 4: loadWaveform(PRESET_DW3_BASS);    break;
        case 5: loadWaveform(PRESET_HALF_SINE);   break;
        case 6: loadWaveform(PRESET_PULSE25);     break;
        case 7: loadWaveform(PRESET_DETUNED_SAW); break;
        case 8: loadWaveform(PRESET_BELL);        break;
        case 9: loadWaveform(PRESET_ACID_BASS);   break;
        default: loadWaveform(PRESET_TRIANGLE);   break;
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
        "Triangle", "Sawtooth", "Square", "Sine", "DW3 Bass",
        "Half-Sine", "Pulse 25%", "Detuned Saw", "Bell", "Acid Bass"
    };
    if (index >= 0 && index < NUM_PRESETS)
        return names[index];
    return "Unknown";
}
