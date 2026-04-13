# GBC Synth

A VST3 instrument plugin that faithfully emulates the **Game Boy Color** 4-channel APU (Audio Processing Unit), themed after *Dragon Warrior III* GBC.

Built with [JUCE](https://juce.com/) and C++. Targets **FL Studio 2025** on Windows 64-bit, with cross-platform macOS support.

## Features

- **4 authentic GBC channels:**
  - **Pulse 1 (CH1)** — Square wave with 4 duty cycles (12.5/25/50/75%), volume envelope, frequency sweep
  - **Pulse 2 (CH2)** — Square wave with duty cycles and volume envelope (no sweep)
  - **Wave (CH3)** — 32-nibble wavetable with 5 built-in presets and a drawable wave editor
  - **Noise (CH4)** — LFSR pseudo-random noise with 15-bit and 7-bit (metallic) modes

- **Modern Retro Hybrid GUI** — Dark theme with GBC-era green/purple/amber accents, CRT scanline waveform display with phosphor glow, custom pixel-style knobs

- **Dragon Warrior III presets** — Field Theme, Battle Theme, Town Theme, Dungeon Theme, Percussion Hit, White Noise SFX

- **Single-channel MIDI mode** — Select one GBC channel at a time, play via MIDI

- **Hardware-accurate details** — 11-bit period register quantization, 64 Hz envelope clock, 128 Hz sweep clock, first-order DC-offset high-pass filter

## Build from Source

### Requirements

| Tool | Version |
|------|---------|
| CMake | >= 3.22 |
| Visual Studio 2022 (Windows) | Build Tools or Community |
| Xcode 15+ (macOS) | From Mac App Store |
| Git | Any recent version |

### Steps

```bash
git clone --recursive https://github.com/NickSanft/WyvernAdventure3Vst.git
cd WyvernAdventure3Vst

# Windows
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release

# macOS
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Build output:
- **VST3:** `build/GBCSynth_artefacts/Release/VST3/GBC Synth.vst3`
- **Standalone:** `build/GBCSynth_artefacts/Release/Standalone/GBC Synth.exe`

### Running Tests

```bash
ctest --test-dir build --build-config Release --output-on-failure
```

## FL Studio Setup

1. Copy `GBC Synth.vst3` to `C:\Program Files\Common Files\VST3\`
2. Open FL Studio > **Options > File Settings > Manage Plugins**
3. Click **Find installed plugins** > **Start scan**
4. Find **GBC Synth** in Browser > Plugin Database > Generators

## License

MIT
