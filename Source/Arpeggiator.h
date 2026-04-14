#pragma once

#include <vector>
#include <algorithm>

// Simple arpeggiator that cycles through held notes at a fixed rate.
// Emits internal note-on events that the processor uses instead of the raw MIDI.
class Arpeggiator
{
public:
    enum Pattern { Up = 0, Down = 1, UpDown = 2, Random = 3 };

    void setSampleRate(double sampleRate) { hostSampleRate = sampleRate; }

    void setEnabled(bool on)
    {
        if (enabled == on) return;
        enabled = on;
        reset();
    }

    bool isEnabled() const { return enabled; }

    // Rate in steps per second (e.g. 8 Hz = 8 notes per second)
    void setRateHz(float hz) { rateHz = std::max(0.1f, hz); }

    void setPattern(Pattern p) { pattern = p; }

    // Called on MIDI note-on
    void addNote(int midiNote)
    {
        if (std::find(heldNotes.begin(), heldNotes.end(), midiNote) == heldNotes.end())
            heldNotes.push_back(midiNote);
        std::sort(heldNotes.begin(), heldNotes.end());
    }

    // Called on MIDI note-off
    void removeNote(int midiNote)
    {
        auto it = std::find(heldNotes.begin(), heldNotes.end(), midiNote);
        if (it != heldNotes.end())
            heldNotes.erase(it);
    }

    bool hasNotes() const { return !heldNotes.empty(); }

    void reset()
    {
        stepPhase = 0.0;
        arpIndex = 0;
        ascending = true;
    }

    // Advance one sample. Returns >= 0 if a new step fires, -1 otherwise.
    int tick()
    {
        if (!enabled || heldNotes.empty())
            return -1;

        stepPhase += rateHz / hostSampleRate;
        if (stepPhase < 1.0)
            return -1;

        stepPhase -= 1.0;
        return selectNextNote();
    }

private:
    int selectNextNote()
    {
        const int n = static_cast<int>(heldNotes.size());
        if (n == 0) return -1;

        int noteIdx = 0;
        switch (pattern)
        {
            case Up:
                noteIdx = arpIndex % n;
                arpIndex = (arpIndex + 1) % n;
                break;
            case Down:
                noteIdx = (n - 1 - (arpIndex % n));
                arpIndex = (arpIndex + 1) % n;
                break;
            case UpDown:
                noteIdx = arpIndex;
                if (ascending)
                {
                    arpIndex++;
                    if (arpIndex >= n - 1) { arpIndex = n - 1; ascending = false; }
                }
                else
                {
                    arpIndex--;
                    if (arpIndex <= 0) { arpIndex = 0; ascending = true; }
                }
                if (n == 1) noteIdx = 0;
                break;
            case Random:
                noteIdx = rand() % n;
                break;
        }

        return heldNotes[std::max(0, std::min(n - 1, noteIdx))];
    }

    std::vector<int> heldNotes;
    double hostSampleRate = 44100.0;
    bool enabled = false;
    float rateHz = 8.0f;
    Pattern pattern = Up;
    double stepPhase = 0.0;
    int arpIndex = 0;
    bool ascending = true;
};
