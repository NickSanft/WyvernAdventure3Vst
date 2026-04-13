#pragma once

#include <JuceHeader.h>

// Color palette for Modern Retro Hybrid theme
namespace RetroColors
{
    const juce::Colour background     { 0xff1a1a2e };  // Dark navy
    const juce::Colour panelBg        { 0xff16213e };  // Slightly lighter navy
    const juce::Colour panelBorder    { 0xff0f3460 };  // Blue border
    const juce::Colour gbcGreen       { 0xff9bbc0f };  // Classic GBC green
    const juce::Colour gbcDarkGreen   { 0xff306230 };  // Darker GBC green
    const juce::Colour purple         { 0xff8b5cf6 };  // Purple accent
    const juce::Colour amber          { 0xfff59e0b };  // Amber/gold accent
    const juce::Colour textPrimary    { 0xffe2e8f0 };  // Light gray text
    const juce::Colour textSecondary  { 0xff94a3b8 };  // Muted text
    const juce::Colour knobFill       { 0xff334155 };  // Knob background
    const juce::Colour knobTrack      { 0xff1e293b };  // Knob track
    const juce::Colour activeTab      { 0xff9bbc0f };  // Active tab color
    const juce::Colour inactiveTab    { 0xff334155 };  // Inactive tab
    const juce::Colour waveformGlow   { 0xff39ff14 };  // Phosphor green glow
    const juce::Colour headerBg       { 0xff0d1117 };  // Header background
}

class RetroLookAndFeel : public juce::LookAndFeel_V4
{
public:
    RetroLookAndFeel();

    // Rotary slider (knob)
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override;

    // Linear slider
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          juce::Slider::SliderStyle style, juce::Slider& slider) override;

    // Combo box
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox& box) override;

    // Button
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;

    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                        bool shouldDrawButtonAsHighlighted,
                        bool shouldDrawButtonAsDown) override;

    // Toggle button
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override;

    // Label
    void drawLabel(juce::Graphics& g, juce::Label& label) override;

    // Popup menu
    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;

    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                           bool isSeparator, bool isActive, bool isHighlighted,
                           bool isTicked, bool hasSubMenu,
                           const juce::String& text, const juce::String& shortcutKeyText,
                           const juce::Drawable* icon, const juce::Colour* textColour) override;

private:
    juce::Font getPixelFont(float height) const;
};
