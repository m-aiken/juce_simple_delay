/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleDelayAudioProcessorEditor::SimpleDelayAudioProcessorEditor (SimpleDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    addSliderWithLabel (masterGainSlider, masterGainLabel,
                        "Master Gain",
                        -60.f,
                        0.f,
                        1.f,
                        -6.f);
    addSliderWithLabel (delayTimeSlider, delayTimeLabel,
                        "Delay Time",
                        0.f,
                        1000.f,
                        1.f,
                        400.f);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

SimpleDelayAudioProcessorEditor::~SimpleDelayAudioProcessorEditor()
{
}

//==============================================================================
void SimpleDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);
}

void SimpleDelayAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto r = getLocalBounds();
    auto topTwenty = r.removeFromTop (getHeight() * 0.2f);
    auto bottomTwenty = r.removeFromBottom (getHeight() * 0.2f);
    delayTimeSlider.setBounds (r.removeFromLeft(getWidth() * 0.5f));
    masterGainSlider.setBounds (r);
}

void SimpleDelayAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &masterGainSlider)
        audioProcessor.masterGain = masterGainSlider.getValue();
    if (slider == &delayTimeSlider)
        audioProcessor.delayTime = delayTimeSlider.getValue();
}

void SimpleDelayAudioProcessorEditor::addSliderWithLabel(juce::Slider &slider,
                                                         juce::Label &label,
                                                         const juce::String &labelText,
                                                         float rangeMin,
                                                         float rangeMax,
                                                         float interval,
                                                         float defaultValue)
{
    // Set slider params
    slider.setSliderStyle (juce::Slider::SliderStyle::LinearVertical);
    slider.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    slider.setRange (rangeMin, rangeMax, interval);
    slider.setValue (defaultValue);
    slider.addListener (this);
    // Set label params
    addAndMakeVisible (slider);
    label.setText (labelText, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.attachToComponent (&slider, false);
    addAndMakeVisible (label);
}
