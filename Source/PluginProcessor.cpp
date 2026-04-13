#include "PluginProcessor.h"
#include "PluginEditor.h"

GBCSynthProcessor::GBCSynthProcessor()
    : AudioProcessor(BusesProperties()
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

GBCSynthProcessor::~GBCSynthProcessor() {}

void GBCSynthProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    currentSampleRate = sampleRate;
}

void GBCSynthProcessor::releaseResources() {}

void GBCSynthProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;

    // Clear output — no sound yet (placeholder for Milestone 2+)
    buffer.clear();
}

juce::AudioProcessorEditor* GBCSynthProcessor::createEditor()
{
    return new GBCSynthEditor(*this);
}

void GBCSynthProcessor::getStateInformation(juce::MemoryBlock& /*destData*/)
{
    // State save will be implemented with APVTS in Milestone 2
}

void GBCSynthProcessor::setStateInformation(const void* /*data*/, int /*sizeInBytes*/)
{
    // State restore will be implemented with APVTS in Milestone 2
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GBCSynthProcessor();
}
