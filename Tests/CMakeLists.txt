cmake_minimum_required(VERSION 3.22)

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.2
)
FetchContent_MakeAvailable(Catch2)

add_executable(GBCSynthTests
    PulseChannelTest.cpp
    WaveChannelTest.cpp
    NoiseChannelTest.cpp
    MidiMappingTest.cpp
    # Include the source files under test
    ${CMAKE_SOURCE_DIR}/Source/PulseChannel.cpp
    ${CMAKE_SOURCE_DIR}/Source/WaveChannel.cpp
    ${CMAKE_SOURCE_DIR}/Source/NoiseChannel.cpp
)

target_include_directories(GBCSynthTests PRIVATE
    ${CMAKE_SOURCE_DIR}/Source
)

target_link_libraries(GBCSynthTests PRIVATE
    Catch2::Catch2WithMain
)

include(CTest)
include(Catch)
catch_discover_tests(GBCSynthTests)
