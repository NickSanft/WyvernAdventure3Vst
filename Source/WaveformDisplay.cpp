#include "WaveformDisplay.h"
#include "PluginProcessor.h"
#include <cmath>

WaveformDisplay::WaveformDisplay(GBCSynthProcessor& processor)
    : processorRef(processor)
{
    startTimerHz(30);
}

WaveformDisplay::~WaveformDisplay()
{
    stopTimer();
}

void WaveformDisplay::timerCallback()
{
    int writePos = processorRef.waveformWritePos;
    int readPos = (writePos - static_cast<int>(displayBuffer.size()) + GBCSynthProcessor::WAVEFORM_BUFFER_SIZE)
                  % GBCSynthProcessor::WAVEFORM_BUFFER_SIZE;

    float peak = 0.0f;
    for (size_t i = 0; i < displayBuffer.size(); ++i)
    {
        float s = processorRef.waveformBuffer[(readPos + static_cast<int>(i)) % GBCSynthProcessor::WAVEFORM_BUFFER_SIZE];
        displayBuffer[i] = s;
        peak = std::max(peak, std::abs(s));
    }

    // Smooth the peak level for a nicer glow animation
    currentPeakLevel = currentPeakLevel * 0.7f + peak * 0.3f;

    // Consume the note-triggered flag from the processor
    if (processorRef.noteTriggered.exchange(false))
        triggerFlash();

    // Decay the flash alpha (30 FPS → ~200ms fade)
    if (flashAlpha > 0.0f)
        flashAlpha = std::max(0.0f, flashAlpha - 0.15f);

    repaint();
}

void WaveformDisplay::triggerFlash()
{
    flashAlpha = 1.0f;
}

void WaveformDisplay::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    g.setColour(Theme::waveformBg());
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(RetroColors::panelBorder());
    g.drawRoundedRectangle(bounds.reduced(0.5f), 4.0f, 1.0f);

    auto drawArea = bounds.reduced(4.0f);
    auto centreY = drawArea.getCentreY();
    auto halfHeight = drawArea.getHeight() / 2.0f;

    g.setColour(RetroColors::panelBorder().withAlpha(0.3f));
    g.drawLine(drawArea.getX(), centreY, drawArea.getRight(), centreY, 0.5f);
    g.drawLine(drawArea.getCentreX(), drawArea.getY(), drawArea.getCentreX(), drawArea.getBottom(), 0.5f);

    // Dynamic glow intensity — brighter at louder volumes
    float glowAlpha = 0.05f + 0.35f * std::min(1.0f, currentPeakLevel * 1.5f);
    float glowWidth = 3.0f + 5.0f * std::min(1.0f, currentPeakLevel);

    // Wide glow pass
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
        g.setColour(RetroColors::waveformGlow().withAlpha(glowAlpha));
        g.strokePath(glowPath, juce::PathStrokeType(glowWidth));
    }

    // Sharp main line
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
        g.setColour(RetroColors::waveformGlow());
        g.strokePath(wavePath, juce::PathStrokeType(1.5f));
    }

    // CRT scanline overlay
    drawScanlines(g, drawArea);

    // Note-triggered flash — brief pixel-art star burst overlay
    if (flashAlpha > 0.0f)
    {
        g.setColour(RetroColors::amber().withAlpha(flashAlpha * 0.25f));
        g.fillRoundedRectangle(bounds, 4.0f);

        float cx = drawArea.getCentreX();
        float cy = drawArea.getCentreY();
        float r = 16.0f * flashAlpha;
        g.setColour(juce::Colours::white.withAlpha(flashAlpha));
        for (int i = 0; i < 6; ++i)
        {
            float a = i * juce::MathConstants<float>::twoPi / 6.0f;
            g.drawLine(cx, cy, cx + std::cos(a) * r * 2.0f, cy + std::sin(a) * r * 2.0f, 2.0f);
        }
    }
}

void WaveformDisplay::drawScanlines(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    g.setColour(juce::Colour(0x08000000));
    for (float y = bounds.getY(); y < bounds.getBottom(); y += 2.0f)
        g.drawLine(bounds.getX(), y, bounds.getRight(), y, 1.0f);
}
