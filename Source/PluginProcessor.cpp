/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleDelayAudioProcessor::SimpleDelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

SimpleDelayAudioProcessor::~SimpleDelayAudioProcessor()
{
}

//==============================================================================
const juce::String SimpleDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleDelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SimpleDelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleDelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SimpleDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    const int numInputChannels = getTotalNumInputChannels();
    const int delayBufferSize = 2 * (sampleRate + samplesPerBlock);
    sRate = sampleRate;

    delayBuffer.setSize(numInputChannels, delayBufferSize);
}

void SimpleDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SimpleDelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    const int bufferLength      = buffer.getNumSamples();
    const int delayBufferLength = delayBuffer.getNumSamples();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        const float* bufferData      = buffer.getReadPointer(channel);
        const float* delayBufferData = delayBuffer.getReadPointer(channel);

        fillDelayBuffer(channel, bufferLength, delayBufferLength, bufferData, delayBufferData);
        getFromDelayBuffer(buffer, channel, bufferLength, delayBufferLength, bufferData, delayBufferData,
                           500);
//        auto* channelData = buffer.getWritePointer (channel);
//
//        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
//        {
//            channelData[sample] *= juce::Decibels::decibelsToGain(masterGain);
//        }
    }

    writePosition += bufferLength;
    writePosition %= delayBufferLength;
}

void SimpleDelayAudioProcessor::fillDelayBuffer(int channel, const int bufferLength, const int
delayBufferLength, const float* bufferData, const float* delayBufferData)
{
    if (delayBufferLength > bufferLength + writePosition)
    {
        delayBuffer.copyFromWithRamp(channel, writePosition, bufferData, bufferLength, 0.8f, 0.8f);
    }
    else
    {
        const int bufferRemaining = delayBufferLength - writePosition;
        delayBuffer.copyFromWithRamp(channel, writePosition, bufferData, bufferRemaining, 0.8f, 0.8f);
        delayBuffer.copyFromWithRamp(channel, 0, bufferData, bufferLength - bufferRemaining, 0.8f, 0.8f);
    }
}

void SimpleDelayAudioProcessor::getFromDelayBuffer(juce::AudioBuffer<float>& buffer, int channel, const int bufferLength, const int
delayBufferLength, const float* bufferData, const float* delayBufferData, int dTime)
{
    int delayTime = (sRate * dTime / 1000);
    const int readPosition = static_cast<int>(delayBufferLength + writePosition - delayTime)
            % delayBufferLength;

    if (delayBufferLength > bufferLength + readPosition)
    {
        buffer.addFrom(channel, 0, delayBufferData + readPosition, bufferLength);
    }
    else
    {
        const int bufferRemaining = delayBufferLength - readPosition;
        buffer.addFrom(channel, 0, delayBufferData + readPosition, bufferRemaining);
        buffer.addFrom(channel, bufferRemaining, delayBufferData, bufferLength - bufferRemaining);
    }
}

//==============================================================================
bool SimpleDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleDelayAudioProcessor::createEditor()
{
    return new SimpleDelayAudioProcessorEditor (*this);
}

//==============================================================================
void SimpleDelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SimpleDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleDelayAudioProcessor();
}
