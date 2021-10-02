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
    auto delayBufferSize = sampleRate * 2;
    delayBuffer.setSize (getTotalNumInputChannels(), static_cast<int> (delayBufferSize));

    sRate = sampleRate;
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

    auto mainBufferLength  = buffer.getNumSamples();
    auto delayBufferLength = delayBuffer.getNumSamples();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData     = buffer.getWritePointer (channel);

        auto* mainBufferData  = buffer.getReadPointer(channel);
        auto* delayBufferData = delayBuffer.getReadPointer(channel);

        writeToDelayBuffer (channel, mainBufferLength, delayBufferLength, mainBufferData);
        readFromDelayBuffer (buffer, channel, mainBufferLength, delayBufferLength, delayBufferData,delayTime);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] *= juce::Decibels::decibelsToGain(masterGain);
        }
    }

    delayWritePosition += mainBufferLength;
    delayWritePosition %= delayBufferLength;
}

void SimpleDelayAudioProcessor::writeToDelayBuffer (int channel, int mainBufferLength, int delayBufferLength,
                                                 const float* mainBufferData)
{
    if (delayBufferLength > delayWritePosition + mainBufferLength)
    {
        delayBuffer.copyFrom (channel, delayWritePosition, mainBufferData, mainBufferLength);
    }
    else
    {
        auto numCopyAtEnd   = delayBufferLength - delayWritePosition;
        auto numCopyAtStart = mainBufferLength - numCopyAtEnd;
        delayBuffer.copyFrom (channel, delayWritePosition, mainBufferData, numCopyAtEnd);
        delayBuffer.copyFrom (channel, 0, mainBufferData, numCopyAtStart);
    }
}

void SimpleDelayAudioProcessor::readFromDelayBuffer (juce::AudioBuffer<float>& buffer, int channel, int
mainBufferLength, int delayBufferLength, const float*
delayBufferChannelData, float dTime)
{
    int delayTime     = (sRate * dTime / 1000);
    auto readPosition = static_cast<int> (delayBufferLength + delayWritePosition - delayTime)
            % delayBufferLength;

    if (delayBufferLength > mainBufferLength + readPosition)
    {
        buffer.addFrom (channel, 0, delayBufferChannelData + readPosition, mainBufferLength);
    }
    else
    {
        auto numCopyAtEnd   = delayBufferLength - readPosition;
        auto numCopyAtStart = mainBufferLength - numCopyAtEnd;
        buffer.addFrom (channel, 0, delayBufferChannelData + readPosition, numCopyAtEnd);
        buffer.addFrom (channel, numCopyAtEnd, delayBufferChannelData, numCopyAtStart);
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
