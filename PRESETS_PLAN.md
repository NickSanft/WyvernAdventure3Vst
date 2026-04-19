# GBC Synth — Preset & Sound Expansion Plan

## Current State

**6 existing presets** (all DW3-themed): Field Theme, Battle Theme, Town Theme, Dungeon Theme, Percussion Hit, White Noise SFX.

**Engine capabilities today:**
- 4 channels (Pulse1, Pulse2, Wave, Noise) with 8-voice polyphony
- Pulse: 4 duties, decay envelope, frequency sweep (CH1)
- Wave: 32-nibble wavetable with 5 presets
- Noise: 15-bit / 7-bit LFSR, envelope
- Vibrato LFO (pitch modulation)
- Arpeggiator (Up/Down/Up-Down/Random)
- Single Mode (one channel) and Stack Mode (all 4)

---

## Strategy: What We Can & Can't Emulate

### NES — mostly translates directly
The NES APU has **2 pulse + 1 triangle + 1 noise + DMC**. Our GBC pulses and noise work nearly identically. NES triangle is covered by our Wave channel with the triangle preset. So NES timbres are **highly achievable** with our existing engine.

### SNES — approximation only
SNES uses **sample-based** playback (SPC700, 8 voices). We can't replay real samples, but the *character* of SNES music — soft leads, rich bass, mellow tone — can be approximated with careful wavetable programming and (ideally) ADSR envelopes.

### What blocks truly authentic sounds
Our engine lacks:
- **ADSR envelope** — only decay. Can't do pads, brass, strings, sustained leads
- **Pitch envelope** — can't do kick drums (pitch drop on attack) or percussive bloops
- **Duty-cycle LFO** — NES leads often PWM the duty for "breathing" timbre
- **More wavetable presets** — only 5 shapes limits variety

---

## Part 1 — Preset Packs

### Pack A: NES Classic (8 presets, all work with current engine)

Inspired by iconic NES games. Each is a single-channel preset for Single Mode.

| Preset | Channel | Duty | Env | Sweep | Vibrato | Arp | Notes |
|--------|---------|------|-----|-------|---------|-----|-------|
| **NES Mario Lead** | Pulse 1 | 50% | Vol 14, Per 3 | off | off | off | Staccato bouncy melody lead |
| **NES Zelda Overworld** | Pulse 1 | 50% | Vol 13, Per 4 | off | on 5Hz 20c | off | Smooth sustained melody with gentle vib |
| **NES Mega Man Lead** | Pulse 2 | 25% | Vol 15, Per 2 | off | off | off | Bright aggressive lead, fast decay |
| **NES Castlevania** | Pulse 1 | 12.5% | Vol 12, Per 5 | off | on 6Hz 40c | off | Thin nasal lead with deep vibrato |
| **NES Triangle Bass** | Wave | n/a | 100% vol, Triangle preset | n/a | off | off | Classic NES triangle bass line |
| **NES Mega Man Bass** | Wave | n/a | 100% vol, Triangle | n/a | off | Up 6Hz | Octave-bouncing triangle bass |
| **NES Snare** | Noise | n/a | Vol 14, Per 1, 7-bit narrow | n/a | n/a | off | Snappy metallic snare |
| **NES Hi-Hat** | Noise | n/a | Vol 8, Per 1, 15-bit wide, clockShift 6 | n/a | n/a | off | Bright short noise burst |

### Pack B: SNES RPG (6 presets, some need ADSR)

Approximations of SNES soundtrack timbres.

| Preset | Channel | Settings | Needs ADSR? |
|--------|---------|----------|-------------|
| **SNES Flute** | Wave | Sine preset, Vol 50%, vibrato 5Hz 25c | ★ for proper attack |
| **SNES Brass** | Wave | Sawtooth, Vol 100%, vibrato 4Hz 15c | ★ for swell |
| **SNES Choir Pad** | Wave | Sine, Vol 50%, vibrato 4Hz 30c | ★ Yes, needs sustained note |
| **SNES Harp Pluck** | Wave | Triangle, Vol 100%, fast decay | no — works today |
| **SNES Music Box** | Wave | Sine, Vol 25%, vibrato 7Hz 10c | no |
| **SNES Bass Pluck** | Wave | DW3 Bass, Vol 100% | no |

### Pack C: Arcade SFX (10 presets — all work today)

Classic game sound effects. Designed to be triggered as one-shot notes.

| Preset | Channel | Technique |
|--------|---------|-----------|
| **Coin Pickup** | Pulse 1 | 25% duty, fast decay, rising arp (up pattern, 24Hz, 2 held notes) |
| **Jump** | Pulse 1 | 50% duty, upward sweep disabled automatically, short envelope |
| **Laser Shot** | Pulse 1 | 12.5% duty, downward sweep shift 3, very fast decay |
| **Power-Up** | Pulse 2 | 25% duty, arp Up 20Hz over 4-note chord |
| **1-Up Jingle** | Pulse 2 | 50% duty, arp Up 16Hz, Stack mode on for chord |
| **Explosion** | Noise | 15-bit wide, clockShift 2, long envelope decay |
| **Hit / Bump** | Noise | 7-bit narrow, clockShift 4, Vol 10, Per 0 (instant off) |
| **Alarm** | Pulse 1 | 50% duty, vibrato 8Hz 80c (extreme), no decay |
| **Menu Select** | Pulse 2 | 50% duty, Vol 12, Per 1 — clean click |
| **Menu Error** | Pulse 1 | 25% duty, downward sweep period 1 shift 2 |

### Pack D: Chiptune Instruments (8 general-purpose presets)

Category-based, not game-specific.

| Preset | Category | Channel | Character |
|--------|----------|---------|-----------|
| **8-Bit Square Lead** | Lead | Pulse 1 | 50% duty, Vol 15, Per 6, slight vibrato |
| **Hollow Lead** | Lead | Pulse 1 | 12.5% duty — thin whistle tone |
| **Fat Lead (Stack)** | Lead | Stack mode | Pulse1 50% + Pulse2 25% + Wave sawtooth, slight detune via pan |
| **Bell Pluck** | Pluck | Wave | DW3 Bass preset, very fast decay |
| **Soft Triangle Bass** | Bass | Wave | Triangle preset, 100% vol |
| **Sawtooth Bass** | Bass | Wave | Sawtooth preset, 100% vol |
| **Arp Melody** | Arp | Pulse 2 | 25% duty, arp Up-Down 12Hz |
| **Pad (Stack)** | Pad | Stack | All channels layered, slow vibrato ★ needs ADSR for true pad |

### Pack E: DW3 Expansion (5 more presets)

Rounding out the Dragon Warrior III theme.

| Preset | Description |
|--------|-------------|
| **DW3 Overworld Reprise** | Pulse 2 25% duty, gentle vibrato |
| **DW3 Cave Echo** | Wave triangle, very slow decay |
| **DW3 Boss Battle** | Stack mode, all channels aggressive |
| **DW3 Victory Fanfare** | Pulse 1 50%, arp Up 16Hz with stack mode |
| **DW3 Game Over** | Wave DW3 Bass, slow descending via manual note input (no sweep) |

**Total new presets: ~37** (up from 6).

---

## Part 2 — Engine Enhancements to Unlock More Sounds

These are optional — each preset pack lists which presets need which enhancements. Most of Pack A & C work today with zero engine changes.

### Priority 1 — ADSR Envelope (biggest impact)

**Current:** single "decay" envelope that ramps volume down (or up) at a fixed period.
**New:** `attack` (ms), `decay` (ms), `sustain` (0–15 level), `release` (ms).

**Unlocks:** pads, strings, brass, proper flute, choir — everything sustained. Probably doubles the range of believable presets.

**Scope:** ~50 lines in each channel class + 4 new APVTS params. Back-compat: map existing "envPeriod" to decay rate and keep "envInitVol" as peak.

### Priority 2 — Pitch Envelope

**Current:** pitch is static (apart from sweep + vibrato).
**New:** optional pitch drop from `+semitones` to 0 over `time` ms on noteOn.

**Unlocks:** kick drums, percussive blips, "bwoop" attack transients. Essential for convincing drum sounds since we can't layer noise + pitched transient otherwise.

**Scope:** ~30 lines. 3 new params (enable, depth, time).

### Priority 3 — Duty-Cycle LFO

**Current:** duty cycle is static per preset.
**New:** LFO that cycles through 12.5 → 25 → 50 → 75 → back over time, or sine-modulates between two duty values.

**Unlocks:** classic NES "breathing" leads (used heavily in Shovel Knight, late-era NES titles). The duty changing over time makes a single pulse voice sound much richer.

**Scope:** ~40 lines. 3 new params (enable, rate, depth).

### Priority 4 — Additional Wavetable Presets

Current 5 (Triangle, Sawtooth, Square, Sine, DW3 Bass) → add:
- **Half-Sine** — fundamental + strong 2nd harmonic, organ-like
- **Pulse 25%** — wavetable version for richer tone than pulse channel
- **Detuned Saw** — two slightly offset sawtooths, thick lead character
- **Bell** — odd harmonics only (1, 3, 5, 7), metallic ring
- **Acid Bass** — resonant-sounding, inspired by 303 overtones

**Scope:** ~5 lines each — just nibble arrays in WaveChannel.cpp.

### Priority 5 — Wave Sequencing

**Current:** wavetable is set once per preset.
**New:** cycle through 2-4 wave presets at a configurable rate (sync'd to note or free-running).

**Unlocks:** morphing pads, FM-like tones, complex evolving textures typical of advanced C64/chip music.

**Scope:** ~40 lines. 2-3 new params.

---

## Part 3 — Recommended Implementation Order

### Phase 1: Preset expansion only (no engine changes) — ~1 session
- Add **Pack A (NES Classic)** — 8 presets
- Add **Pack C (Arcade SFX)** — 10 presets
- Add **Pack D partial** — the 6 presets that don't need ADSR
- Extend `PresetManager::getPresets()` from 6 to ~30 presets
- Each preset is just a `Preset` struct in `PresetManager.cpp` — trivial to add
- GUI preset row already scrolls/wraps, so no layout changes needed

**Effort:** ~2 hours. Instant value.

### Phase 2: Additional wavetable presets — ~1 hour
- Add 5 new waveforms to `WaveChannel::PRESET_*` arrays
- Extend `wavePreset` APVTS choice list
- Add **Pack E (DW3 Expansion)** which uses some new waves

**Effort:** ~1 hour. Enables richer Wave channel sounds.

### Phase 3: ADSR envelope (largest impact) — ~1 session
- Refactor each channel's envelope to 4-stage ADSR
- Add 4 new APVTS params (attack, decay, sustain, release)
- Keep backwards compat — "envInitVol" → sustain level, "envPeriod" → decay rate
- Add **Pack B (SNES RPG)** — all 6 presets now sound authentic
- Complete **Pack D** — real pad preset works now

**Effort:** ~4 hours. Unlocks an entire category of sounds.

### Phase 4: Pitch envelope — ~1 hour
- Add pitch-envelope fields to pulse and wave channels
- Add presets that use it: **Kick Drum**, **Percussive Bloop**, improved **Jump**
- Good for Pack C

**Effort:** ~1 hour. Completes drum emulation.

### Phase 5: Duty LFO + Wave sequencing (optional polish) — ~2 hours
- Duty LFO: add to pulse channels, add APVTS params
- Wave sequencing: add timer to Wave channel for preset cycling
- Add presets that use these: **NES Breathing Lead**, **Morphing Pad**, **FM Bell**

**Effort:** ~2 hours. Nice-to-have, not critical.

---

## Suggested Next Step

**Start with Phase 1** — it's pure-data (no DSP changes), adds ~24 new presets, no risk of regressions, and gives users lots to play with immediately. We'd be moving from 6 presets → 30 presets in a couple hours of work.

If you want to go further after that, **Phase 3 (ADSR)** is the biggest quality-of-life jump for the engine and unlocks everything from Pack B.
