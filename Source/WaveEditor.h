#pragma once

#include <JuceHeader.h>
#include "RetroLookAndFeel.h"
#include <array>
#include <functional>

// Interactive 32-bar nibble editor for the Wave channel waveform
class WaveEditor : public juce::Component
{
public:
    WaveEditor();

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;

    // Set/get the 32 nibble values (0-15 each)
    void setWaveform(const uint8_t* nibbles32);
    void getWaveform(uint8_t* nibbles32) const;

    // Callback when user edits a nibble
    std::function<void(int index, int value)> onNibbleChanged;

private:
    std::array<uint8_t, 32> nibbles{};

    void setNibbleFromMouse(const juce::MouseEvent& event);
};
