# GBC Synth

A VST3 instrument plugin that faithfully emulates the **Game Boy Color** 4-channel APU (Audio Processing Unit), themed after *Dragon Warrior III* GBC. Plays inside FL Studio 2025 on Windows 64-bit, with cross-platform macOS support from the same codebase.

Built with C++ and the [JUCE](https://juce.com/) framework.

---

## Table of Contents

- [What This Is](#what-this-is)
- [Features](#features)
- [Build from Source](#build-from-source)
- [Installation (FL Studio)](#installation-fl-studio)
- [Architecture](#architecture)
- [Source Layout](#source-layout)
- [DSP Details](#dsp-details)
- [GUI Design](#gui-design)
- [Testing](#testing)
- [CI/CD](#cicd)

---

## What This Is

The Game Boy Color has exactly **4 sound channels**, each with strict hardware constraints. This limited palette is what gives GBC music — and Dragon Warrior 3 GBC in particular — its iconic "chiptune" timbre. This plugin emulates those 4 channels accurately, including their quirks (11-bit period quantization, 4-bit volume envelopes, LFSR noise) and exposes them as a playable VST3 instrument with MIDI input.

The plugin operates in **single-channel mode**: one GBC channel is active at a time, selected via the GUI. This matches the original hardware where each channel was its own independent voice.

---

## Features

### Four GBC APU Channels

| Channel | Type | Key Features |
|---------|------|--------------|
| **CH1 Pulse 1** | Square wave | 4 duty cycles (12.5 / 25 / 50 / 75%), 4-bit volume envelope, frequency sweep |
| **CH2 Pulse 2** | Square wave | Same as CH1 but no sweep |
| **CH3 Wave** | Wavetable | 32-nibble (4-bit) custom waveform, 4 volume levels, 5 built-in presets |
| **CH4 Noise** | LFSR noise | 15-bit white / 7-bit metallic modes, configurable clock shift + divisor |

### Dragon Warrior 3 Presets

Six built-in presets tuned to match DW3 sound character:
- Field Theme
- Battle Theme
- Town Theme
- Dungeon Theme
- Percussion Hit
- White Noise SFX

Exposed via both the in-plugin preset buttons **and** FL Studio's native preset browser (top-right dropdown).

### Modern Retro Hybrid GUI

Dark navy background (`#1a1a2e`) with GBC-era accents:
- GBC green (`#9bbc0f`) for primary controls
- Purple (`#8b5cf6`) and amber (`#f59e0b`) for highlights
- Custom pixel-style rotary knobs with notch marks
- Real-time oscilloscope waveform display with phosphor-green glow and CRT scanline overlay
- Interactive 32-bar wave RAM editor for drawing custom CH3 waveforms
- Tabbed channel selector with per-channel control visibility

### Hardware-Accurate Details

- 11-bit period register quantization (MIDI pitches snap to the nearest hardware frequency — this is authentic GBC behavior)
- 64 Hz envelope clock for volume decay
- 128 Hz sweep clock for CH1 pitch sweep
- First-order high-pass filter for DC-offset removal (matches GBC hardware high-pass behavior)
- Last-note-priority monophonic behavior per channel

---

## Build from Source

### Requirements

| Tool | Version | Notes |
|------|---------|-------|
| **CMake** | ≥ 3.22 | Bundled with VS 2022 Build Tools |
| **Visual Studio 2022** | Any edition | Windows only |
| **Xcode** | 15+ | macOS only |
| **Git** | Any recent | Required for JUCE submodule |

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

### Build Outputs

After a successful build, the `build/GBCSynth_artefacts/Release/` directory contains:

```
VST3/
  GBC Synth.vst3/        ← Standard VST3 bundle (for most DAWs)
  GBC Synth.dll          ← Flat DLL copy (for FL Studio VST2 search paths)
Standalone/
  GBC Synth.exe          ← Standalone host application (no DAW required)
```

---

## Installation (FL Studio)

FL Studio's VST3 scanning is hardcoded to `C:\Program Files\Common Files\VST3\`. The custom VST search paths under **Options > File Settings** are for **VST2 plugins only**.

From an elevated PowerShell:

```powershell
Copy-Item -Recurse -Force `
  ".\build\GBCSynth_artefacts\Release\VST3\GBC Synth.vst3" `
  "C:\Program Files\Common Files\VST3\"
```

Then in FL Studio:
1. **Options > File Settings > Manage Plugins**
2. Make sure **VST3** is checked
3. Click **Find installed plugins > Start scan**
4. Look for **GBC Synth** under Browser > Plugin Database > Generators

Alternatively, run `scripts/install-vst.ps1` as Administrator to install both the bundle and a flat DLL copy automatically.

---

## Architecture

### High-Level Flow

```
┌──────────────────┐   MIDI    ┌───────────────────────┐   Audio  ┌─────────────┐
│  FL Studio /     │──────────▶│  GBCSynthProcessor    │─────────▶│  DAW Output │
│  MIDI Source     │           │  (AudioProcessor)     │          │             │
└──────────────────┘           └───────────────────────┘          └─────────────┘
                                        │
                                        │ reads/writes
                                        ▼
                               ┌───────────────────────┐
                               │  APVTS Parameters     │
                               │  (all knobs/combos)   │
                               └───────────────────────┘
                                        │
                                        │ binds to
                                        ▼
                               ┌───────────────────────┐
                               │  GBCSynthEditor       │
                               │  (AudioProcessorEditor│
                               │   with custom GUI)    │
                               └───────────────────────┘
```

### Data Flow Inside `processBlock()`

```
1. Read master volume from APVTS
2. Call updateChannelParameters() → reads all APVTS params, pushes to channels
3. For each sample position [0 ... numSamples):
     a. If a MIDI event is scheduled here:
          - On note-on: record currentNoteNumber, call <channel>.noteOn()
          - On note-off: only fire if it matches currentNoteNumber (last-note priority)
     b. Dispatch to active channel: sample = channel.processSample()
     c. Apply per-channel stereo pan
     d. Multiply by master volume
     e. Write to output buffer L/R
     f. Copy to ring buffer for the GUI oscilloscope
4. Apply first-order HPF to each output channel (DC removal)
```

Since the GBC is monophonic per channel and this plugin uses single-channel mode, only one voice is active at a time. Multiple held keys use **last-note-priority**: the most recently pressed key wins, and releasing a different key doesn't cut the sound.

---

## Source Layout

```
WyvernAdventure3Vst/
├── CMakeLists.txt                  ← Root build config, JUCE setup, post-build DLL copy
├── JUCE/                           ← JUCE 8.x framework (git submodule)
├── Source/
│   ├── GBCConstants.h              ← Clock rate, duty tables, noise divisors, freq formulas
│   ├── GBCChannel.h                ← Abstract base class for all 4 channels
│   ├── PulseChannel.h/.cpp         ← CH1/CH2 square wave DSP
│   ├── WaveChannel.h/.cpp          ← CH3 wavetable DSP + 5 built-in waveforms
│   ├── NoiseChannel.h/.cpp         ← CH4 LFSR noise DSP
│   ├── PluginProcessor.h/.cpp      ← Main AudioProcessor, APVTS, MIDI handling
│   ├── PluginEditor.h/.cpp         ← Main AudioProcessorEditor (plugin window)
│   ├── RetroLookAndFeel.h/.cpp     ← Custom JUCE LookAndFeel for retro theme
│   ├── WaveformDisplay.h/.cpp      ← Real-time oscilloscope component
│   ├── WaveEditor.h/.cpp           ← Interactive 32-bar wave RAM editor
│   └── PresetManager.h/.cpp        ← DW3 preset definitions and apply-preset logic
├── Tests/
│   ├── CMakeLists.txt              ← Catch2 test runner config
│   ├── PulseChannelTest.cpp        ← Duty cycle, envelope, sweep tests
│   ├── WaveChannelTest.cpp         ← Wavetable read/write, volume code tests
│   ├── NoiseChannelTest.cpp        ← LFSR sequence, width mode tests
│   └── MidiMappingTest.cpp         ← MIDI note → period conversion, constants
├── scripts/
│   └── install-vst.ps1             ← Admin install script (copies to system VST3 dir)
├── .github/workflows/
│   ├── build.yml                   ← CI: Windows + macOS build, test on every push
│   └── release.yml                 ← On tag push: build, package, create GitHub Release
└── README.md                       ← This file
```

### Class Hierarchy

```
juce::AudioProcessor
  └─ GBCSynthProcessor
       │ owns:
       ├─ AudioProcessorValueTreeState (apvts)
       ├─ PulseChannel pulse1   ─┐
       ├─ PulseChannel pulse2    │  all inherit from
       ├─ WaveChannel wave       │  GBCChannel
       └─ NoiseChannel noise    ─┘

juce::AudioProcessorEditor
  └─ GBCSynthEditor
       │ uses:
       ├─ RetroLookAndFeel      ← custom paint for knobs, buttons, combos
       ├─ WaveformDisplay       ← Timer-driven oscilloscope
       ├─ WaveEditor            ← Mouse-interactive nibble editor
       └─ dozens of juce::Slider / juce::ComboBox / juce::TextButton
         bound to APVTS parameters via *Attachment objects
```

---

## DSP Details

### Pulse Channels (CH1 / CH2)

Each pulse channel generates a square wave with a selectable duty cycle using an 8-step lookup table:

```
DUTY_TABLE[4][8] = {
  {0,0,0,0,0,0,0,1},  // 12.5% — 1 high step
  {1,0,0,0,0,0,0,1},  // 25%   — 2 high steps
  {1,0,0,0,1,1,1,1},  // 50%   — 5 high steps (authentic GBC "50%")
  {0,1,1,1,1,1,1,0}   // 75%   — 6 high steps
};
```

**Frequency formula.** The GBC timer runs at `clock / 4` = 1.048576 MHz. With an 11-bit period register and 8 duty steps per cycle:

```
audible_freq = GBC_CLOCK_HZ / (32 * (2048 - period))
             = 4194304 / (32 * (2048 - period))
```

Getting this formula wrong was a real bug during development — using `/(4 * ...)` instead of `/(32 * ...)` made every MIDI note clamp to the same clamped period, playing 512 Hz regardless of key.

**Envelope.** A 4-bit volume register (0–15) decrements (or increments) once every `envPeriod` ticks of a 64 Hz clock. An `envPeriod` of 0 disables the envelope. When decay completes and the note is released, the channel deactivates.

**Sweep (CH1 only).** Every `sweepPeriod` ticks of a 128 Hz clock, the period register is updated by `period ± (period >> sweepShift)`. Overflow beyond 2047 silences the channel — an authentic GBC behavior used for "laser" and "explosion" effects.

### Wave Channel (CH3)

32 consecutive 4-bit samples stored packed as 16 bytes (`waveRam[16]`), upper nibble first:

```
byte[0]:  sample[0]  sample[1]
byte[1]:  sample[2]  sample[3]
...
byte[15]: sample[30] sample[31]
```

The sample pointer advances at `GBC_CLOCK_HZ / (2 * (2048 - period))` per second, cycling through the 32 samples. The 4-bit value is then right-shifted by 0 / 1 / 2 bits for the 100% / 50% / 25% volume settings (mute is a shift of 4, which zeroes everything).

Output is centered at 7.5 and normalized to −1.0 / +1.0 float range.

Five built-in presets: Triangle, Sawtooth, Square, Sine (4-bit quantized), DW3 Bass.

### Noise Channel (CH4)

A 15-bit LFSR (Linear Feedback Shift Register) with classic GBC feedback:

```cpp
int bit = (lfsr ^ (lfsr >> 1)) & 1;   // XOR bits 0 and 1
lfsr = (lfsr >> 1) | (bit << 14);     // shift right, feed back to bit 14
if (narrowMode)                       // 7-bit "metallic" mode
    lfsr = (lfsr & ~(1 << 6)) | (bit << 6);
```

Clocked at `GBC_CLOCK_HZ / (divisor << clockShift)` where `divisor` is looked up in a table of 8 values `{8, 16, 32, 48, 64, 80, 96, 112}`. Each LFSR bit output is scaled by the envelope volume and written as ±1.

### DC-Offset Filter

A first-order IIR high-pass filter removes DC drift caused by summing the 4 channels:

```cpp
float output = input - prevInput + alpha * prevOutput;  // alpha ≈ 0.999
```

This matches the behavior of the real GBC DAC output coupling capacitor.

---

## GUI Design

### Layout

```
┌──────────────────────────────────────────────────────────┐
│  GBC SYNTH                    Dragon Warrior III Edition │ ← Header (50px)
├──────────────────────────────────────────────────────────┤
│  [ PULSE 1 ] [ PULSE 2 ] [ WAVE ] [ NOISE ]              │ ← Channel tabs (40px)
├──────────────────────────────────────────────────────────┤
│                                                          │
│    (Per-channel controls — knobs, combos, toggles)       │ ← Controls (200px)
│                                                          │
├──────────────────────────────────────────────────────────┤
│  ╭──╮    ╭──╮    ╭──╮    ╭──╮                            │
│──╯  ╰────╯  ╰────╯  ╰────╯  ╰──── (CRT waveform) ────────│ ← Oscilloscope (100px)
├──────────────────────────────────────────────────────────┤
│  [Field] [Battle] [Town] [Dungeon] [Percuss] [Noise SFX] │ ← DW3 presets (28px)
├──────────────────────────────────────────────────────────┤
│  PAN: [Center ▾]   MASTER: ████████░░                    │ ← Output (35px)
└──────────────────────────────────────────────────────────┘
    800 px wide × 590 px tall
```

### Custom Painting

**`RetroLookAndFeel`** overrides JUCE's default painting for:
- Rotary knobs — dark knob body, GBC-green arc indicator, notch marks around the ring
- Linear sliders — amber chunky thumb with pixel-block appearance
- Combo boxes — dark fill, green arrow
- Buttons — knob-fill background with subtle green border glow on hover
- Toggle buttons — pixel-style X mark inside a box
- Popup menus — retro-themed with green-on-dark highlighting

**`WaveformDisplay`** is a `Timer` at 30 Hz that pulls the last 512 samples from a ring buffer in the processor and draws:
1. A semi-transparent wide glow pass (opacity 0.15, stroke width 5)
2. A sharp 1.5 px phosphor-green line on top
3. A CRT scanline overlay (2 px semi-transparent dark lines)

**`WaveEditor`** is a mouse-interactive grid of 32 vertical bars. Clicking or dragging sets the corresponding nibble value, and a callback fires so the processor's APVTS stays in sync.

### Keyboard Focus

The editor calls `setWantsKeyboardFocus(false)` and `setMouseClickGrabsKeyboardFocus(false)` on itself and recursively on every child component. This prevents the plugin from capturing keystrokes, so FL Studio's typing-keyboard-to-piano feature continues to work even while interacting with the plugin window.

---

## Testing

24 unit tests using [Catch2](https://github.com/catchorg/Catch2) cover the core DSP:

| Test File | What it verifies |
|-----------|------------------|
| `PulseChannelTest.cpp` | Duty cycle output range, envelope decay, sweep overflow silencing, noteOff behavior |
| `WaveChannelTest.cpp` | Peak-to-peak range per volume code, preset load validity, nibble set/get roundtrip |
| `NoiseChannelTest.cpp` | LFSR output variety, 15-bit vs 7-bit differentiation, envelope decay |
| `MidiMappingTest.cpp` | MIDI → period conversion, monotonic frequency increase, duty table hi-count |

Run from the build directory:

```bash
ctest --test-dir build --build-config Release --output-on-failure
```

Or run the test executable directly:

```bash
./build/Tests/Release/GBCSynthTests.exe
```

---

## CI/CD

### `build.yml` (on every push / PR)

- Matrix: Windows (VS 2022) + macOS (AppleClang)
- `fail-fast: false` so both platforms always report
- Steps: checkout with submodules → configure CMake → build Release → run `ctest` → upload VST3 + Standalone artifacts

### `release.yml` (on tag push `v*`)

Same build matrix plus:
- Package the VST3 bundle, flat `.dll`, and standalone `.exe` into per-platform `.zip` archives
- Create a GitHub Release at the tag with auto-generated release notes
- Attach all four zips (Windows VST3, Windows Standalone, macOS VST3, macOS Standalone)

To cut a release:

```bash
git tag -a v1.1.0 -m "Release v1.1.0"
git push origin v1.1.0
```

The Release will appear at `https://github.com/NickSanft/WyvernAdventure3Vst/releases`.

---

## License

MIT
