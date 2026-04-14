#pragma once

#include <JuceHeader.h>
#include "PulseChannel.h"
#include "WaveChannel.h"
#include "NoiseChannel.h"
#include "PresetManager.h"
#include "Arpeggiator.h"
#include <atomic>

class GBCSynthProcessor : public juce::AudioProcessor
{
public:
    GBCSynthProcessor();
    ~GBCSynthProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    // FL Studio preset browser integration
    int getNumPrograms() override { return PresetManager::getNumPresets(); }
    int getCurrentProgram() override { return currentPreset; }
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    // Ring buffer for waveform display
    static constexpr int WAVEFORM_BUFFER_SIZE = 1024;
    float waveformBuffer[WAVEFORM_BUFFER_SIZE] = {};
    int waveformWritePos = 0;

    // Note activity flag — set on noteOn, GUI clears when it has consumed it
    std::atomic<bool> noteTriggered{ false };

private:
    // Helper to read a Choice parameter as its integer index
    int getChoiceIndex(const juce::String& paramID) const;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void updateChannelParameters();
    void handleMidiEvent(const juce::MidiMessage& msg);

    juce::AudioProcessorValueTreeState apvts;

    // GBC channels
    PulseChannel pulse1{ true };   // CH1 — has frequency sweep
    PulseChannel pulse2{ false };  // CH2 — no sweep
    WaveChannel wave;              // CH3 — wavetable
    NoiseChannel noise;            // CH4 — LFSR noise

    // Active channel for single-channel mode
    // 0 = Pulse1, 1 = Pulse2, 2 = Wave, 3 = Noise
    int activeChannel = 0;

    // Channel mode: 0 = Single, 1 = Stack (all 4 play simultaneously)
    int channelMode = 0;

    // Arpeggiator
    Arpeggiator arpeggiator;
    int arpCurrentNote = -1;

    // Note tracking — only release the note that's currently playing
    int currentNoteNumber = -1;

    // Current preset index for FL Studio program interface
    int currentPreset = 0;

    // High-pass filter for DC offset removal
    float hpfPrevInput[2] = { 0.0f, 0.0f };
    float hpfPrevOutput[2] = { 0.0f, 0.0f };
    static constexpr float HPF_ALPHA = 0.999f;

    double currentSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GBCSynthProcessor)
};
