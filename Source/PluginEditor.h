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
    juce::Slider envVolSlider, envPeriodSlider;
    juce::ComboBox envDirCombo;
    juce::Slider sweepPeriodSlider, sweepShiftSlider;
    juce::ToggleButton sweepNegateToggle{ "Negate" };

    // Wave controls
    juce::ComboBox waveVolumeCombo, wavePresetCombo;
    WaveEditor waveEditor;

    // Noise controls
    juce::Slider noiseClockShiftSlider, noiseDivisorSlider;
    juce::ToggleButton noiseWidthToggle{ "7-bit Mode" };
    juce::Slider noiseEnvVolSlider, noiseEnvPeriodSlider;
    juce::ComboBox noiseEnvDirCombo;

    // Common controls
    juce::ComboBox panCombo;
    juce::Slider masterVolumeSlider;

    // Labels
    juce::Label dutyLabel, envVolLabel, envPeriodLabel, envDirLabel;
    juce::Label sweepPeriodLabel, sweepShiftLabel;
    juce::Label waveVolLabel, wavePresetLabel;
    juce::Label noiseClockLabel, noiseDivLabel, noiseEnvVolLabel, noiseEnvPeriodLabel, noiseEnvDirLabel;
    juce::Label panLabel, masterVolLabel;

    // Waveform display
    WaveformDisplay waveformDisplay;

    // Attachments
    std::unique_ptr<ComboBoxAttachment> dutyAttachment, envDirAttachment, panAttachment;
    std::unique_ptr<ComboBoxAttachment> waveVolumeAttachment, wavePresetAttachment;
    std::unique_ptr<ComboBoxAttachment> noiseEnvDirAttachment;
    std::unique_ptr<SliderAttachment> envVolAttachment, envPeriodAttachment;
    std::unique_ptr<SliderAttachment> sweepPeriodAttachment, sweepShiftAttachment;
    std::unique_ptr<SliderAttachment> noiseClockShiftAttachment, noiseDivisorAttachment;
    std::unique_ptr<SliderAttachment> noiseEnvVolAttachment, noiseEnvPeriodAttachment;
    std::unique_ptr<SliderAttachment> masterVolumeAttachment;
    std::unique_ptr<ButtonAttachment> sweepNegateAttachment, noiseWidthAttachment;

    // Preset buttons
    std::vector<std::unique_ptr<juce::TextButton>> presetButtons;

    // Note activity LED state
    bool noteActive = false;

    void setupLabel(juce::Label& label, const juce::String& text);
    void setupRotarySlider(juce::Slider& slider);
    void updateChannelVisibility(int channel);
    void drawHeader(juce::Graphics& g);
    void drawChannelPanel(juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& title);
    static void disableFocusForAllChildren(juce::Component& parent);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GBCSynthEditor)
};
