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
class SimpleDelayAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SimpleDelayAudioProcessorEditor (SimpleDelayAudioProcessor&);
    ~SimpleDelayAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void addControlLabel (juce::Slider &slider, juce::Label &label, const juce::String &labelText);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleDelayAudioProcessor& audioProcessor;

    juce::Slider timeSlider, feedbackSlider, wetSlider, drySlider;
    juce::Label  timeLabel,  feedbackLabel,  wetLabel,  dryLabel;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    Attachment timeSliderAttachment, feedbackSliderAttachment, wetSliderAttachment, drySliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleDelayAudioProcessorEditor)
};
