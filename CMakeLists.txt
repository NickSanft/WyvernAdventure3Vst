cmake_minimum_required(VERSION 3.22)
project(GBCSynth VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(JUCE)

juce_add_plugin(GBCSynth
    COMPANY_NAME "WyvernStudios"
    PLUGIN_MANUFACTURER_CODE "GBCS"
    PLUGIN_CODE "Gbc1"
    FORMATS VST3 Standalone
    PRODUCT_NAME "GBC Synth"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(GBCSynth PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/PulseChannel.cpp
    Source/WaveChannel.cpp
    Source/NoiseChannel.cpp
    Source/RetroLookAndFeel.cpp
    Source/WaveformDisplay.cpp
    Source/WaveEditor.cpp
    Source/PresetManager.cpp
)

juce_generate_juce_header(GBCSynth)

target_compile_definitions(GBCSynth PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(GBCSynth PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags
)

# Tests
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Tests)
endif()
