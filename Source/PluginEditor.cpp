#include "PluginProcessor.h"
#include "PluginEditor.h"

DattorroPlateReverbEditor::DattorroPlateReverbEditor(DattorroPlateReverbProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(400, 600);
}

DattorroPlateReverbEditor::~DattorroPlateReverbEditor()
{
}

void DattorroPlateReverbEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(16.0f);
    g.drawFittedText("Dattorro Plate Reverb", getLocalBounds(), juce::Justification::centredTop, 1);
}

void DattorroPlateReverbEditor::resized()
{
    // Phase 1: No layout
}
