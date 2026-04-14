#pragma once

#include <JuceHeader.h>
#include "Theme.h"

// Compatibility shim — old code uses RetroColors::name. These now delegate
// to the runtime-switchable Theme so day/night mode works everywhere.
namespace RetroColors
{
    inline juce::Colour background()     { return Theme::background();     }
    inline juce::Colour panelBg()        { return Theme::panelBg();        }
    inline juce::Colour panelBorder()    { return Theme::panelBorder();    }
    inline juce::Colour gbcGreen()       { return Theme::primary();        }
    inline juce::Colour gbcDarkGreen()   { return Theme::primaryDark();    }
    inline juce::Colour purple()         { return Theme::secondary();      }
    inline juce::Colour amber()          { return Theme::amber();          }
    inline juce::Colour textPrimary()    { return Theme::textPrimary();    }
    inline juce::Colour textSecondary()  { return Theme::textSecondary();  }
    inline juce::Colour knobFill()       { return Theme::knobFill();       }
    inline juce::Colour knobTrack()      { return Theme::knobTrack();      }
    inline juce::Colour activeTab()      { return Theme::primary();        }
    inline juce::Colour inactiveTab()    { return Theme::knobFill();       }
    inline juce::Colour waveformGlow()   { return Theme::waveformGlow();   }
    inline juce::Colour headerBg()       { return Theme::headerBg();       }
}

class RetroLookAndFeel : public juce::LookAndFeel_V4
{
public:
    RetroLookAndFeel();

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override;

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          juce::Slider::SliderStyle style, juce::Slider& slider) override;

    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox& box) override;

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;

    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                        bool shouldDrawButtonAsHighlighted,
                        bool shouldDrawButtonAsDown) override;

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override;

    void drawLabel(juce::Graphics& g, juce::Label& label) override;

    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;

    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                           bool isSeparator, bool isActive, bool isHighlighted,
                           bool isTicked, bool hasSubMenu,
                           const juce::String& text, const juce::String& shortcutKeyText,
                           const juce::Drawable* icon, const juce::Colour* textColour) override;

private:
    juce::Font getPixelFont(float height) const;
};
