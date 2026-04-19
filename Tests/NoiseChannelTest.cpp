#include <catch2/catch_test_macros.hpp>
#include "NoiseChannel.h"
#include "GBCConstants.h"
#include <cmath>
#include <set>

TEST_CASE("NoiseChannel basic output", "[noise]")
{
    NoiseChannel ch;
    ch.setSampleRate(44100.0);
    ch.setClockShift(0);
    ch.setDivisorCode(0);
    ch.setWidthMode(false);  // 15-bit
    ch.setPeakLevel(15); ch.setADSR(0.0f, 0.0f, 15.0f, 0.0f);

    ch.noteOn(0, 1.0f);
    REQUIRE(ch.isActive());

    // Generate samples
    for (int i = 0; i < 1000; ++i)
    {
        float sample = ch.processSample();
        REQUIRE(sample >= -1.0f);
        REQUIRE(sample <= 1.0f);
    }
}

TEST_CASE("NoiseChannel 15-bit produces pseudo-random output", "[noise]")
{
    NoiseChannel ch;
    ch.setSampleRate(44100.0);
    ch.setClockShift(0);
    ch.setDivisorCode(0);
    ch.setWidthMode(false);
    ch.setPeakLevel(15); ch.setADSR(0.0f, 0.0f, 15.0f, 0.0f);

    ch.noteOn(0, 1.0f);

    // Collect unique sample values — noise should produce variety
    std::set<float> uniqueValues;
    for (int i = 0; i < 10000; ++i)
    {
        uniqueValues.insert(ch.processSample());
    }

    // Should have at least 2 distinct values (positive and negative)
    REQUIRE(uniqueValues.size() >= 2);
}

TEST_CASE("NoiseChannel 7-bit mode sounds different from 15-bit", "[noise]")
{
    auto generateSamples = [](bool narrowMode) -> std::vector<float>
    {
        NoiseChannel ch;
        ch.setSampleRate(44100.0);
        ch.setClockShift(2);
        ch.setDivisorCode(0);
        ch.setWidthMode(narrowMode);
        ch.setPeakLevel(15); ch.setADSR(0.0f, 0.0f, 15.0f, 0.0f);
        ch.noteOn(0, 1.0f);

        std::vector<float> samples;
        for (int i = 0; i < 4410; ++i)
            samples.push_back(ch.processSample());
        return samples;
    };

    auto wide = generateSamples(false);
    auto narrow = generateSamples(true);

    // The sequences should differ
    int differences = 0;
    for (size_t i = 0; i < wide.size(); ++i)
    {
        if (wide[i] != narrow[i])
            differences++;
    }

    REQUIRE(differences > 0);
}

TEST_CASE("NoiseChannel envelope decay", "[noise]")
{
    NoiseChannel ch;
    ch.setSampleRate(44100.0);
    ch.setClockShift(0);
    ch.setDivisorCode(0);
    ch.setWidthMode(false);
    ch.setPeakLevel(15); ch.setADSR(0.0f, 200.0f, 0.0f, 0.0f);  // Fast decay

    ch.noteOn(0, 1.0f);

    float maxEarly = 0.0f;
    for (int i = 0; i < 500; ++i)
        maxEarly = std::max(maxEarly, std::abs(ch.processSample()));

    // Process more to let envelope decay
    for (int i = 0; i < 44100; ++i)
        ch.processSample();

    float maxLate = 0.0f;
    for (int i = 0; i < 500; ++i)
        maxLate = std::max(maxLate, std::abs(ch.processSample()));

    REQUIRE(maxLate < maxEarly);
}
