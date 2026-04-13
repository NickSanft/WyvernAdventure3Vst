#pragma once

#include <JuceHeader.h>
#include "RetroLookAndFeel.h"

class GBCSynthProcessor;

// Real-time oscilloscope with CRT scanline effect and phosphor glow
class WaveformDisplay : public juce::Component,
                        public juce::Timer
{
public:
    explicit WaveformDisplay(GBCSynthProcessor& processor);
    ~WaveformDisplay() override;

    void paint(juce::Graphics& g) override;
    void timerCallback() override;

private:
    GBCSynthProcessor& processorRef;
    std::array<float, 512> displayBuffer{};

    void drawScanlines(juce::Graphics& g, juce::Rectangle<float> bounds);
};
