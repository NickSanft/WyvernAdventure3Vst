#include "WaveEditor.h"

WaveEditor::WaveEditor()
{
    // Default triangle waveform
    for (int i = 0; i < 16; ++i) nibbles[i] = static_cast<uint8_t>(i);
    for (int i = 0; i < 16; ++i) nibbles[16 + i] = static_cast<uint8_t>(15 - i);
}

void WaveEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Background
    g.setColour(Theme::waveformBg());
    g.fillRoundedRectangle(bounds, 4.0f);
    g.setColour(RetroColors::panelBorder());
    g.drawRoundedRectangle(bounds.reduced(0.5f), 4.0f, 1.0f);

    auto drawArea = bounds.reduced(4.0f);
    float barWidth = drawArea.getWidth() / 32.0f;
    float maxBarHeight = drawArea.getHeight();

    // Draw each nibble as a vertical bar
    for (int i = 0; i < 32; ++i)
    {
        float x = drawArea.getX() + i * barWidth;
        float barHeight = (nibbles[i] / 15.0f) * maxBarHeight;
        float barY = drawArea.getBottom() - barHeight;

        // Filled bar with pixel-block appearance
        auto barColour = RetroColors::gbcGreen().withAlpha(0.6f + 0.4f * (nibbles[i] / 15.0f));
        g.setColour(barColour);
        g.fillRect(x + 1.0f, barY, barWidth - 2.0f, barHeight);

        // Top cap (brighter)
        if (nibbles[i] > 0)
        {
            g.setColour(RetroColors::gbcGreen());
            g.fillRect(x + 1.0f, barY, barWidth - 2.0f, 2.0f);
        }

        // Subtle grid line
        g.setColour(RetroColors::panelBorder().withAlpha(0.2f));
        g.drawLine(x, drawArea.getY(), x, drawArea.getBottom(), 0.5f);
    }

    // Label
    g.setColour(RetroColors::textSecondary());
    g.setFont(juce::Font(juce::FontOptions(10.0f)));
    g.drawText("WAVE RAM", drawArea.removeFromTop(12), juce::Justification::centred);
}

void WaveEditor::mouseDown(const juce::MouseEvent& event)
{
    setNibbleFromMouse(event);
}

void WaveEditor::mouseDrag(const juce::MouseEvent& event)
{
    setNibbleFromMouse(event);
}

void WaveEditor::setNibbleFromMouse(const juce::MouseEvent& event)
{
    auto drawArea = getLocalBounds().toFloat().reduced(4.0f);
    float barWidth = drawArea.getWidth() / 32.0f;

    int index = static_cast<int>((event.position.x - drawArea.getX()) / barWidth);
    index = juce::jlimit(0, 31, index);

    float normalizedY = 1.0f - (event.position.y - drawArea.getY()) / drawArea.getHeight();
    normalizedY = juce::jlimit(0.0f, 1.0f, normalizedY);
    int value = static_cast<int>(normalizedY * 15.0f + 0.5f);
    value = juce::jlimit(0, 15, value);

    if (nibbles[index] != static_cast<uint8_t>(value))
    {
        nibbles[index] = static_cast<uint8_t>(value);
        repaint();

        if (onNibbleChanged)
            onNibbleChanged(index, value);
    }
}

void WaveEditor::setWaveform(const uint8_t* nibbles32)
{
    for (int i = 0; i < 32; ++i)
        nibbles[i] = nibbles32[i];
    repaint();
}

void WaveEditor::getWaveform(uint8_t* nibbles32) const
{
    for (int i = 0; i < 32; ++i)
        nibbles32[i] = nibbles[i];
}
