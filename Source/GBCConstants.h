#pragma once

#include <algorithm>
#include <cmath>

// GBC master clock: 4,194,304 Hz
static constexpr double GBC_CLOCK_HZ = 4194304.0;

// Envelope clock: 64 Hz (GBC_CLOCK_HZ / 65536)
static constexpr double ENVELOPE_CLOCK_HZ = 64.0;

// Sweep clock: 128 Hz (GBC_CLOCK_HZ / 32768)
static constexpr double SWEEP_CLOCK_HZ = 128.0;

// Pulse channel: frequency = GBC_CLOCK_HZ / (4 * (2048 - period))
// period is 11-bit (0-2047)
inline double pulseFrequency(int period)
{
    if (period >= 2048) return 0.0;
    return GBC_CLOCK_HZ / (4.0 * (2048 - period));
}

// Wave channel: effective frequency = GBC_CLOCK_HZ / (2 * (2048 - period)) / 32
// (32 samples per cycle)
inline double waveFrequency(int period)
{
    if (period >= 2048) return 0.0;
    return GBC_CLOCK_HZ / (2.0 * (2048 - period));
}

// Noise divisors (divisor code 0-7)
static const int NOISE_DIVISORS[8] = { 8, 16, 32, 48, 64, 80, 96, 112 };

// Duty cycle lookup table: 4 patterns, each 8 steps
// Values represent high (1) or low (0) for each step of the waveform
static const int DUTY_TABLE[4][8] = {
    {0, 0, 0, 0, 0, 0, 0, 1},  // 12.5%
    {1, 0, 0, 0, 0, 0, 0, 1},  // 25%
    {1, 0, 0, 0, 1, 1, 1, 1},  // 50%
    {0, 1, 1, 1, 1, 1, 1, 0},  // 75%
};

// Convert MIDI note number to GBC 11-bit period register value
inline int midiNoteToPulsePeriod(int midiNote)
{
    double freqHz = 440.0 * std::pow(2.0, (midiNote - 69) / 12.0);
    int period = static_cast<int>(2048.0 - GBC_CLOCK_HZ / (4.0 * freqHz));
    return std::clamp(period, 0, 2047);
}

inline int midiNoteToWavePeriod(int midiNote)
{
    double freqHz = 440.0 * std::pow(2.0, (midiNote - 69) / 12.0);
    int period = static_cast<int>(2048.0 - GBC_CLOCK_HZ / (2.0 * 32.0 * freqHz));
    return std::clamp(period, 0, 2047);
}
