//
//  SlidersComponent.cpp
//  juicysfplugin - Shared Code
//
//  Created by Alex Birch on 29/06/2019.
//  Copyright © 2019 Birchlabs. All rights reserved.
//

#include "SlidersComponent.h"
#include "FluidSynthModel.h"
#include "MidiConstants.h"

std::function<void()> SlidersComponent::makeSliderListener(Slider& slider, int controller/*, std::function<void()> callback*/) {
    return [this, controller, &slider]{
//        slider.setValue(slider.getValue(), NotificationType::dontSendNotification);
        fluidSynthModel->setControllerValue(controller, slider.getValue());
        // callback();
    };
}

SlidersComponent::~SlidersComponent()
{
}

const int SlidersComponent::getDesiredWidth() {
    const int envelopeSliders{4};
    const int filterSliders{2};
    const int groupXMargin{8};
    const int groupXPadding{8};
    const int sliderXMargin{3};
    const int sliderWidth{30};

    return envelopeSliders * sliderWidth + (envelopeSliders-1) * sliderXMargin + 2 * groupXPadding
    + filterSliders * sliderWidth + (filterSliders-1) * sliderXMargin + 2 * groupXPadding + groupXMargin;
}

void SlidersComponent::resized() {
    const int envelopeSliders{4};
    const int filterSliders{2};
    const int groupXMargin{8};
    const int groupXPadding{8};
    const int groupYPadding{9};
    const int sliderXMargin{3};
    const int labelHeight{25};
    const int sliderWidth{30};
    Rectangle<int> r{getLocalBounds()};
    Rectangle<int> rEnvelope{r.removeFromLeft(envelopeSliders * sliderWidth + (envelopeSliders-1) * sliderXMargin + 2 * groupXPadding)};
    Rectangle<int> rFilter{r.removeFromLeft(filterSliders * sliderWidth + (filterSliders-1) * sliderXMargin + 2 * groupXPadding + groupXMargin).withTrimmedLeft(groupXMargin)};
    envelopeGroup.setBounds(rEnvelope);
    filterGroup.setBounds(rFilter);

    rEnvelope.reduce(groupXPadding, groupYPadding);
    rFilter.reduce(groupXPadding, groupYPadding);
    attackSlider.setBounds(rEnvelope.removeFromLeft(sliderWidth).withTrimmedTop(labelHeight));
    decaySlider.setBounds(rEnvelope.removeFromLeft(sliderWidth + sliderXMargin).withTrimmedTop(labelHeight).withTrimmedLeft(sliderXMargin));
    sustainSlider.setBounds(rEnvelope.removeFromLeft(sliderWidth + sliderXMargin).withTrimmedTop(labelHeight).withTrimmedLeft(sliderXMargin));
    releaseSlider.setBounds(rEnvelope.removeFromLeft(sliderWidth + sliderXMargin).withTrimmedTop(labelHeight).withTrimmedLeft(sliderXMargin));
    filterCutOffSlider.setBounds(rFilter.removeFromLeft(sliderWidth).withTrimmedTop(labelHeight));
    filterResonanceSlider.setBounds(rFilter.removeFromLeft(sliderWidth + sliderXMargin).withTrimmedTop(labelHeight).withTrimmedLeft(sliderXMargin));
}

void SlidersComponent::acceptMidiControlEvent(int controller, int value) {
    switch(static_cast<fluid_midi_control_change>(controller)) {
        case SOUND_CTRL2: // MIDI CC 71 Timbre/Harmonic Intensity (filter resonance)
            filterResonanceSlider.setValue(value, NotificationType::dontSendNotification);
            break;
        case SOUND_CTRL3: // MIDI CC 72 Release time
            releaseSlider.setValue(value, NotificationType::dontSendNotification);
            break;
        case SOUND_CTRL4: // MIDI CC 73 Attack time
            attackSlider.setValue(value, NotificationType::dontSendNotification);
            break;
        case SOUND_CTRL5: // MIDI CC 74 Brightness (cutoff frequency, FILTERFC)
            filterCutOffSlider.setValue(value, NotificationType::dontSendNotification);
            break;
        case SOUND_CTRL6: // MIDI CC 75 Decay Time
            decaySlider.setValue(value, NotificationType::dontSendNotification);
            break;
        case SOUND_CTRL10: // MIDI CC 79 undefined
            sustainSlider.setValue(value, NotificationType::dontSendNotification);
            break;
        default:
            break;
    }
}

// void SlidersComponent::updateAttackSlider(int value) {
//     attackSlider.setValue(value, NotificationType::dontSendNotification);
// }
// void SlidersComponent::updateDecaySlider(int value) {
//     decaySlider.setValue(value, NotificationType::dontSendNotification);
// }
// void SlidersComponent::updateSustainSlider(int value) {
//     sustainSlider.setValue(value, NotificationType::dontSendNotification);
// }
// void SlidersComponent::updateReleaseSlider(int value) {
//     releaseSlider.setValue(value, NotificationType::dontSendNotification);
// }
// void SlidersComponent::updateFilterCutOffSlider(int value) {
//     filterCutOffSlider.setValue(value, NotificationType::dontSendNotification);
// }
// void SlidersComponent::updateFilterResonanceSlider(int value) {
//     filterResonanceSlider.setValue(value, NotificationType::dontSendNotification);
// }

SlidersComponent::SlidersComponent(
    shared_ptr<SharesParams> sharedParams,
    FluidSynthModel* fluidSynthModel) :
sharedParams{sharedParams},
fluidSynthModel{fluidSynthModel},
envelopeGroup{"envelopeGroup", "Envelope"},
filterGroup{"filterGroup", "Filter"}
{
    const Slider::SliderStyle style{Slider::SliderStyle::LinearVertical};
    const double rangeMin(0);
    const double rangeMax(127);
    const double rangeStep(1);

    attackSlider.setSliderStyle(style);
    attackSlider.setRange(rangeMin, rangeMax, rangeStep);
    attackSlider.onValueChange = makeSliderListener(attackSlider, static_cast<int>(SOUND_CTRL4));
    attackSlider.setTextBoxStyle(Slider::TextBoxBelow, true, attackSlider.getTextBoxWidth(), attackSlider.getTextBoxHeight());

    decaySlider.setSliderStyle(style);
    decaySlider.setRange(rangeMin, rangeMax, rangeStep);
    decaySlider.onValueChange = makeSliderListener(decaySlider, static_cast<int>(SOUND_CTRL6));
    decaySlider.setTextBoxStyle(Slider::TextBoxBelow, true, decaySlider.getTextBoxWidth(), decaySlider.getTextBoxHeight());

    sustainSlider.setSliderStyle(style);
    sustainSlider.setRange(rangeMin, rangeMax, rangeStep);
    sustainSlider.onValueChange = makeSliderListener(sustainSlider, static_cast<int>(SOUND_CTRL10));
    sustainSlider.setTextBoxStyle(Slider::TextBoxBelow, true, sustainSlider.getTextBoxWidth(), sustainSlider.getTextBoxHeight());

    releaseSlider.setSliderStyle(style);
    releaseSlider.setRange(rangeMin, rangeMax, rangeStep);
    releaseSlider.onValueChange = makeSliderListener(releaseSlider, static_cast<int>(SOUND_CTRL3));
    releaseSlider.setTextBoxStyle(Slider::TextBoxBelow, true, releaseSlider.getTextBoxWidth(), releaseSlider.getTextBoxHeight());

    filterCutOffSlider.setSliderStyle(style);
    filterCutOffSlider.setRange(rangeMin, rangeMax, rangeStep);
    filterCutOffSlider.onValueChange = makeSliderListener(filterCutOffSlider, static_cast<int>(SOUND_CTRL5));
    filterCutOffSlider.setTextBoxStyle(Slider::TextBoxBelow, true, filterCutOffSlider.getTextBoxWidth(), filterCutOffSlider.getTextBoxHeight());

    filterResonanceSlider.setSliderStyle(style);
    filterResonanceSlider.setRange(rangeMin, rangeMax, rangeStep);
    filterResonanceSlider.onValueChange = makeSliderListener(filterResonanceSlider, static_cast<int>(SOUND_CTRL2));
    filterResonanceSlider.setTextBoxStyle(Slider::TextBoxBelow, true, filterResonanceSlider.getTextBoxWidth(), filterResonanceSlider.getTextBoxHeight());

    addAndMakeVisible(attackSlider);
    addAndMakeVisible(decaySlider);
    addAndMakeVisible(sustainSlider);
    addAndMakeVisible(releaseSlider);
    addAndMakeVisible(filterCutOffSlider);
    addAndMakeVisible(filterResonanceSlider);

    attackLabel.setText("A", NotificationType::dontSendNotification);
    attackLabel.setJustificationType(Justification::centredBottom);
    attackLabel.attachToComponent(&attackSlider, false);

    decayLabel.setText("D", NotificationType::dontSendNotification);
    decayLabel.setJustificationType(Justification::centredBottom);
    decayLabel.attachToComponent(&decaySlider, false);

    sustainLabel.setText("S", NotificationType::dontSendNotification);
    sustainLabel.setJustificationType(Justification::centredBottom);
    sustainLabel.attachToComponent(&sustainSlider, false);
    
    releaseLabel.setText("R", NotificationType::dontSendNotification);
    releaseLabel.setJustificationType(Justification::centredBottom);
    releaseLabel.attachToComponent(&releaseSlider, false);

    filterCutOffLabel.setText("Cut", NotificationType::dontSendNotification);
    filterCutOffLabel.setJustificationType(Justification::centredBottom);
    filterCutOffLabel.attachToComponent(&filterCutOffSlider, false);

    filterResonanceLabel.setText("Res", NotificationType::dontSendNotification);
    filterResonanceLabel.setJustificationType(Justification::centredBottom);
    filterResonanceLabel.attachToComponent(&filterResonanceSlider, false);

    addAndMakeVisible(attackLabel);
    addAndMakeVisible(decayLabel);
    addAndMakeVisible(sustainLabel);
    addAndMakeVisible(releaseLabel);
    addAndMakeVisible(filterCutOffLabel);
    addAndMakeVisible(filterResonanceLabel);

    addAndMakeVisible(envelopeGroup);
    addAndMakeVisible(filterGroup);
}