#include "PluginProcessor.h"
#include "PluginEditor.h"

DattorroPlateReverbProcessor::DattorroPlateReverbProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", Parameters::createParameterLayout())
{
}

DattorroPlateReverbProcessor::~DattorroPlateReverbProcessor()
{
}

const juce::String DattorroPlateReverbProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DattorroPlateReverbProcessor::acceptsMidi() const
{
    return false;
}

bool DattorroPlateReverbProcessor::producesMidi() const
{
    return false;
}

bool DattorroPlateReverbProcessor::isMidiEffect() const
{
    return false;
}

double DattorroPlateReverbProcessor::getTailLengthSeconds() const
{
    return 10.0;
}

int DattorroPlateReverbProcessor::getNumPrograms()
{
    return 1;
}

int DattorroPlateReverbProcessor::getCurrentProgram()
{
    return 0;
}

void DattorroPlateReverbProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String DattorroPlateReverbProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void DattorroPlateReverbProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void DattorroPlateReverbProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void DattorroPlateReverbProcessor::releaseResources()
{
}

bool DattorroPlateReverbProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void DattorroPlateReverbProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    // Phase 1: Pass-through (Input copy to Output is implicit if we do nothing, 
    // but clearing ensures silence if we want to test empty processing, 
    // however spec says "Input copy". In JUCE, input buffer IS output buffer.
    // So doing nothing is effectively a pass-through.)
}

void DattorroPlateReverbProcessor::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(buffer, midiMessages);
}

bool DattorroPlateReverbProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* DattorroPlateReverbProcessor::createEditor()
{
    return new DattorroPlateReverbEditor(*this);
}

void DattorroPlateReverbProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void DattorroPlateReverbProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DattorroPlateReverbProcessor();
}
