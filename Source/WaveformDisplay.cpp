#include "WaveformDisplay.h"
#include "PluginProcessor.h"

WaveformDisplay::WaveformDisplay(GBCSynthProcessor& processor)
    : processorRef(processor)
{
    startTimerHz(30);  // 30 FPS refresh
}

WaveformDisplay::~WaveformDisplay()
{
    stopTimer();
}

void WaveformDisplay::timerCallback()
{
    // Copy samples from ring buffer for display
    int writePos = processorRef.waveformWritePos;
    int readPos = (writePos - static_cast<int>(displayBuffer.size()) + GBCSynthProcessor::WAVEFORM_BUFFER_SIZE)
                  % GBCSynthProcessor::WAVEFORM_BUFFER_SIZE;

    for (size_t i = 0; i < displayBuffer.size(); ++i)
    {
        displayBuffer[i] = processorRef.waveformBuffer[(readPos + static_cast<int>(i)) % GBCSynthProcessor::WAVEFORM_BUFFER_SIZE];
    }

    repaint();
}

void WaveformDisplay::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Background
    g.setColour(juce::Colour(0xff0a0a14));
    g.fillRoundedRectangle(bounds, 4.0f);

    // Border
    g.setColour(RetroColors::panelBorder);
    g.drawRoundedRectangle(bounds.reduced(0.5f), 4.0f, 1.0f);

    auto drawArea = bounds.reduced(4.0f);
    auto centreY = drawArea.getCentreY();
    auto halfHeight = drawArea.getHeight() / 2.0f;

    // Grid lines
    g.setColour(RetroColors::panelBorder.withAlpha(0.3f));
    g.drawLine(drawArea.getX(), centreY, drawArea.getRight(), centreY, 0.5f);
    g.drawLine(drawArea.getCentreX(), drawArea.getY(), drawArea.getCentreX(), drawArea.getBottom(), 0.5f);

    // Draw waveform glow (wider, lower opacity pass)
    {
        juce::Path glowPath;
        auto xScale = drawArea.getWidth() / float(displayBuffer.size() - 1);

        glowPath.startNewSubPath(drawArea.getX(), centreY - displayBuffer[0] * halfHeight * 0.9f);
        for (size_t i = 1; i < displayBuffer.size(); ++i)
        {
            float x = drawArea.getX() + float(i) * xScale;
            float y = centreY - displayBuffer[i] * halfHeight * 0.9f;
            glowPath.lineTo(x, y);
        }

        g.setColour(RetroColors::waveformGlow.withAlpha(0.15f));
        g.strokePath(glowPath, juce::PathStrokeType(5.0f));
    }

    // Draw waveform main line (sharp)
    {
        juce::Path wavePath;
        auto xScale = drawArea.getWidth() / float(displayBuffer.size() - 1);

        wavePath.startNewSubPath(drawArea.getX(), centreY - displayBuffer[0] * halfHeight * 0.9f);
        for (size_t i = 1; i < displayBuffer.size(); ++i)
        {
            float x = drawArea.getX() + float(i) * xScale;
            float y = centreY - displayBuffer[i] * halfHeight * 0.9f;
            wavePath.lineTo(x, y);
        }

        g.setColour(RetroColors::waveformGlow);
        g.strokePath(wavePath, juce::PathStrokeType(1.5f));
    }

    // CRT scanline overlay
    drawScanlines(g, drawArea);
}

void WaveformDisplay::drawScanlines(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    g.setColour(juce::Colour(0x08000000));  // Very subtle dark lines
    for (float y = bounds.getY(); y < bounds.getBottom(); y += 2.0f)
    {
        g.drawLine(bounds.getX(), y, bounds.getRight(), y, 1.0f);
    }
}
