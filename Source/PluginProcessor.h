#pragma once

#include <JuceHeader.h>
#include "PulseChannel.h"
#include "WaveChannel.h"
#include "NoiseChannel.h"

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

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    // Ring buffer for waveform display (Milestone 6)
    static constexpr int WAVEFORM_BUFFER_SIZE = 1024;
    float waveformBuffer[WAVEFORM_BUFFER_SIZE] = {};
    int waveformWritePos = 0;

private:
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

    // High-pass filter for DC offset removal
    float hpfPrevInput[2] = { 0.0f, 0.0f };
    float hpfPrevOutput[2] = { 0.0f, 0.0f };
    static constexpr float HPF_ALPHA = 0.999f;

    double currentSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GBCSynthProcessor)
};
