#include "PluginEditor.h"

GBCSynthEditor::GBCSynthEditor(GBCSynthProcessor& p)
    : AudioProcessorEditor(&p),
      processorRef(p),
      waveformDisplay(p)
{
    setLookAndFeel(&retroLookAndFeel);
    setSize(800, 560);

    // --- Channel select tabs ---
    const juce::StringArray tabNames{ "PULSE 1", "PULSE 2", "WAVE", "NOISE" };
    for (int i = 0; i < 4; ++i)
    {
        channelTabs[i].setButtonText(tabNames[i]);
        channelTabs[i].setClickingTogglesState(true);
        channelTabs[i].setRadioGroupId(1001);
        channelTabs[i].onClick = [this, i]()
        {
            channelSelectHidden.setSelectedItemIndex(i, juce::sendNotification);
            updateChannelVisibility(i);
        };
        addAndMakeVisible(channelTabs[i]);
    }
    channelTabs[0].setToggleState(true, juce::dontSendNotification);

    // Hidden combo for APVTS binding
    channelSelectHidden.addItemList(juce::StringArray{ "Pulse 1", "Pulse 2", "Wave", "Noise" }, 1);
    channelSelectAttachment = std::make_unique<ComboBoxAttachment>(
        processorRef.getAPVTS(), "channelSelect", channelSelectHidden);

    // --- Pulse controls ---
    dutyCombo.addItemList(juce::StringArray{ "12.5%", "25%", "50%", "75%" }, 1);
    addAndMakeVisible(dutyCombo);
    dutyAttachment = std::make_unique<ComboBoxAttachment>(processorRef.getAPVTS(), "duty", dutyCombo);

    setupRotarySlider(envVolSlider);
    envVolAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "envInitVol", envVolSlider);

    envDirCombo.addItemList(juce::StringArray{ "Down", "Up" }, 1);
    addAndMakeVisible(envDirCombo);
    envDirAttachment = std::make_unique<ComboBoxAttachment>(processorRef.getAPVTS(), "envDir", envDirCombo);

    setupRotarySlider(envPeriodSlider);
    envPeriodAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "envPeriod", envPeriodSlider);

    setupRotarySlider(sweepPeriodSlider);
    sweepPeriodAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "sweepPeriod", sweepPeriodSlider);

    setupRotarySlider(sweepShiftSlider);
    sweepShiftAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "sweepShift", sweepShiftSlider);

    addAndMakeVisible(sweepNegateToggle);
    sweepNegateAttachment = std::make_unique<ButtonAttachment>(processorRef.getAPVTS(), "sweepNegate", sweepNegateToggle);

    // --- Wave controls ---
    waveVolumeCombo.addItemList(juce::StringArray{ "Mute", "100%", "50%", "25%" }, 1);
    addAndMakeVisible(waveVolumeCombo);
    waveVolumeAttachment = std::make_unique<ComboBoxAttachment>(processorRef.getAPVTS(), "waveVolume", waveVolumeCombo);

    wavePresetCombo.addItemList(juce::StringArray{ "Triangle", "Sawtooth", "Square", "Sine", "DW3 Bass" }, 1);
    addAndMakeVisible(wavePresetCombo);
    wavePresetAttachment = std::make_unique<ComboBoxAttachment>(processorRef.getAPVTS(), "wavePreset", wavePresetCombo);

    addAndMakeVisible(waveEditor);

    // --- Noise controls ---
    setupRotarySlider(noiseClockShiftSlider);
    noiseClockShiftAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "noiseClockShift", noiseClockShiftSlider);

    setupRotarySlider(noiseDivisorSlider);
    noiseDivisorAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "noiseDivisor", noiseDivisorSlider);

    addAndMakeVisible(noiseWidthToggle);
    noiseWidthAttachment = std::make_unique<ButtonAttachment>(processorRef.getAPVTS(), "noiseWidth", noiseWidthToggle);

    setupRotarySlider(noiseEnvVolSlider);
    noiseEnvVolAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "noiseEnvInitVol", noiseEnvVolSlider);

    noiseEnvDirCombo.addItemList(juce::StringArray{ "Down", "Up" }, 1);
    addAndMakeVisible(noiseEnvDirCombo);
    noiseEnvDirAttachment = std::make_unique<ComboBoxAttachment>(processorRef.getAPVTS(), "noiseEnvDir", noiseEnvDirCombo);

    setupRotarySlider(noiseEnvPeriodSlider);
    noiseEnvPeriodAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "noiseEnvPeriod", noiseEnvPeriodSlider);

    // --- Common controls ---
    panCombo.addItemList(juce::StringArray{ "Left", "Center", "Right" }, 1);
    addAndMakeVisible(panCombo);
    panAttachment = std::make_unique<ComboBoxAttachment>(processorRef.getAPVTS(), "pan", panCombo);

    masterVolumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    masterVolumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    addAndMakeVisible(masterVolumeSlider);
    masterVolumeAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "masterVolume", masterVolumeSlider);

    // --- Labels ---
    setupLabel(dutyLabel, "DUTY");
    setupLabel(envVolLabel, "VOL");
    setupLabel(envPeriodLabel, "PERIOD");
    setupLabel(envDirLabel, "DIR");
    setupLabel(sweepPeriodLabel, "SWP PER");
    setupLabel(sweepShiftLabel, "SWP SFT");
    setupLabel(waveVolLabel, "VOLUME");
    setupLabel(wavePresetLabel, "PRESET");
    setupLabel(noiseClockLabel, "CLK SHIFT");
    setupLabel(noiseDivLabel, "DIVISOR");
    setupLabel(noiseEnvVolLabel, "VOL");
    setupLabel(noiseEnvPeriodLabel, "PERIOD");
    setupLabel(noiseEnvDirLabel, "DIR");
    setupLabel(panLabel, "PAN");
    setupLabel(masterVolLabel, "MASTER");

    // Waveform display
    addAndMakeVisible(waveformDisplay);

    // Show correct channel on startup
    updateChannelVisibility(0);
}

GBCSynthEditor::~GBCSynthEditor()
{
    setLookAndFeel(nullptr);
}

void GBCSynthEditor::setupLabel(juce::Label& label, const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, RetroColors::textSecondary);
    label.setFont(juce::Font(juce::FontOptions(11.0f, juce::Font::bold)));
    addAndMakeVisible(label);
}

void GBCSynthEditor::setupRotarySlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 16);
    addAndMakeVisible(slider);
}

void GBCSynthEditor::updateChannelVisibility(int channel)
{
    // Pulse controls
    bool showPulse = (channel == 0 || channel == 1);
    bool showSweep = (channel == 0);
    dutyCombo.setVisible(showPulse);
    envVolSlider.setVisible(showPulse);
    envDirCombo.setVisible(showPulse);
    envPeriodSlider.setVisible(showPulse);
    dutyLabel.setVisible(showPulse);
    envVolLabel.setVisible(showPulse);
    envDirLabel.setVisible(showPulse);
    envPeriodLabel.setVisible(showPulse);

    sweepPeriodSlider.setVisible(showSweep);
    sweepShiftSlider.setVisible(showSweep);
    sweepNegateToggle.setVisible(showSweep);
    sweepPeriodLabel.setVisible(showSweep);
    sweepShiftLabel.setVisible(showSweep);

    // Wave controls
    bool showWave = (channel == 2);
    waveVolumeCombo.setVisible(showWave);
    wavePresetCombo.setVisible(showWave);
    waveEditor.setVisible(showWave);
    waveVolLabel.setVisible(showWave);
    wavePresetLabel.setVisible(showWave);

    // Noise controls
    bool showNoise = (channel == 3);
    noiseClockShiftSlider.setVisible(showNoise);
    noiseDivisorSlider.setVisible(showNoise);
    noiseWidthToggle.setVisible(showNoise);
    noiseEnvVolSlider.setVisible(showNoise);
    noiseEnvDirCombo.setVisible(showNoise);
    noiseEnvPeriodSlider.setVisible(showNoise);
    noiseClockLabel.setVisible(showNoise);
    noiseDivLabel.setVisible(showNoise);
    noiseEnvVolLabel.setVisible(showNoise);
    noiseEnvDirLabel.setVisible(showNoise);
    noiseEnvPeriodLabel.setVisible(showNoise);

    repaint();
}

void GBCSynthEditor::drawHeader(juce::Graphics& g)
{
    auto headerBounds = getLocalBounds().removeFromTop(50);

    // Header background
    g.setColour(RetroColors::headerBg);
    g.fillRect(headerBounds);

    // Decorative pixel border
    g.setColour(RetroColors::gbcDarkGreen);
    g.fillRect(headerBounds.getX(), headerBounds.getBottom() - 3, headerBounds.getWidth(), 3);
    g.setColour(RetroColors::gbcGreen);
    g.fillRect(headerBounds.getX(), headerBounds.getBottom() - 2, headerBounds.getWidth(), 1);

    // Title — pixel-style block letters
    g.setColour(RetroColors::gbcGreen);
    g.setFont(juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 26.0f, juce::Font::bold)));
    g.drawText("GBC SYNTH", headerBounds.reduced(15, 0).removeFromLeft(250),
               juce::Justification::centredLeft, false);

    // Subtitle
    g.setColour(RetroColors::purple);
    g.setFont(juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 12.0f, juce::Font::plain)));
    g.drawText("Dragon Warrior III Edition", headerBounds.reduced(15, 0).removeFromRight(300),
               juce::Justification::centredRight, false);

    // Status LED (note activity indicator)
    auto ledBounds = juce::Rectangle<float>(float(headerBounds.getRight() - 35), 15.0f, 10.0f, 10.0f);
    g.setColour(RetroColors::gbcDarkGreen);
    g.fillEllipse(ledBounds);
    // In a future update, this could pulse based on note activity
}

void GBCSynthEditor::drawChannelPanel(juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& title)
{
    auto boundsF = bounds.toFloat();

    // Panel background
    g.setColour(RetroColors::panelBg);
    g.fillRoundedRectangle(boundsF, 6.0f);

    // Panel border
    g.setColour(RetroColors::panelBorder);
    g.drawRoundedRectangle(boundsF.reduced(0.5f), 6.0f, 1.0f);

    // Title bar
    auto titleBar = bounds.removeFromTop(22);
    g.setColour(RetroColors::panelBorder.withAlpha(0.3f));
    g.fillRect(titleBar.getX() + 1, titleBar.getY() + 1, titleBar.getWidth() - 2, titleBar.getHeight());

    g.setColour(RetroColors::amber);
    g.setFont(juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 11.0f, juce::Font::bold)));
    g.drawText(title, titleBar.reduced(8, 0), juce::Justification::centredLeft, false);
}

void GBCSynthEditor::paint(juce::Graphics& g)
{
    // Main background
    g.fillAll(RetroColors::background);

    // Header
    drawHeader(g);

    // Channel controls panel
    auto controlsArea = getLocalBounds().reduced(10).withTrimmedTop(95);
    auto channelTitle = juce::StringArray{ "PULSE 1", "PULSE 2", "WAVE", "NOISE" };
    int ch = static_cast<int>(processorRef.getAPVTS().getRawParameterValue("channelSelect")->load());
    drawChannelPanel(g, controlsArea.removeFromTop(200), channelTitle[ch]);

    // Bottom panel for common controls
    auto bottomArea = getLocalBounds().reduced(10);
    bottomArea = bottomArea.removeFromBottom(45);
    drawChannelPanel(g, bottomArea, "OUTPUT");
}

void GBCSynthEditor::resized()
{
    auto area = getLocalBounds();

    // Header: 50px
    area.removeFromTop(50);
    area.reduce(10, 5);

    // Channel tabs: 40px height
    auto tabArea = area.removeFromTop(40);
    int tabWidth = tabArea.getWidth() / 4;
    for (int i = 0; i < 4; ++i)
        channelTabs[i].setBounds(tabArea.removeFromLeft(tabWidth).reduced(2));

    area.removeFromTop(5);

    // Channel controls area: ~200px
    auto controlsArea = area.removeFromTop(200).reduced(5, 25);

    // --- Pulse layout ---
    {
        auto pulseArea = controlsArea;
        int knobSize = 60;
        int comboW = 80;
        int comboH = 24;
        int spacing = 10;

        // Duty cycle section
        auto dutyArea = pulseArea.removeFromLeft(comboW + spacing);
        dutyLabel.setBounds(dutyArea.removeFromTop(14));
        dutyCombo.setBounds(dutyArea.removeFromTop(comboH).reduced(0, 2));

        // Envelope section
        auto envArea = pulseArea.removeFromLeft(3 * (knobSize + spacing));
        auto envRow = envArea;
        auto envLabels = envRow.removeFromTop(14);

        auto volArea = envRow.removeFromLeft(knobSize + spacing);
        envVolLabel.setBounds(envLabels.removeFromLeft(knobSize + spacing));
        envVolSlider.setBounds(volArea.removeFromTop(knobSize));

        auto dirArea = envRow.removeFromLeft(comboW + spacing);
        envDirLabel.setBounds(envLabels.removeFromLeft(comboW + spacing));
        envDirCombo.setBounds(dirArea.removeFromTop(comboH).reduced(0, 2));

        auto perArea = envRow.removeFromLeft(knobSize + spacing);
        envPeriodLabel.setBounds(envLabels.removeFromLeft(knobSize + spacing));
        envPeriodSlider.setBounds(perArea.removeFromTop(knobSize));

        // Sweep section (CH1 only)
        pulseArea.removeFromLeft(spacing);
        auto sweepArea = pulseArea;
        auto sweepLabels = sweepArea.removeFromTop(14);

        auto swpPerArea = sweepArea.removeFromLeft(knobSize + spacing);
        sweepPeriodLabel.setBounds(sweepLabels.removeFromLeft(knobSize + spacing));
        sweepPeriodSlider.setBounds(swpPerArea.removeFromTop(knobSize));

        auto swpShiftArea = sweepArea.removeFromLeft(knobSize + spacing);
        sweepShiftLabel.setBounds(sweepLabels.removeFromLeft(knobSize + spacing));
        sweepShiftSlider.setBounds(swpShiftArea.removeFromTop(knobSize));

        sweepNegateToggle.setBounds(sweepArea.removeFromLeft(90).removeFromTop(24));
    }

    // --- Wave layout ---
    {
        auto waveArea = controlsArea;
        int comboW = 100;

        auto topRow = waveArea.removeFromTop(30);
        waveVolLabel.setBounds(topRow.removeFromLeft(50));
        waveVolumeCombo.setBounds(topRow.removeFromLeft(comboW).reduced(0, 3));
        topRow.removeFromLeft(20);
        wavePresetLabel.setBounds(topRow.removeFromLeft(50));
        wavePresetCombo.setBounds(topRow.removeFromLeft(comboW).reduced(0, 3));

        waveArea.removeFromTop(5);
        waveEditor.setBounds(waveArea.reduced(0, 5));
    }

    // --- Noise layout ---
    {
        auto noiseArea = controlsArea;
        int knobSize = 60;
        int comboW = 80;
        int comboH = 24;
        int spacing = 10;

        auto noiseLabels = noiseArea.removeFromTop(14);

        auto clkArea = noiseArea.removeFromLeft(knobSize + spacing);
        noiseClockLabel.setBounds(noiseLabels.removeFromLeft(knobSize + spacing));
        noiseClockShiftSlider.setBounds(clkArea.removeFromTop(knobSize));

        auto divArea = noiseArea.removeFromLeft(knobSize + spacing);
        noiseDivLabel.setBounds(noiseLabels.removeFromLeft(knobSize + spacing));
        noiseDivisorSlider.setBounds(divArea.removeFromTop(knobSize));

        noiseWidthToggle.setBounds(noiseArea.removeFromLeft(110).removeFromTop(24));
        noiseArea.removeFromLeft(spacing);

        // Noise envelope
        noiseLabels = noiseArea.removeFromTop(0);  // Labels already consumed
        auto nVolArea = noiseArea.removeFromLeft(knobSize + spacing);
        noiseEnvVolLabel.setBounds(nVolArea.removeFromBottom(14));
        noiseEnvVolSlider.setBounds(nVolArea.removeFromTop(knobSize));

        auto nDirArea = noiseArea.removeFromLeft(comboW + spacing);
        noiseEnvDirLabel.setBounds(nDirArea.removeFromBottom(14));
        noiseEnvDirCombo.setBounds(nDirArea.removeFromTop(comboH).reduced(0, 2));

        auto nPerArea = noiseArea.removeFromLeft(knobSize + spacing);
        noiseEnvPeriodLabel.setBounds(nPerArea.removeFromBottom(14));
        noiseEnvPeriodSlider.setBounds(nPerArea.removeFromTop(knobSize));
    }

    area.removeFromTop(5);

    // Waveform display
    auto waveformArea = area.removeFromTop(100);
    waveformDisplay.setBounds(waveformArea.reduced(0, 5));

    area.removeFromTop(5);

    // Bottom controls: pan + master volume
    auto bottomArea = area.removeFromTop(35);
    panLabel.setBounds(bottomArea.removeFromLeft(35));
    panCombo.setBounds(bottomArea.removeFromLeft(85).reduced(0, 5));
    bottomArea.removeFromLeft(20);
    masterVolLabel.setBounds(bottomArea.removeFromLeft(55));
    masterVolumeSlider.setBounds(bottomArea.reduced(0, 5));
}
