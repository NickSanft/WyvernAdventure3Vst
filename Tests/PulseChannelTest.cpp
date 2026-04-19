#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "PulseChannel.h"
#include "GBCConstants.h"
#include <cmath>

TEST_CASE("PulseChannel produces output in valid range", "[pulse]")
{
    PulseChannel ch(false);
    ch.setSampleRate(44100.0);
    ch.setDutyCycle(2);  // 50%
    ch.setPeakLevel(15); ch.setADSR(0.0f, 0.0f, 15.0f, 0.0f);  // Max volume, no decay

    int period = midiNoteToPulsePeriod(69);
    ch.noteOn(period, 1.0f);

    REQUIRE(ch.isActive());

    for (int i = 0; i < 4410; ++i)
    {
        float sample = ch.processSample();
        REQUIRE(sample >= -1.0f);
        REQUIRE(sample <= 1.0f);
    }
}

TEST_CASE("PulseChannel 12.5% duty has fewer positive samples than 50%", "[pulse]")
{
    auto countPositive = [](int dutyMode) -> float
    {
        PulseChannel ch(false);
        ch.setSampleRate(44100.0);
        ch.setDutyCycle(dutyMode);
        ch.setPeakLevel(15); ch.setADSR(0.0f, 0.0f, 15.0f, 0.0f);
        int period = midiNoteToPulsePeriod(69);
        ch.noteOn(period, 1.0f);

        int pos = 0, total = 0;
        for (int i = 0; i < 4410; ++i)
        {
            float s = ch.processSample();
            if (s > 0.0f) pos++;
            if (s != 0.0f) total++;
        }
        return float(pos) / float(total);
    };

    float ratio125 = countPositive(0);   // 12.5%
    float ratio50 = countPositive(2);    // 50% (actually 5/8 in GBC)

    REQUIRE(ratio125 < ratio50);
}

TEST_CASE("PulseChannel envelope decay reduces amplitude", "[pulse]")
{
    PulseChannel ch(false);
    ch.setSampleRate(44100.0);
    ch.setDutyCycle(2);
    ch.setPeakLevel(15); ch.setADSR(0.0f, 200.0f, 0.0f, 0.0f);  // Fast decay

    int period = midiNoteToPulsePeriod(69);
    ch.noteOn(period, 1.0f);

    float maxInitial = 0.0f;
    for (int i = 0; i < 100; ++i)
        maxInitial = std::max(maxInitial, std::abs(ch.processSample()));

    for (int i = 0; i < 44100; ++i)
        ch.processSample();

    float maxLater = 0.0f;
    for (int i = 0; i < 100; ++i)
        maxLater = std::max(maxLater, std::abs(ch.processSample()));

    REQUIRE(maxLater < maxInitial);
}

TEST_CASE("PulseChannel sweep changes frequency (CH1)", "[pulse]")
{
    PulseChannel ch(true);
    ch.setSampleRate(44100.0);
    ch.setDutyCycle(2);
    ch.setPeakLevel(15); ch.setADSR(0.0f, 0.0f, 15.0f, 0.0f);
    ch.setSweep(1, false, 3);  // Period 1, no negate, shift 3 (upward sweep)

    int period = midiNoteToPulsePeriod(60);  // Middle C
    ch.noteOn(period, 1.0f);

    REQUIRE(ch.isActive());

    // With upward sweep from a high note, should overflow quickly
    bool silenced = false;
    for (int i = 0; i < 441000; ++i)
    {
        ch.processSample();
        if (!ch.isActive())
        {
            silenced = true;
            break;
        }
    }

    REQUIRE(silenced);
}

TEST_CASE("PulseChannel noteOff with decay envelope eventually stops", "[pulse]")
{
    PulseChannel ch(false);
    ch.setSampleRate(44100.0);
    ch.setDutyCycle(2);
    ch.setPeakLevel(15); ch.setADSR(0.0f, 200.0f, 0.0f, 0.0f);

    int period = midiNoteToPulsePeriod(69);
    ch.noteOn(period, 1.0f);
    REQUIRE(ch.isActive());

    ch.noteOff();

    for (int i = 0; i < 441000; ++i)
    {
        ch.processSample();
        if (!ch.isActive()) break;
    }

    REQUIRE(!ch.isActive());
}
