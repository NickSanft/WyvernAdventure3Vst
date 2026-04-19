#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "GBCConstants.h"

GBCSynthProcessor::GBCSynthProcessor()
    : AudioProcessor(BusesProperties()
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    // Allocate voice pools. Each pulse1 voice has sweep enabled (CH1 behaviour),
    // pulse2 voices do not. MAX_VOICES per pool is plenty for chords.
    pulse1Voices.reserve(MAX_VOICES);
    pulse2Voices.reserve(MAX_VOICES);
    waveVoices.reserve(MAX_VOICES);
    noiseVoices.reserve(MAX_VOICES);
    for (int i = 0; i < MAX_VOICES; ++i)
    {
        pulse1Voices.emplace_back(true);   // CH1 has sweep
        pulse2Voices.emplace_back(false);  // CH2 no sweep
        waveVoices.emplace_back();
        noiseVoices.emplace_back();
    }
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
        juce::StringArray{ "Triangle", "Sawtooth", "Square", "Sine", "DW3 Bass",
                           "Half-Sine", "Pulse 25%", "Detuned Saw", "Bell", "Acid Bass" }, 0));

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
    for (auto& v : pulse1Voices) { v.setSampleRate(sampleRate); v.reset(); }
    for (auto& v : pulse2Voices) { v.setSampleRate(sampleRate); v.reset(); }
    for (auto& v : waveVoices)   { v.setSampleRate(sampleRate); v.reset(); }
    for (auto& v : noiseVoices)  { v.setSampleRate(sampleRate); v.reset(); }
    pulse1Slots.fill({});
    pulse2Slots.fill({});
    waveSlots.fill({});
    noiseSlots.fill({});
    voiceCounter = 0;
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
    activeChannel = getChoiceIndex("channelSelect");
    channelMode = getChoiceIndex("channelMode");

    bool vibOn = apvts.getRawParameterValue("vibratoOn")->load() > 0.5f;
    float vibRate = apvts.getRawParameterValue("vibratoRate")->load();
    float vibDepth = apvts.getRawParameterValue("vibratoDepth")->load();

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

    int swpPer = static_cast<int>(apvts.getRawParameterValue("sweepPeriod")->load());
    bool swpNeg = apvts.getRawParameterValue("sweepNegate")->load() > 0.5f;
    int swpShift = static_cast<int>(apvts.getRawParameterValue("sweepShift")->load());

    int waveVol = getChoiceIndex("waveVolume");
    int wavePreset = getChoiceIndex("wavePreset");

    int nClkShift = static_cast<int>(apvts.getRawParameterValue("noiseClockShift")->load());
    int nDivisor = static_cast<int>(apvts.getRawParameterValue("noiseDivisor")->load());
    bool nWidth = apvts.getRawParameterValue("noiseWidth")->load() > 0.5f;
    int nEnvVol = static_cast<int>(apvts.getRawParameterValue("noiseEnvInitVol")->load());
    bool nEnvDir = getChoiceIndex("noiseEnvDir") == 1;
    int nEnvPer = static_cast<int>(apvts.getRawParameterValue("noiseEnvPeriod")->load());

    // Apply shared params to every voice in every pool
    for (auto& v : pulse1Voices)
    {
        v.setDutyCycle(duty);
        v.setEnvelope(envVol, envDir, envPer);
        v.setSweep(swpPer, swpNeg, swpShift);
        v.setPan(panMode);
        v.setVibrato(vibOn, vibRate, vibDepth);
    }
    for (auto& v : pulse2Voices)
    {
        v.setDutyCycle(duty);
        v.setEnvelope(envVol, envDir, envPer);
        v.setPan(panMode);
        v.setVibrato(vibOn, vibRate, vibDepth);
    }
    for (auto& v : waveVoices)
    {
        v.setVolumeCode(waveVol);
        v.loadPreset(wavePreset);
        v.setPan(panMode);
        v.setVibrato(vibOn, vibRate, vibDepth);
    }
    for (auto& v : noiseVoices)
    {
        v.setClockShift(nClkShift);
        v.setDivisorCode(nDivisor);
        v.setWidthMode(nWidth);
        v.setEnvelope(nEnvVol, nEnvDir, nEnvPer);
        v.setPan(panMode);
    }
}

// ---------------------------------------------------------------------------
// Voice allocation helpers (templated by pool type so we can share logic)
// ---------------------------------------------------------------------------
namespace
{
    template <typename VoiceT>
    int allocateVoiceSlot(std::array<GBCSynthProcessor::VoiceSlot, GBCSynthProcessor::MAX_VOICES>& slots,
                          std::vector<VoiceT>& voices,
                          int midiNote,
                          std::uint64_t& counter)
    {
        // 1. Prefer a voice that is no longer producing sound
        for (int i = 0; i < GBCSynthProcessor::MAX_VOICES; ++i)
        {
            if (!voices[i].isActive())
            {
                slots[i].note = midiNote;
                slots[i].age = ++counter;
                return i;
            }
        }

        // 2. Prefer stealing the oldest already-released slot (note == -1)
        int bestIdx = -1;
        std::uint64_t bestAge = UINT64_MAX;
        for (int i = 0; i < GBCSynthProcessor::MAX_VOICES; ++i)
        {
            if (slots[i].note == -1 && slots[i].age < bestAge)
            {
                bestAge = slots[i].age;
                bestIdx = i;
            }
        }

        // 3. Last resort: steal the oldest held voice
        if (bestIdx < 0)
        {
            bestIdx = 0;
            bestAge = slots[0].age;
            for (int i = 1; i < GBCSynthProcessor::MAX_VOICES; ++i)
            {
                if (slots[i].age < bestAge)
                {
                    bestAge = slots[i].age;
                    bestIdx = i;
                }
            }
        }

        slots[bestIdx].note = midiNote;
        slots[bestIdx].age = ++counter;
        return bestIdx;
    }

    int findVoiceForNote(const std::array<GBCSynthProcessor::VoiceSlot, GBCSynthProcessor::MAX_VOICES>& slots,
                         int midiNote)
    {
        for (int i = 0; i < GBCSynthProcessor::MAX_VOICES; ++i)
            if (slots[i].note == midiNote)
                return i;
        return -1;
    }
}

void GBCSynthProcessor::channelNoteOn(int channelIdx, int midiNote, float velocity)
{
    switch (channelIdx)
    {
        case 0:
        {
            int idx = allocateVoiceSlot(pulse1Slots, pulse1Voices, midiNote, voiceCounter);
            pulse1Voices[idx].noteOn(midiNoteToPulsePeriod(midiNote), velocity);
            break;
        }
        case 1:
        {
            int idx = allocateVoiceSlot(pulse2Slots, pulse2Voices, midiNote, voiceCounter);
            pulse2Voices[idx].noteOn(midiNoteToPulsePeriod(midiNote), velocity);
            break;
        }
        case 2:
        {
            int idx = allocateVoiceSlot(waveSlots, waveVoices, midiNote, voiceCounter);
            waveVoices[idx].noteOn(midiNoteToWavePeriod(midiNote), velocity);
            break;
        }
        case 3:
        {
            int idx = allocateVoiceSlot(noiseSlots, noiseVoices, midiNote, voiceCounter);
            noiseVoices[idx].noteOn(0, velocity);
            break;
        }
        default: break;
    }
}

void GBCSynthProcessor::channelNoteOff(int channelIdx, int midiNote)
{
    switch (channelIdx)
    {
        case 0:
        {
            int idx = findVoiceForNote(pulse1Slots, midiNote);
            if (idx >= 0) { pulse1Voices[idx].noteOff(); pulse1Slots[idx].note = -1; }
            break;
        }
        case 1:
        {
            int idx = findVoiceForNote(pulse2Slots, midiNote);
            if (idx >= 0) { pulse2Voices[idx].noteOff(); pulse2Slots[idx].note = -1; }
            break;
        }
        case 2:
        {
            int idx = findVoiceForNote(waveSlots, midiNote);
            if (idx >= 0) { waveVoices[idx].noteOff(); waveSlots[idx].note = -1; }
            break;
        }
        case 3:
        {
            int idx = findVoiceForNote(noiseSlots, midiNote);
            if (idx >= 0) { noiseVoices[idx].noteOff(); noiseSlots[idx].note = -1; }
            break;
        }
        default: break;
    }
}

void GBCSynthProcessor::channelAllNotesOff(int channelIdx)
{
    auto releaseAll = [&](auto& voices, auto& slots)
    {
        for (int i = 0; i < MAX_VOICES; ++i)
        {
            if (slots[i].note != -1)
            {
                voices[i].noteOff();
                slots[i].note = -1;
            }
        }
    };

    switch (channelIdx)
    {
        case 0: releaseAll(pulse1Voices, pulse1Slots); break;
        case 1: releaseAll(pulse2Voices, pulse2Slots); break;
        case 2: releaseAll(waveVoices, waveSlots);     break;
        case 3: releaseAll(noiseVoices, noiseSlots);   break;
    }
}

void GBCSynthProcessor::handleMidiEvent(const juce::MidiMessage& msg)
{
    auto routeOn = [&](int midiNote, float velocity)
    {
        if (channelMode == 1)
            for (int ch = 0; ch < 4; ++ch) channelNoteOn(ch, midiNote, velocity);
        else
            channelNoteOn(activeChannel, midiNote, velocity);
    };
    auto routeOff = [&](int midiNote)
    {
        if (channelMode == 1)
            for (int ch = 0; ch < 4; ++ch) channelNoteOff(ch, midiNote);
        else
            channelNoteOff(activeChannel, midiNote);
    };

    if (msg.isNoteOn())
    {
        int midiNote = msg.getNoteNumber();
        float velocity = msg.getFloatVelocity();

        updateChannelParameters();
        noteTriggered.store(true);

        if (arpeggiator.isEnabled())
        {
            arpeggiator.addNote(midiNote);
            return;
        }

        routeOn(midiNote, velocity);
    }
    else if (msg.isNoteOff())
    {
        int midiNote = msg.getNoteNumber();

        if (arpeggiator.isEnabled())
        {
            arpeggiator.removeNote(midiNote);
            if (!arpeggiator.hasNotes() && arpCurrentNote >= 0)
            {
                routeOff(arpCurrentNote);
                arpCurrentNote = -1;
            }
            return;
        }

        routeOff(midiNote);
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

    // Render one output sample by summing every active voice in every pool
    // that the current channelMode routes to.
    auto renderSample = [&](int idx)
    {
        // Arp: fire a step when due. Uses channelNoteOn/Off so voices allocate
        // properly (polyphony-aware), and routes through channelMode.
        int arpNote = arpeggiator.tick();
        if (arpNote >= 0)
        {
            if (arpCurrentNote >= 0)
            {
                if (channelMode == 1)
                    for (int ch = 0; ch < 4; ++ch) channelNoteOff(ch, arpCurrentNote);
                else
                    channelNoteOff(activeChannel, arpCurrentNote);
            }

            arpCurrentNote = arpNote;

            if (channelMode == 1)
                for (int ch = 0; ch < 4; ++ch) channelNoteOn(ch, arpNote, 1.0f);
            else
                channelNoteOn(activeChannel, arpNote, 1.0f);

            noteTriggered.store(true);
        }

        float monoLeft = 0.0f;
        float monoRight = 0.0f;

        auto mixPool = [&](auto& voices)
        {
            for (auto& v : voices)
            {
                if (!v.isActive()) continue;
                float s = v.processSample();
                float lGain, rGain;
                v.getStereoGain(lGain, rGain);
                monoLeft  += s * lGain;
                monoRight += s * rGain;
            }
        };

        auto mixChannel = [&](int ch)
        {
            switch (ch)
            {
                case 0: mixPool(pulse1Voices); break;
                case 1: mixPool(pulse2Voices); break;
                case 2: mixPool(waveVoices);   break;
                case 3: mixPool(noiseVoices);  break;
            }
        };

        if (channelMode == 1) // Stack — all 4 pools contribute
        {
            for (int ch = 0; ch < 4; ++ch) mixChannel(ch);
            monoLeft *= 0.5f;
            monoRight *= 0.5f;
        }
        else
        {
            mixChannel(activeChannel);
        }

        // Scale down by sqrt(MAX_VOICES) so a chord doesn't clip. 8 voices * ~1.0
        // peak each would sum to 8.0; we want it closer to 1.0.
        constexpr float voiceGain = 1.0f / 2.83f;  // 1/sqrt(8)
        monoLeft  *= voiceGain;
        monoRight *= voiceGain;

        leftChannel[idx] = monoLeft * masterVol;
        if (rightChannel)
            rightChannel[idx] = monoRight * masterVol;

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
