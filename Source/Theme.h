#pragma once

#include <JuceHeader.h>

// Runtime-switchable theme. Night mode = dark chiptune aesthetic (default).
// Day mode = lighter "overworld" palette inspired by DW3's field map.
namespace Theme
{
    // Global theme state. Set from the editor via setDayMode().
    inline bool& dayModeRef()
    {
        static bool dayMode = false;
        return dayMode;
    }

    inline bool isDayMode() { return dayModeRef(); }
    inline void setDayMode(bool d) { dayModeRef() = d; }

    // --- Color accessors — each returns a different colour based on theme ---

    inline juce::Colour background()
    {
        return isDayMode() ? juce::Colour(0xffe6f0d8) : juce::Colour(0xff1a1a2e);
    }
    inline juce::Colour panelBg()
    {
        return isDayMode() ? juce::Colour(0xffcdd8be) : juce::Colour(0xff16213e);
    }
    inline juce::Colour panelBorder()
    {
        return isDayMode() ? juce::Colour(0xff7a8c5c) : juce::Colour(0xff0f3460);
    }
    inline juce::Colour primary()  // "gbcGreen" equivalent — accent colour
    {
        return isDayMode() ? juce::Colour(0xff306230) : juce::Colour(0xff9bbc0f);
    }
    inline juce::Colour primaryDark()
    {
        return isDayMode() ? juce::Colour(0xff1a3a1a) : juce::Colour(0xff306230);
    }
    inline juce::Colour secondary()  // purple/magenta accent
    {
        return isDayMode() ? juce::Colour(0xff6b4bc4) : juce::Colour(0xff8b5cf6);
    }
    inline juce::Colour amber()
    {
        return isDayMode() ? juce::Colour(0xffc2660a) : juce::Colour(0xfff59e0b);
    }
    inline juce::Colour textPrimary()
    {
        return isDayMode() ? juce::Colour(0xff1e2813) : juce::Colour(0xffe2e8f0);
    }
    inline juce::Colour textSecondary()
    {
        return isDayMode() ? juce::Colour(0xff5a6548) : juce::Colour(0xff94a3b8);
    }
    inline juce::Colour knobFill()
    {
        return isDayMode() ? juce::Colour(0xffb5c2a2) : juce::Colour(0xff334155);
    }
    inline juce::Colour knobTrack()
    {
        return isDayMode() ? juce::Colour(0xff8a9678) : juce::Colour(0xff1e293b);
    }
    inline juce::Colour waveformGlow()
    {
        return isDayMode() ? juce::Colour(0xff306230) : juce::Colour(0xff39ff14);
    }
    inline juce::Colour waveformBg()
    {
        return isDayMode() ? juce::Colour(0xffb8c7a0) : juce::Colour(0xff0a0a14);
    }
    inline juce::Colour headerBg()
    {
        return isDayMode() ? juce::Colour(0xff9bbc0f) : juce::Colour(0xff0d1117);
    }
}
