#pragma once

#include <JuceHeader.h>
#include "RetroLookAndFeel.h"
#include "Theme.h"

class GBCSynthProcessor;

// Real-time oscilloscope with CRT scanline effect and volume-responsive phosphor glow
class WaveformDisplay : public juce::Component,
                        public juce::Timer
{
public:
    explicit WaveformDisplay(GBCSynthProcessor& processor);
    ~WaveformDisplay() override;

    void paint(juce::Graphics& g) override;
    void timerCallback() override;

    // Trigger a brief flash overlay on the waveform (called on note-on)
    void triggerFlash();

private:
    GBCSynthProcessor& processorRef;
    std::array<float, 512> displayBuffer{};
    float currentPeakLevel = 0.0f;   // For dynamic glow intensity
    float flashAlpha = 0.0f;         // Fades from 1.0 to 0.0 over ~200ms

    void drawScanlines(juce::Graphics& g, juce::Rectangle<float> bounds);
};
