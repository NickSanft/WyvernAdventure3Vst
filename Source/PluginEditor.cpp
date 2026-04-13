#include "PluginEditor.h"

GBCSynthEditor::GBCSynthEditor(GBCSynthProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(800, 500);
}

GBCSynthEditor::~GBCSynthEditor() {}

void GBCSynthEditor::paint(juce::Graphics& g)
{
    // Dark retro background
    g.fillAll(juce::Colour(0xff1a1a2e));

    // Title text (placeholder — full themed GUI in Milestone 6)
    g.setColour(juce::Colour(0xff9bbc0f)); // GBC green
    g.setFont(juce::FontOptions(28.0f, juce::Font::bold));
    g.drawFittedText("GBC SYNTH", getLocalBounds().removeFromTop(60),
                     juce::Justification::centred, 1);

    g.setColour(juce::Colour(0xff8b5cf6)); // Purple accent
    g.setFont(juce::FontOptions(14.0f));
    g.drawFittedText("Dragon Warrior III Edition — Milestone 1: Scaffold Complete",
                     getLocalBounds().reduced(20),
                     juce::Justification::centred, 1);
}

void GBCSynthEditor::resized()
{
    // Layout will be implemented in Milestone 6
}
