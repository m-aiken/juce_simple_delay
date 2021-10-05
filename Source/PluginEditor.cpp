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
    addAndMakeVisible (timeSlider);
    timeSlider.setSliderStyle (juce::Slider::SliderStyle::Rotary);
    timeSlider.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    timeSlider.setRange (0.f, 1000.f, 1.f);
    timeSlider.setValue (400.f);
    timeSlider.addListener (this);
    addControlLabel (timeSlider, timeLabel, "Delay Time");

    addAndMakeVisible (feedbackSlider);
    feedbackSlider.setSliderStyle (juce::Slider::SliderStyle::Rotary);
    feedbackSlider.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    feedbackSlider.setRange (0.f, 1.f, 0.01f);
    feedbackSlider.setValue (0.5f);
    feedbackSlider.addListener (this);
    addControlLabel (feedbackSlider, feedbackLabel, "Feedback");

    addAndMakeVisible (wetSlider);
    wetSlider.setSliderStyle (juce::Slider::SliderStyle::LinearVertical);
    wetSlider.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    wetSlider.setRange (-60.f, 0.f, 1.f);
    wetSlider.setValue (-6.f);
    wetSlider.addListener (this);
    addControlLabel (wetSlider, wetLabel, "Wet");

    addAndMakeVisible (drySlider);
    drySlider.setSliderStyle (juce::Slider::SliderStyle::LinearVertical);
    drySlider.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    drySlider.setRange (-60.f, 0.f, 1.f);
    drySlider.setValue (-6.f);
    drySlider.addListener (this);
    addControlLabel (drySlider, dryLabel, "Dry");

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 500);
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

    auto container    = getLocalBounds();
    auto topTwenty    = container.removeFromTop (getHeight() * 0.2f);
    auto bottomTwenty = container.removeFromBottom (getHeight() * 0.2f);

    using Track = juce::Grid::TrackInfo;
    using Fr    = juce::Grid::Fr;

    juce::Grid controls;

    controls.templateRows    = { Track (Fr (1)) };
    controls.templateColumns = { Track (Fr (3)), Track (Fr (3)), Track (Fr (1)), Track (Fr (1)) };

    auto timeS     = juce::GridItem (timeSlider);
    auto feedbackS = juce::GridItem (feedbackSlider);
    auto wetS      = juce::GridItem (wetSlider);
    auto dryS      = juce::GridItem (drySlider);

    timeS.margin     = 20;
    feedbackS.margin = 20;

    controls.items = { timeS, feedbackS, wetS, dryS };

    controls.performLayout(container);
}

void SimpleDelayAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &timeSlider)
        audioProcessor.delayTime = timeSlider.getValue();

    if (slider == &feedbackSlider)
        audioProcessor.delayFeedback = feedbackSlider.getValue();

    if (slider == &wetSlider)
        audioProcessor.delayWetLevel = wetSlider.getValue();

    if (slider == &drySlider)
        audioProcessor.delayDryLevel = drySlider.getValue();
}

void SimpleDelayAudioProcessorEditor::addControlLabel(juce::Slider &slider,
                                                      juce::Label &label,
                                                      const juce::String &labelText)
{
    addAndMakeVisible (label);
    label.setText (labelText, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.attachToComponent (&slider, false);
}
