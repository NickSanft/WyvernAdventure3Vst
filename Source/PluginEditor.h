#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class GBCSynthEditor : public juce::AudioProcessorEditor
{
public:
    explicit GBCSynthEditor(GBCSynthProcessor&);
    ~GBCSynthEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    GBCSynthProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GBCSynthEditor)
};
