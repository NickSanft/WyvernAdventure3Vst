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

    // Channel mode: Single (one channel) or Stack (all 4 simultaneously)
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("channelMode", 1), "Channel Mode",
        juce::StringArray{ "Single", "Stack" }, 0));

    // Vibrato LFO
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("vibratoOn", 1), "Vibrato", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("vibratoRate", 1), "Vibrato Rate (Hz)",
        juce::NormalisableRange<float>(1.0f, 12.0f, 0.1f), 6.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("vibratoDepth", 1), "Vibrato Depth (cents)",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 25.0f));

    // Arpeggiator
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("arpOn", 1), "Arp", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("arpRate", 1), "Arp Rate (Hz)",
        juce::NormalisableRange<float>(2.0f, 32.0f, 0.5f), 8.0f));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("arpPattern", 1), "Arp Pattern",
        juce::StringArray{ "Up", "Down", "Up-Down", "Random" }, 0));

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

    // Noise channel parameters
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID("noiseClockShift", 1), "Noise Clock Shift", 0, 13, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID("noiseDivisor", 1), "Noise Divisor", 0, 7, 0));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("noiseWidth", 1), "Noise 7-bit Mode", false));
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID("noiseEnvInitVol", 1), "Noise Env Volume", 0, 15, 15));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("noiseEnvDir", 1), "Noise Env Direction",
        juce::StringArray{ "Down", "Up" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID("noiseEnvPeriod", 1), "Noise Env Period", 0, 7, 0));

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

int GBCSynthProcessor::getChoiceIndex(const juce::String& paramID) const
{
    if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(paramID)))
        return param->getIndex();
    return 0;
}

void GBCSynthProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    currentSampleRate = sampleRate;
    pulse1.setSampleRate(sampleRate);
    pulse2.setSampleRate(sampleRate);
    wave.setSampleRate(sampleRate);
    noise.setSampleRate(sampleRate);
    pulse1.reset();
    pulse2.reset();
    wave.reset();
    noise.reset();
    arpeggiator.setSampleRate(sampleRate);
    arpeggiator.reset();

    for (int ch = 0; ch < 2; ++ch)
    {
        hpfPrevInput[ch] = 0.0f;
        hpfPrevOutput[ch] = 0.0f;
    }
}

void GBCSynthProcessor::releaseResources() {}

void GBCSynthProcessor::updateChannelParameters()
{
    // Read Choice parameters using getIndex() for correct values
    activeChannel = getChoiceIndex("channelSelect");
    channelMode = getChoiceIndex("channelMode");

    // Vibrato settings (applied to pulse + wave channels)
    bool vibOn = apvts.getRawParameterValue("vibratoOn")->load() > 0.5f;
    float vibRate = apvts.getRawParameterValue("vibratoRate")->load();
    float vibDepth = apvts.getRawParameterValue("vibratoDepth")->load();
    pulse1.setVibrato(vibOn, vibRate, vibDepth);
    pulse2.setVibrato(vibOn, vibRate, vibDepth);
    wave.setVibrato(vibOn, vibRate, vibDepth);

    // Arpeggiator settings
    bool arpOn = apvts.getRawParameterValue("arpOn")->load() > 0.5f;
    float arpRate = apvts.getRawParameterValue("arpRate")->load();
    int arpPattern = getChoiceIndex("arpPattern");
    arpeggiator.setEnabled(arpOn);
    arpeggiator.setRateHz(arpRate);
    arpeggiator.setPattern(static_cast<Arpeggiator::Pattern>(arpPattern));

    int duty = getChoiceIndex("duty");
    int envVol = static_cast<int>(apvts.getRawParameterValue("envInitVol")->load());
    bool envDir = getChoiceIndex("envDir") == 1;
    int envPer = static_cast<int>(apvts.getRawParameterValue("envPeriod")->load());
    int panMode = getChoiceIndex("pan");

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
    int waveVol = getChoiceIndex("waveVolume");
    int wavePreset = getChoiceIndex("wavePreset");
    wave.setVolumeCode(waveVol);
    wave.loadPreset(wavePreset);
    wave.setPan(panMode);

    // Noise channel
    int nClkShift = static_cast<int>(apvts.getRawParameterValue("noiseClockShift")->load());
    int nDivisor = static_cast<int>(apvts.getRawParameterValue("noiseDivisor")->load());
    bool nWidth = apvts.getRawParameterValue("noiseWidth")->load() > 0.5f;
    int nEnvVol = static_cast<int>(apvts.getRawParameterValue("noiseEnvInitVol")->load());
    bool nEnvDir = getChoiceIndex("noiseEnvDir") == 1;
    int nEnvPer = static_cast<int>(apvts.getRawParameterValue("noiseEnvPeriod")->load());
    noise.setClockShift(nClkShift);
    noise.setDivisorCode(nDivisor);
    noise.setWidthMode(nWidth);
    noise.setEnvelope(nEnvVol, nEnvDir, nEnvPer);
    noise.setPan(panMode);
}

// Trigger a note on the active channel(s) based on channelMode
void triggerNoteOn_impl(GBCSynthProcessor* self, int midiNote, float velocity,
                        int activeChannel, int channelMode,
                        PulseChannel& pulse1, PulseChannel& pulse2,
                        WaveChannel& wave, NoiseChannel& noise)
{
    (void)self;
    auto fire = [&](int ch)
    {
        switch (ch)
        {
            case 0: pulse1.noteOn(midiNoteToPulsePeriod(midiNote), velocity); break;
            case 1: pulse2.noteOn(midiNoteToPulsePeriod(midiNote), velocity); break;
            case 2: wave.noteOn(midiNoteToWavePeriod(midiNote), velocity); break;
            case 3: noise.noteOn(0, velocity); break;
            default: break;
        }
    };

    if (channelMode == 1) // Stack
    {
        for (int ch = 0; ch < 4; ++ch)
            fire(ch);
    }
    else // Single
    {
        fire(activeChannel);
    }
}

void triggerNoteOff_impl(int activeChannel, int channelMode,
                         PulseChannel& pulse1, PulseChannel& pulse2,
                         WaveChannel& wave, NoiseChannel& noise)
{
    auto fire = [&](int ch)
    {
        switch (ch)
        {
            case 0: pulse1.noteOff(); break;
            case 1: pulse2.noteOff(); break;
            case 2: wave.noteOff(); break;
            case 3: noise.noteOff(); break;
            default: break;
        }
    };

    if (channelMode == 1)
    {
        for (int ch = 0; ch < 4; ++ch) fire(ch);
    }
    else
    {
        fire(activeChannel);
    }
}

void GBCSynthProcessor::handleMidiEvent(const juce::MidiMessage& msg)
{
    if (msg.isNoteOn())
    {
        int midiNote = msg.getNoteNumber();
        float velocity = msg.getFloatVelocity();

        updateChannelParameters();

        noteTriggered.store(true);

        // If arpeggiator is on, just track the held note — arp drives actual playback
        if (arpeggiator.isEnabled())
        {
            arpeggiator.addNote(midiNote);
            return;
        }

        currentNoteNumber = midiNote;
        triggerNoteOn_impl(this, midiNote, velocity, activeChannel, channelMode,
                           pulse1, pulse2, wave, noise);
    }
    else if (msg.isNoteOff())
    {
        int midiNote = msg.getNoteNumber();

        if (arpeggiator.isEnabled())
        {
            arpeggiator.removeNote(midiNote);
            if (!arpeggiator.hasNotes())
            {
                triggerNoteOff_impl(activeChannel, channelMode,
                                    pulse1, pulse2, wave, noise);
                arpCurrentNote = -1;
            }
            return;
        }

        // Last-note priority: only release if this matches the current note
        if (midiNote != currentNoteNumber)
            return;

        currentNoteNumber = -1;
        triggerNoteOff_impl(activeChannel, channelMode, pulse1, pulse2, wave, noise);
    }
}

void GBCSynthProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    buffer.clear();

    float masterVol = apvts.getRawParameterValue("masterVolume")->load();

    updateChannelParameters();

    auto* leftChannel = buffer.getWritePointer(0);
    auto* rightChannel = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

    // Helper lambda to render one sample at the given index
    auto renderSample = [&](int idx)
    {
        // Arp: every sample, check if it's time to fire a step
        int arpNote = arpeggiator.tick();
        if (arpNote >= 0)
        {
            // Release previous arp note
            if (arpCurrentNote >= 0)
                triggerNoteOff_impl(activeChannel, channelMode, pulse1, pulse2, wave, noise);

            arpCurrentNote = arpNote;
            triggerNoteOn_impl(this, arpNote, 1.0f, activeChannel, channelMode,
                               pulse1, pulse2, wave, noise);
            noteTriggered.store(true);
        }

        float monoLeft = 0.0f;
        float monoRight = 0.0f;

        auto mixChannel = [&](int ch)
        {
            float s = 0.0f;
            float lGain = 1.0f, rGain = 1.0f;
            switch (ch)
            {
                case 0: s = pulse1.processSample(); pulse1.getStereoGain(lGain, rGain); break;
                case 1: s = pulse2.processSample(); pulse2.getStereoGain(lGain, rGain); break;
                case 2: s = wave.processSample();   wave.getStereoGain(lGain, rGain);   break;
                case 3: s = noise.processSample();  noise.getStereoGain(lGain, rGain);  break;
            }
            monoLeft  += s * lGain;
            monoRight += s * rGain;
        };

        if (channelMode == 1)  // Stack — mix all 4
        {
            for (int ch = 0; ch < 4; ++ch) mixChannel(ch);
            // Prevent clipping when summing 4 channels
            monoLeft *= 0.5f;
            monoRight *= 0.5f;
        }
        else
        {
            mixChannel(activeChannel);
        }

        leftChannel[idx] = monoLeft * masterVol;
        if (rightChannel)
            rightChannel[idx] = monoRight * masterVol;

        // Average for oscilloscope
        waveformBuffer[waveformWritePos] = (monoLeft + monoRight) * 0.5f;
        waveformWritePos = (waveformWritePos + 1) % WAVEFORM_BUFFER_SIZE;
    };

    int sampleIndex = 0;

    for (const auto metadata : midiMessages)
    {
        const int eventPos = metadata.samplePosition;

        while (sampleIndex < eventPos)
            renderSample(sampleIndex++);

        handleMidiEvent(metadata.getMessage());
    }

    while (sampleIndex < buffer.getNumSamples())
        renderSample(sampleIndex++);

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

void GBCSynthProcessor::setCurrentProgram(int index)
{
    if (index >= 0 && index < PresetManager::getNumPresets())
    {
        currentPreset = index;
        PresetManager::applyPreset(apvts, index);
    }
}

const juce::String GBCSynthProcessor::getProgramName(int index)
{
    return PresetManager::getPresetName(index);
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
