#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "RetroLookAndFeel.h"
#include "WaveformDisplay.h"
#include "WaveEditor.h"
#include "PresetManager.h"

class GBCSynthEditor : public juce::AudioProcessorEditor
{
public:
    explicit GBCSynthEditor(GBCSynthProcessor&);
    ~GBCSynthEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    GBCSynthProcessor& processorRef;
    RetroLookAndFeel retroLookAndFeel;

    // Channel select tabs with icons
    class ChannelTabButton : public juce::TextButton
    {
    public:
        ChannelTabButton(const juce::String& text, int channelIndex);
        void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    private:
        int chIndex;
        void drawIcon(juce::Graphics& g, juce::Rectangle<float> area);
    };
    std::array<std::unique_ptr<ChannelTabButton>, 4> channelTabs;

    // Channel mode (Single/Stack) and Day/Night toggle
    juce::ComboBox channelModeCombo;
    juce::TextButton dayNightButton{ "NIGHT" };
    std::unique_ptr<ComboBoxAttachment> channelModeAttachment;

    // Vibrato + Arpeggiator controls
    juce::ToggleButton vibratoToggle{ "Vibrato" };
    juce::Slider vibratoRateSlider, vibratoDepthSlider;
    juce::Label vibratoRateLabel, vibratoDepthLabel;

    juce::ToggleButton arpToggle{ "Arp" };
    juce::Slider arpRateSlider;
    juce::ComboBox arpPatternCombo;
    juce::Label arpRateLabel, arpPatternLabel;

    std::unique_ptr<ButtonAttachment> vibratoToggleAttachment, arpToggleAttachment;
    std::unique_ptr<SliderAttachment> vibratoRateAttachment, vibratoDepthAttachment, arpRateAttachment;
    std::unique_ptr<ComboBoxAttachment> arpPatternAttachment;

    // Pulse controls
    juce::ComboBox dutyCombo;
    juce::Slider envVolSlider;
    juce::Slider envAttackSlider, envDecaySlider, envSustainSlider, envReleaseSlider;
    juce::Slider sweepPeriodSlider, sweepShiftSlider;
    juce::ToggleButton sweepNegateToggle{ "Negate" };

    // Wave controls
    juce::ComboBox waveVolumeCombo, wavePresetCombo;
    WaveEditor waveEditor;

    // Noise controls
    juce::Slider noiseClockShiftSlider, noiseDivisorSlider;
    juce::ToggleButton noiseWidthToggle{ "7-bit Mode" };
    juce::Slider noiseEnvVolSlider;
    juce::Slider noiseAttackSlider, noiseDecaySlider, noiseSustainSlider, noiseReleaseSlider;

    // Common controls
    juce::ComboBox panCombo;
    juce::Slider masterVolumeSlider;

    // Labels
    juce::Label dutyLabel, envVolLabel;
    juce::Label envAttackLabel, envDecayLabel, envSustainLabel, envReleaseLabel;
    juce::Label sweepPeriodLabel, sweepShiftLabel;
    juce::Label waveVolLabel, wavePresetLabel;
    juce::Label noiseClockLabel, noiseDivLabel, noiseEnvVolLabel;
    juce::Label noiseAttackLabel, noiseDecayLabel, noiseSustainLabel, noiseReleaseLabel;
    juce::Label panLabel, masterVolLabel;

    // Waveform display
    WaveformDisplay waveformDisplay;

    // Attachments
    std::unique_ptr<ComboBoxAttachment> dutyAttachment, panAttachment;
    std::unique_ptr<ComboBoxAttachment> waveVolumeAttachment, wavePresetAttachment;
    std::unique_ptr<SliderAttachment> envVolAttachment;
    std::unique_ptr<SliderAttachment> envAttackAttachment, envDecayAttachment, envSustainAttachment, envReleaseAttachment;
    std::unique_ptr<SliderAttachment> sweepPeriodAttachment, sweepShiftAttachment;
    std::unique_ptr<SliderAttachment> noiseClockShiftAttachment, noiseDivisorAttachment;
    std::unique_ptr<SliderAttachment> noiseEnvVolAttachment;
    std::unique_ptr<SliderAttachment> noiseAttackAttachment, noiseDecayAttachment, noiseSustainAttachment, noiseReleaseAttachment;
    std::unique_ptr<SliderAttachment> masterVolumeAttachment;
    std::unique_ptr<ButtonAttachment> sweepNegateAttachment, noiseWidthAttachment;

    // Preset buttons
    // Preset browser ComboBox — scales to any number of presets
    juce::ComboBox presetCombo;
    juce::Label presetLabel;
    juce::TextButton presetPrevButton{ "<" };
    juce::TextButton presetNextButton{ ">" };

    // Note activity LED state
    bool noteActive = false;

    void setupLabel(juce::Label& label, const juce::String& text);
    void setupRotarySlider(juce::Slider& slider);
    void updateChannelVisibility(int channel);
    void drawHeader(juce::Graphics& g);
    void drawChannelPanel(juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& title);
    static void disableFocusForAllChildren(juce::Component& parent);

    // Cached layout rects (set in resized, drawn in paint) so panels align with controls
    juce::Rectangle<int> modRowRect, channelPanelRect, bottomPanelRect;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GBCSynthEditor)
};
