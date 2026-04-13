#include "RetroLookAndFeel.h"

RetroLookAndFeel::RetroLookAndFeel()
{
    setColour(juce::ComboBox::backgroundColourId, RetroColors::knobFill);
    setColour(juce::ComboBox::textColourId, RetroColors::textPrimary);
    setColour(juce::ComboBox::outlineColourId, RetroColors::panelBorder);
    setColour(juce::ComboBox::arrowColourId, RetroColors::gbcGreen);
    setColour(juce::PopupMenu::backgroundColourId, RetroColors::panelBg);
    setColour(juce::PopupMenu::textColourId, RetroColors::textPrimary);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, RetroColors::gbcDarkGreen);
    setColour(juce::PopupMenu::highlightedTextColourId, RetroColors::gbcGreen);
    setColour(juce::Label::textColourId, RetroColors::textPrimary);
    setColour(juce::Slider::textBoxTextColourId, RetroColors::textPrimary);
    setColour(juce::Slider::textBoxBackgroundColourId, RetroColors::knobTrack);
    setColour(juce::Slider::textBoxOutlineColourId, RetroColors::panelBorder);
}

juce::Font RetroLookAndFeel::getPixelFont(float height) const
{
    return juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), height, juce::Font::bold));
}

void RetroLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                         juce::Slider& /*slider*/)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // Outer ring (track)
    g.setColour(RetroColors::knobTrack);
    g.fillEllipse(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f);

    // Active arc
    juce::Path arcPath;
    arcPath.addCentredArc(centreX, centreY, radius - 2.0f, radius - 2.0f,
                          0.0f, rotaryStartAngle, angle, true);
    g.setColour(RetroColors::gbcGreen);
    g.strokePath(arcPath, juce::PathStrokeType(3.0f));

    // Knob body
    auto knobRadius = radius * 0.65f;
    g.setColour(RetroColors::knobFill);
    g.fillEllipse(centreX - knobRadius, centreY - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);

    // Knob border
    g.setColour(RetroColors::panelBorder);
    g.drawEllipse(centreX - knobRadius, centreY - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f, 1.5f);

    // Pointer line
    auto pointerLength = knobRadius * 0.8f;
    auto pointerThickness = 2.5f;
    juce::Path pointerPath;
    pointerPath.addRectangle(-pointerThickness * 0.5f, -pointerLength, pointerThickness, pointerLength);
    pointerPath.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    g.setColour(RetroColors::gbcGreen);
    g.fillPath(pointerPath);

    // Notch marks (stepped positions)
    g.setColour(RetroColors::textSecondary.withAlpha(0.3f));
    for (int i = 0; i <= 8; ++i)
    {
        float notchAngle = rotaryStartAngle + (float(i) / 8.0f) * (rotaryEndAngle - rotaryStartAngle);
        float notchInner = radius - 1.0f;
        float notchOuter = radius + 2.0f;
        float nx1 = centreX + notchInner * std::cos(notchAngle - juce::MathConstants<float>::halfPi);
        float ny1 = centreY + notchInner * std::sin(notchAngle - juce::MathConstants<float>::halfPi);
        float nx2 = centreX + notchOuter * std::cos(notchAngle - juce::MathConstants<float>::halfPi);
        float ny2 = centreY + notchOuter * std::sin(notchAngle - juce::MathConstants<float>::halfPi);
        g.drawLine(nx1, ny1, nx2, ny2, 1.0f);
    }
}

void RetroLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPos, float /*minSliderPos*/, float /*maxSliderPos*/,
                                         juce::Slider::SliderStyle style, juce::Slider& /*slider*/)
{
    if (style == juce::Slider::LinearHorizontal)
    {
        auto trackY = y + height / 2;
        auto trackHeight = 6;

        // Track background
        g.setColour(RetroColors::knobTrack);
        g.fillRoundedRectangle(float(x), float(trackY - trackHeight / 2),
                               float(width), float(trackHeight), 3.0f);

        // Filled portion
        g.setColour(RetroColors::gbcGreen);
        g.fillRoundedRectangle(float(x), float(trackY - trackHeight / 2),
                               sliderPos - float(x), float(trackHeight), 3.0f);

        // Thumb (chunky pixel block)
        auto thumbWidth = 12.0f;
        auto thumbHeight = 18.0f;
        g.setColour(RetroColors::amber);
        g.fillRect(sliderPos - thumbWidth / 2.0f, float(trackY) - thumbHeight / 2.0f,
                   thumbWidth, thumbHeight);
        g.setColour(RetroColors::panelBorder);
        g.drawRect(sliderPos - thumbWidth / 2.0f, float(trackY) - thumbHeight / 2.0f,
                   thumbWidth, thumbHeight, 1.0f);
    }
    else
    {
        LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos,
                                          0, 0, style, const_cast<juce::Slider&>(static_cast<const juce::Slider&>(*static_cast<juce::Slider*>(nullptr))));
    }
}

void RetroLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool /*isButtonDown*/,
                                     int /*buttonX*/, int /*buttonY*/, int /*buttonW*/, int /*buttonH*/,
                                     juce::ComboBox& /*box*/)
{
    g.setColour(RetroColors::knobFill);
    g.fillRoundedRectangle(0, 0, float(width), float(height), 4.0f);
    g.setColour(RetroColors::panelBorder);
    g.drawRoundedRectangle(0.5f, 0.5f, float(width - 1), float(height - 1), 4.0f, 1.0f);

    // Arrow
    auto arrowX = float(width - 20);
    auto arrowY = float(height) * 0.5f;
    juce::Path arrow;
    arrow.addTriangle(arrowX, arrowY - 3.0f, arrowX + 8.0f, arrowY - 3.0f, arrowX + 4.0f, arrowY + 3.0f);
    g.setColour(RetroColors::gbcGreen);
    g.fillPath(arrow);
}

void RetroLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                             const juce::Colour& /*backgroundColour*/,
                                             bool shouldDrawButtonAsHighlighted,
                                             bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);

    juce::Colour bgColour = RetroColors::knobFill;
    if (shouldDrawButtonAsDown)
        bgColour = RetroColors::gbcDarkGreen;
    else if (shouldDrawButtonAsHighlighted)
        bgColour = RetroColors::knobFill.brighter(0.1f);

    g.setColour(bgColour);
    g.fillRoundedRectangle(bounds, 4.0f);

    // Border with subtle glow on hover
    auto borderColour = shouldDrawButtonAsHighlighted ? RetroColors::gbcGreen : RetroColors::panelBorder;
    g.setColour(borderColour);
    g.drawRoundedRectangle(bounds, 4.0f, 1.5f);
}

void RetroLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                       bool /*shouldDrawButtonAsHighlighted*/,
                                       bool shouldDrawButtonAsDown)
{
    auto font = getPixelFont(13.0f);
    g.setFont(font);

    auto colour = button.getToggleState() ? RetroColors::gbcGreen : RetroColors::textPrimary;
    if (shouldDrawButtonAsDown)
        colour = RetroColors::amber;

    g.setColour(colour);
    g.drawFittedText(button.getButtonText(), button.getLocalBounds().reduced(4),
                     juce::Justification::centred, 1);
}

void RetroLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                         bool shouldDrawButtonAsHighlighted,
                                         bool /*shouldDrawButtonAsDown*/)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto boxSize = 16.0f;
    auto boxX = bounds.getX() + 4.0f;
    auto boxY = bounds.getCentreY() - boxSize / 2.0f;

    // Checkbox box
    g.setColour(RetroColors::knobFill);
    g.fillRect(boxX, boxY, boxSize, boxSize);
    g.setColour(shouldDrawButtonAsHighlighted ? RetroColors::gbcGreen : RetroColors::panelBorder);
    g.drawRect(boxX, boxY, boxSize, boxSize, 1.5f);

    // Check mark (pixel-style X)
    if (button.getToggleState())
    {
        g.setColour(RetroColors::gbcGreen);
        auto inset = 3.0f;
        g.drawLine(boxX + inset, boxY + inset, boxX + boxSize - inset, boxY + boxSize - inset, 2.0f);
        g.drawLine(boxX + boxSize - inset, boxY + inset, boxX + inset, boxY + boxSize - inset, 2.0f);
    }

    // Label text
    g.setColour(RetroColors::textPrimary);
    g.setFont(getPixelFont(13.0f));
    g.drawFittedText(button.getButtonText(),
                     juce::Rectangle<int>(int(boxX + boxSize + 8), int(bounds.getY()),
                                          int(bounds.getWidth() - boxSize - 16), int(bounds.getHeight())),
                     juce::Justification::centredLeft, 1);
}

void RetroLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.setColour(label.findColour(juce::Label::textColourId));
    g.setFont(getPixelFont(label.getFont().getHeight()));
    g.drawFittedText(label.getText(), label.getLocalBounds().reduced(2),
                     label.getJustificationType(), 1);
}

void RetroLookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int width, int height)
{
    g.setColour(RetroColors::panelBg);
    g.fillRect(0, 0, width, height);
    g.setColour(RetroColors::panelBorder);
    g.drawRect(0, 0, width, height, 1);
}

void RetroLookAndFeel::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                                          bool isSeparator, bool isActive, bool isHighlighted,
                                          bool isTicked, bool /*hasSubMenu*/,
                                          const juce::String& text, const juce::String& /*shortcutKeyText*/,
                                          const juce::Drawable* /*icon*/, const juce::Colour* /*textColour*/)
{
    if (isSeparator)
    {
        g.setColour(RetroColors::panelBorder);
        g.drawLine(float(area.getX() + 4), float(area.getCentreY()),
                   float(area.getRight() - 4), float(area.getCentreY()));
        return;
    }

    if (isHighlighted)
    {
        g.setColour(RetroColors::gbcDarkGreen);
        g.fillRect(area);
    }

    auto textColourToUse = isActive ? (isHighlighted ? RetroColors::gbcGreen : RetroColors::textPrimary)
                                    : RetroColors::textSecondary;
    g.setColour(textColourToUse);
    g.setFont(getPixelFont(14.0f));

    auto textArea = area.reduced(8, 0);
    if (isTicked)
    {
        g.setColour(RetroColors::gbcGreen);
        g.drawText(juce::CharPointer_UTF8("\xe2\x96\xb8"), textArea.removeFromLeft(20),
                   juce::Justification::centredLeft, false);  // ▸ arrow
    }
    else
    {
        textArea.removeFromLeft(20);
    }

    g.setColour(textColourToUse);
    g.drawFittedText(text, textArea, juce::Justification::centredLeft, 1);
}
