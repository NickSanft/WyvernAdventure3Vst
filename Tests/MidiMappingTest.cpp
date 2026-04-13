#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "GBCConstants.h"
#include <cmath>

TEST_CASE("MIDI note to pulse period produces valid range", "[midi]")
{
    for (int note = 0; note <= 127; ++note)
    {
        int period = midiNoteToPulsePeriod(note);
        REQUIRE(period >= 0);
        REQUIRE(period <= 2047);
    }
}

TEST_CASE("MIDI note to wave period produces valid range", "[midi]")
{
    for (int note = 0; note <= 127; ++note)
    {
        int period = midiNoteToWavePeriod(note);
        REQUIRE(period >= 0);
        REQUIRE(period <= 2047);
    }
}

TEST_CASE("Higher MIDI notes produce higher period values", "[midi]")
{
    // Higher period = higher frequency in GBC hardware
    int periodC4 = midiNoteToPulsePeriod(60);
    int periodC5 = midiNoteToPulsePeriod(72);

    REQUIRE(periodC5 > periodC4);
}

TEST_CASE("Pulse frequency is monotonically increasing with period", "[midi]")
{
    double prevFreq = 0.0;
    for (int p = 0; p < 2047; ++p)
    {
        double freq = pulseFrequency(p);
        REQUIRE(freq > prevFreq);
        prevFreq = freq;
    }
}

TEST_CASE("Pulse frequency formula matches expected calculation", "[midi]")
{
    // freq = GBC_CLOCK_HZ / (32 * (2048 - period))
    for (int p : {0, 500, 1000, 1500, 2000})
    {
        double expected = GBC_CLOCK_HZ / (32.0 * (2048 - p));
        double actual = pulseFrequency(p);
        REQUIRE_THAT(actual, Catch::Matchers::WithinAbs(expected, 0.001));
    }
}

TEST_CASE("A4 MIDI note converts to approximately 440 Hz", "[midi]")
{
    // GBC frequencies are quantized to 11-bit periods, so won't be exactly 440
    int period = midiNoteToPulsePeriod(69);
    double freq = pulseFrequency(period);

    // Allow small tolerance due to 11-bit quantization
    REQUIRE_THAT(freq, Catch::Matchers::WithinRel(440.0, 0.02));
}

TEST_CASE("Duty table entries are 0 or 1", "[constants]")
{
    for (int d = 0; d < 4; ++d)
        for (int s = 0; s < 8; ++s)
            REQUIRE((DUTY_TABLE[d][s] == 0 || DUTY_TABLE[d][s] == 1));
}

TEST_CASE("Duty table high counts match GBC hardware", "[constants]")
{
    // GBC duty patterns from pandocs:
    // 0: 12.5% = 1 high, 1: 25% = 2 high, 2: 50% = 5 high, 3: 75% = 6 high
    // Note: GBC "50%" is actually 5/8 = 62.5% — this is authentic hardware behavior
    int expectedHighCounts[] = { 1, 2, 5, 6 };

    for (int d = 0; d < 4; ++d)
    {
        int highCount = 0;
        for (int s = 0; s < 8; ++s)
            if (DUTY_TABLE[d][s] == 1) highCount++;

        REQUIRE(highCount == expectedHighCounts[d]);
    }
}

TEST_CASE("Noise divisors are positive and non-decreasing", "[constants]")
{
    for (int i = 0; i < 8; ++i)
        REQUIRE(NOISE_DIVISORS[i] > 0);

    for (int i = 1; i < 8; ++i)
        REQUIRE(NOISE_DIVISORS[i] >= NOISE_DIVISORS[i - 1]);
}
