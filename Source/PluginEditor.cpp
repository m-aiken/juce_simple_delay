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

    delayTimeSlider.setSliderStyle (juce::Slider::SliderStyle::Rotary);
    delayTimeSlider.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    delayTimeSlider.setRange (0.f, 1000.f, 1.f);
    delayTimeSlider.setValue (400.f);
    delayTimeSlider.addListener (this);
    addAndMakeVisible (delayTimeSlider);
    addControlLabel(delayTimeSlider, delayTimeLabel, "Delay Time", false);

    delayFeedbackSlider.setSliderStyle (juce::Slider::SliderStyle::Rotary);
    delayFeedbackSlider.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    delayFeedbackSlider.setRange (0.f, 1.f, 0.01f);
    delayFeedbackSlider.setValue (0.5f);
    delayFeedbackSlider.addListener (this);
    addAndMakeVisible (delayFeedbackSlider);
    addControlLabel(delayFeedbackSlider, delayFeedbackLabel, "Feedback", false);

    delayWetSlider.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    delayWetSlider.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxRight, false, 50, 20);
    delayWetSlider.setRange (-60.f, 0.f, 1.f);
    delayWetSlider.setValue (0.f);
    delayWetSlider.addListener (this);
    addAndMakeVisible (delayWetSlider);
    addControlLabel (delayWetSlider, delayWetLabel, "Wet", true);

    delayDrySlider.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    delayDrySlider.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxRight, false, 50, 20);
    delayDrySlider.setRange (-60.f, 0.f, 1.f);
    delayDrySlider.setValue (0.f);
    delayDrySlider.addListener (this);
    addAndMakeVisible (delayDrySlider);
    addControlLabel (delayDrySlider, delayDryLabel, "Dry", true);

    masterGainSlider.setSliderStyle (juce::Slider::SliderStyle::LinearVertical);
    masterGainSlider.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    masterGainSlider.setRange (-60.f, 0.f, 1.f);
    masterGainSlider.setValue (-6.f);
    masterGainSlider.addListener (this);
    addAndMakeVisible (masterGainSlider);
    addControlLabel (masterGainSlider, masterGainLabel, "Master Gain", false);

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

    auto container = getLocalBounds();
    auto rotaryContainer = container.removeFromLeft(getWidth() * 0.8f);
    auto wetDryContainer = rotaryContainer.removeFromBottom(getHeight() * 0.4f);
    auto topTwenty = container.removeFromTop (getHeight() * 0.2f);
    auto bottomTwenty = container.removeFromBottom (getHeight() * 0.2f);

    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    juce::Grid rotaryControls;
    rotaryControls.templateRows    = { Track (Fr (1)) };
    rotaryControls.templateColumns = { Track (Fr (1)), Track (Fr (1)) };
    auto dts = juce::GridItem (delayTimeSlider);
    dts.margin = 20;
    auto dfs = juce::GridItem (delayFeedbackSlider);
    dfs.margin = 20;
    rotaryControls.items = { dts, dfs };
    rotaryControls.performLayout(rotaryContainer);

    juce::Grid wetDryControls;
    wetDryControls.templateRows = { Track (Fr (1)), Track (Fr (1)) };
    wetDryControls.templateColumns = { Track (Fr (1)) };
    auto dws = juce::GridItem (delayWetSlider);
    dws.margin = 30;
    auto dds = juce::GridItem (delayDrySlider);
    dds.margin = 30;
    wetDryControls.items = { dws, dds };
    wetDryControls.performLayout(wetDryContainer);

    masterGainSlider.setBounds(container);
}

void SimpleDelayAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &delayTimeSlider)
        audioProcessor.delayTime = delayTimeSlider.getValue();

    if (slider == &delayFeedbackSlider)
        audioProcessor.delayFeedback = delayFeedbackSlider.getValue();

    if (slider == &delayWetSlider)
        audioProcessor.delayWetLevel = delayWetSlider.getValue();

    if (slider == &delayDrySlider)
        audioProcessor.delayDryLevel = delayDrySlider.getValue();

    if (slider == &masterGainSlider)
        audioProcessor.masterGain = masterGainSlider.getValue();
}

void SimpleDelayAudioProcessorEditor::addControlLabel(juce::Slider &slider,
                                                      juce::Label &label,
                                                      const juce::String &labelText,
                                                      bool left)
{
    label.setText (labelText, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.attachToComponent (&slider, left);
    addAndMakeVisible (label);
}
