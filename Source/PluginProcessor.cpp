#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "GBCConstants.h"

GBCSynthProcessor::GBCSynthProcessor()
    : AudioProcessor(BusesProperties()
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

GBCSynthProcessor::~GBCSynthProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout GBCSynthProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Channel select: 0=Pulse1, 1=Pulse2, 2=Wave, 3=Noise
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("channelSelect", 1), "Channel",
        juce::StringArray{ "Pulse 1", "Pulse 2", "Wave", "Noise" }, 0));

    // Duty cycle: 0=12.5%, 1=25%, 2=50%, 3=75%
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("duty", 1), "Duty Cycle",
        juce::StringArray{ "12.5%", "25%", "50%", "75%" }, 2));

    // Volume envelope
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID("envInitVol", 1), "Env Volume", 0, 15, 15));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("envDir", 1), "Env Direction",
        juce::StringArray{ "Down", "Up" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID("envPeriod", 1), "Env Period", 0, 7, 0));

    // Frequency sweep (CH1 only)
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID("sweepPeriod", 1), "Sweep Period", 0, 7, 0));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("sweepNegate", 1), "Sweep Negate", false));
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID("sweepShift", 1), "Sweep Shift", 0, 7, 0));

    // Wave channel parameters
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("waveVolume", 1), "Wave Volume",
        juce::StringArray{ "Mute", "100%", "50%", "25%" }, 1));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("wavePreset", 1), "Wave Preset",
        juce::StringArray{ "Triangle", "Sawtooth", "Square", "Sine", "DW3 Bass" }, 0));

    // Pan: 0=Left, 1=Both, 2=Right
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("pan", 1), "Pan",
        juce::StringArray{ "Left", "Center", "Right" }, 1));

    // Master volume
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("masterVolume", 1), "Master Volume",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.75f));

    return { params.begin(), params.end() };
}

void GBCSynthProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    currentSampleRate = sampleRate;
    pulse1.setSampleRate(sampleRate);
    pulse2.setSampleRate(sampleRate);
    wave.setSampleRate(sampleRate);
    pulse1.reset();
    pulse2.reset();
    wave.reset();

    // Reset HPF state
    for (int ch = 0; ch < 2; ++ch)
    {
        hpfPrevInput[ch] = 0.0f;
        hpfPrevOutput[ch] = 0.0f;
    }
}

void GBCSynthProcessor::releaseResources() {}

void GBCSynthProcessor::updateChannelParameters()
{
    int duty = static_cast<int>(apvts.getRawParameterValue("duty")->load());
    int envVol = static_cast<int>(apvts.getRawParameterValue("envInitVol")->load());
    bool envDir = apvts.getRawParameterValue("envDir")->load() > 0.5f;
    int envPer = static_cast<int>(apvts.getRawParameterValue("envPeriod")->load());
    int panMode = static_cast<int>(apvts.getRawParameterValue("pan")->load());

    activeChannel = static_cast<int>(apvts.getRawParameterValue("channelSelect")->load());

    // Apply to pulse channels
    pulse1.setDutyCycle(duty);
    pulse1.setEnvelope(envVol, envDir, envPer);
    pulse1.setPan(panMode);

    pulse2.setDutyCycle(duty);
    pulse2.setEnvelope(envVol, envDir, envPer);
    pulse2.setPan(panMode);

    // Sweep (CH1 only)
    int swpPer = static_cast<int>(apvts.getRawParameterValue("sweepPeriod")->load());
    bool swpNeg = apvts.getRawParameterValue("sweepNegate")->load() > 0.5f;
    int swpShift = static_cast<int>(apvts.getRawParameterValue("sweepShift")->load());
    pulse1.setSweep(swpPer, swpNeg, swpShift);

    // Wave channel
    int waveVol = static_cast<int>(apvts.getRawParameterValue("waveVolume")->load());
    int wavePreset = static_cast<int>(apvts.getRawParameterValue("wavePreset")->load());
    wave.setVolumeCode(waveVol);
    wave.loadPreset(wavePreset);
    wave.setPan(panMode);
}

void GBCSynthProcessor::handleMidiEvent(const juce::MidiMessage& msg)
{
    if (msg.isNoteOn())
    {
        int midiNote = msg.getNoteNumber();
        float velocity = msg.getFloatVelocity();

        // Update parameters before triggering
        updateChannelParameters();

        switch (activeChannel)
        {
            case 0: // Pulse 1
            {
                int period = midiNoteToPulsePeriod(midiNote);
                pulse1.noteOn(period, velocity);
                break;
            }
            case 1: // Pulse 2
            {
                int period = midiNoteToPulsePeriod(midiNote);
                pulse2.noteOn(period, velocity);
                break;
            }
            case 2: // Wave
            {
                int period = midiNoteToWavePeriod(midiNote);
                wave.noteOn(period, velocity);
                break;
            }
            // Noise channel will be added in Milestone 4
            default:
                break;
        }
    }
    else if (msg.isNoteOff())
    {
        switch (activeChannel)
        {
            case 0: pulse1.noteOff(); break;
            case 1: pulse2.noteOff(); break;
            case 2: wave.noteOff(); break;
            default: break;
        }
    }
}

void GBCSynthProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    buffer.clear();

    float masterVol = apvts.getRawParameterValue("masterVolume")->load();

    // Update parameters each block
    updateChannelParameters();

    auto* leftChannel = buffer.getWritePointer(0);
    auto* rightChannel = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

    int sampleIndex = 0;

    for (const auto metadata : midiMessages)
    {
        const int eventPos = metadata.samplePosition;

        // Render samples up to this MIDI event
        while (sampleIndex < eventPos)
        {
            float mono = 0.0f;

            switch (activeChannel)
            {
                case 0: mono = pulse1.processSample(); break;
                case 1: mono = pulse2.processSample(); break;
                case 2: mono = wave.processSample(); break;
                default: break;
            }

            // Apply panning
            float leftGain = 1.0f, rightGain = 1.0f;
            switch (activeChannel)
            {
                case 0: pulse1.getStereoGain(leftGain, rightGain); break;
                case 1: pulse2.getStereoGain(leftGain, rightGain); break;
                case 2: wave.getStereoGain(leftGain, rightGain); break;
                default: break;
            }

            leftChannel[sampleIndex] = mono * leftGain * masterVol;
            if (rightChannel)
                rightChannel[sampleIndex] = mono * rightGain * masterVol;

            // Write to waveform ring buffer
            waveformBuffer[waveformWritePos] = mono;
            waveformWritePos = (waveformWritePos + 1) % WAVEFORM_BUFFER_SIZE;

            ++sampleIndex;
        }

        // Handle the MIDI event
        handleMidiEvent(metadata.getMessage());
    }

    // Render remaining samples after last MIDI event
    while (sampleIndex < buffer.getNumSamples())
    {
        float mono = 0.0f;

        switch (activeChannel)
        {
            case 0: mono = pulse1.processSample(); break;
            case 1: mono = pulse2.processSample(); break;
            default: break;
        }

        float leftGain = 1.0f, rightGain = 1.0f;
        switch (activeChannel)
        {
            case 0: pulse1.getStereoGain(leftGain, rightGain); break;
            case 1: pulse2.getStereoGain(leftGain, rightGain); break;
            default: break;
        }

        leftChannel[sampleIndex] = mono * leftGain * masterVol;
        if (rightChannel)
            rightChannel[sampleIndex] = mono * rightGain * masterVol;

        waveformBuffer[waveformWritePos] = mono;
        waveformWritePos = (waveformWritePos + 1) % WAVEFORM_BUFFER_SIZE;

        ++sampleIndex;
    }

    // Apply high-pass filter (DC offset removal) to each output channel
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* channelData = buffer.getWritePointer(ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float input = channelData[i];
            float output = input - hpfPrevInput[ch] + HPF_ALPHA * hpfPrevOutput[ch];
            hpfPrevInput[ch] = input;
            hpfPrevOutput[ch] = output;
            channelData[i] = output;
        }
    }
}

juce::AudioProcessorEditor* GBCSynthProcessor::createEditor()
{
    return new GBCSynthEditor(*this);
}

void GBCSynthProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void GBCSynthProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GBCSynthProcessor();
}
