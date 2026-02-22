#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

class DattorroPlateReverbProcessor;

class DattorroPlateReverbEditor : public juce::AudioProcessorEditor
{
public:
    DattorroPlateReverbEditor(DattorroPlateReverbProcessor&);
    ~DattorroPlateReverbEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    DattorroPlateReverbProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DattorroPlateReverbEditor)
};
