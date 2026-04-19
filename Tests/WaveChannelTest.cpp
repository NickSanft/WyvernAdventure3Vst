#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "WaveChannel.h"
#include "GBCConstants.h"
#include <cmath>
#include <numeric>

TEST_CASE("WaveChannel basic output in valid range", "[wave]")
{
    WaveChannel ch;
    ch.setSampleRate(44100.0);
    ch.setVolumeCode(1);

    int period = midiNoteToWavePeriod(60);
    ch.noteOn(period, 1.0f);

    REQUIRE(ch.isActive());

    for (int i = 0; i < 1000; ++i)
    {
        float sample = ch.processSample();
        REQUIRE(sample >= -1.0f);
        REQUIRE(sample <= 1.0f);
    }
}

TEST_CASE("WaveChannel mute produces silence", "[wave]")
{
    WaveChannel ch;
    ch.setSampleRate(44100.0);
    ch.setVolumeCode(0);  // Mute

    int period = midiNoteToWavePeriod(60);
    ch.noteOn(period, 1.0f);

    float maxAmplitude = 0.0f;
    for (int i = 0; i < 1000; ++i)
        maxAmplitude = std::max(maxAmplitude, std::abs(ch.processSample()));

    REQUIRE_THAT(maxAmplitude, Catch::Matchers::WithinAbs(0.0, 0.001));
}

TEST_CASE("WaveChannel 100% has wider peak-to-peak range than 25%", "[wave]")
{
    // Measure peak-to-peak range at different volume codes
    auto measurePeakToPeak = [](int volCode) -> float
    {
        WaveChannel ch;
        ch.setSampleRate(44100.0);
        ch.loadPreset(0);  // Triangle
        ch.setVolumeCode(volCode);
        int period = midiNoteToWavePeriod(60);
        ch.noteOn(period, 1.0f);

        float minVal = 1.0f, maxVal = -1.0f;
        for (int i = 0; i < 4410; ++i)
        {
            float s = ch.processSample();
            minVal = std::min(minVal, s);
            maxVal = std::max(maxVal, s);
        }
        return maxVal - minVal;
    };

    float range100 = measurePeakToPeak(1);  // 100%
    float range25 = measurePeakToPeak(3);   // 25%

    REQUIRE(range100 > range25);
}

TEST_CASE("WaveChannel presets load valid nibbles", "[wave]")
{
    WaveChannel ch;

    for (int p = 0; p < WaveChannel::NUM_PRESETS; ++p)
    {
        ch.loadPreset(p);
        uint8_t waveform[32];
        ch.getWaveform(waveform);

        for (int i = 0; i < 32; ++i)
            REQUIRE(waveform[i] <= 15);
    }
}

TEST_CASE("WaveChannel nibble set/get roundtrip", "[wave]")
{
    WaveChannel ch;

    for (int i = 0; i < 32; ++i)
        ch.setWaveNibble(i, i % 16);

    uint8_t result[32];
    ch.getWaveform(result);

    for (int i = 0; i < 32; ++i)
        REQUIRE(result[i] == (i % 16));
}

TEST_CASE("WaveChannel noteOff triggers release and eventually silences", "[wave]")
{
    WaveChannel ch;
    ch.setSampleRate(44100.0);
    ch.setVolumeCode(1);
    // Instant release: attack=0, decay=0, sustain=15, release=0 → instant off
    ch.setADSR(0.0f, 0.0f, 15.0f, 0.0f);

    int period = midiNoteToWavePeriod(60);
    ch.noteOn(period, 1.0f);
    REQUIRE(ch.isActive());

    ch.noteOff();

    // After processing a few samples with release=0, channel should be inactive
    for (int i = 0; i < 10; ++i)
        ch.processSample();
    REQUIRE(!ch.isActive());

    float sample = ch.processSample();
    REQUIRE_THAT(sample, Catch::Matchers::WithinAbs(0.0, 0.001));
}
