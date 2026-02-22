#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

class Parameters
{
public:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

        // --- Main Controls ---
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("pre_delay", 1),
            "Pre Delay",
            juce::NormalisableRange<float>(0.0f, 200.0f, 0.1f),
            15.0f,
            juce::AudioParameterFloatAttributes().withLabel("ms")
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("decay", 1),
            "Decay",
            juce::NormalisableRange<float>(0.0f, 0.9999f, 0.0001f),
            0.85f
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("damping", 1),
            "Damping",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.3f
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("bandwidth", 1),
            "Input BW",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.0001f),
            0.9995f
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("size", 1),
            "Size",
            juce::NormalisableRange<float>(0.5f, 2.0f, 0.01f),
            1.0f
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("mix", 1),
            "Mix",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
            0.35f
        ));

        // --- Diffusion ---
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("input_diffusion_1", 1),
            "Input Diff 1",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
            0.750f
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("input_diffusion_2", 1),
            "Input Diff 2",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
            0.625f
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("decay_diffusion_1", 1),
            "Decay Diff 1",
            juce::NormalisableRange<float>(0.0f, 0.999f, 0.001f),
            0.70f
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("decay_diffusion_2", 1),
            "Decay Diff 2",
            juce::NormalisableRange<float>(0.0f, 0.999f, 0.001f),
            0.50f
        ));

        // --- Modulation ---
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("mod_rate", 1),
            "Mod Rate",
            juce::NormalisableRange<float>(0.0f, 3.0f, 0.01f),
            0.5f,
            juce::AudioParameterFloatAttributes().withLabel("Hz")
        ));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("mod_depth", 1),
            "Mod Depth",
            juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f),
            0.7f,
            juce::AudioParameterFloatAttributes().withLabel("ms")
        ));

        // --- Oversampling ---
        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID("oversampling", 1),
            "Oversampling",
            juce::StringArray({ "Off", "2x", "4x" }),
            1  // Default: 2x
        ));

        return { params.begin(), params.end() };
    }

private:
    Parameters() = delete; // Static only
};
