#pragma once

#include <JuceHeader.h>
#include "PulseChannel.h"
#include "WaveChannel.h"
#include "NoiseChannel.h"
#include "PresetManager.h"
#include "Arpeggiator.h"
#include <atomic>
#include <array>
#include <vector>
#include <cstdint>

class GBCSynthProcessor : public juce::AudioProcessor
{
public:
    // Number of simultaneous voices per GBC channel pool (for polyphony in Single
    // Mode; in Stack Mode each chord note allocates one voice from every pool).
    static constexpr int MAX_VOICES = 8;

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

    // Tracks which MIDI note each voice slot is currently assigned to.
    // note == -1 means the slot's key has been released (voice may still be
    // ringing out via envelope decay). Public so free helpers in the .cpp can
    // reference it via GBCSynthProcessor::VoiceSlot.
    struct VoiceSlot
    {
        int note = -1;
        std::uint64_t age = 0;
    };

private:
    int getChoiceIndex(const juce::String& paramID) const;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void updateChannelParameters();
    void handleMidiEvent(const juce::MidiMessage& msg);

    void channelNoteOn(int channelIdx, int midiNote, float velocity);
    void channelNoteOff(int channelIdx, int midiNote);
    void channelAllNotesOff(int channelIdx);

    juce::AudioProcessorValueTreeState apvts;

    // Polyphonic voice pools — each channel type has MAX_VOICES independent
    // instances. Voice 0..7 for pulse1, voice 0..7 for pulse2, etc.
    std::vector<PulseChannel> pulse1Voices;
    std::vector<PulseChannel> pulse2Voices;
    std::vector<WaveChannel> waveVoices;
    std::vector<NoiseChannel> noiseVoices;

    std::array<VoiceSlot, MAX_VOICES> pulse1Slots{};
    std::array<VoiceSlot, MAX_VOICES> pulse2Slots{};
    std::array<VoiceSlot, MAX_VOICES> waveSlots{};
    std::array<VoiceSlot, MAX_VOICES> noiseSlots{};

    std::uint64_t voiceCounter = 0;

    // Active channel for Single Mode: 0 = Pulse1, 1 = Pulse2, 2 = Wave, 3 = Noise
    int activeChannel = 0;

    // Channel mode: 0 = Single, 1 = Stack (all 4 play simultaneously)
    int channelMode = 0;

    Arpeggiator arpeggiator;
    int arpCurrentNote = -1;

    // Current preset index for FL Studio program interface
    int currentPreset = 0;

    // High-pass filter for DC offset removal
    float hpfPrevInput[2] = { 0.0f, 0.0f };
    float hpfPrevOutput[2] = { 0.0f, 0.0f };
    static constexpr float HPF_ALPHA = 0.999f;

    double currentSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GBCSynthProcessor)
};
