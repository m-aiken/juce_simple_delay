/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SimpleDelayAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    SimpleDelayAudioProcessorEditor (SimpleDelayAudioProcessor&);
    ~SimpleDelayAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void addSliderWithLabel (juce::Slider &slider, juce::Label &label, const juce::String &labelText,
                            float rangeMin, float rangeMax, float interval, float defaultValue);

    void sliderValueChanged (juce::Slider* slider) override;
private:
    juce::Slider masterGainSlider;
    juce::Label  masterGainLabel;
    juce::Slider delayTimeSlider;
    juce::Label  delayTimeLabel;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleDelayAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleDelayAudioProcessorEditor)
};
