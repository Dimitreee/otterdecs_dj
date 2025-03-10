/*
  ==============================================================================
  Updated by: Dmitrii Danev
  Update date: May 24, 2024

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() {
    setSize(1000, 600);

    if (RuntimePermissions::isRequired(RuntimePermissions::recordAudio)
        && !RuntimePermissions::isGranted(RuntimePermissions::recordAudio)) {
        RuntimePermissions::request(RuntimePermissions::recordAudio,
                                   [&](bool granted) { 
                                       if (granted) 
                                           setAudioChannels(2, 2); 
                                   });
    }  
    else {
        setAudioChannels(0, 2);
    }  

    addAndMakeVisible(deckGUI1); 
    addAndMakeVisible(deckGUI2);
    
    addAndMakeVisible(titleLabel);
    titleLabel.setText("OtoDecks DJ Studio", dontSendNotification);
    titleLabel.setFont(Font(28.0f, Font::bold));
    titleLabel.setJustificationType(Justification::centred);
    titleLabel.setColour(Label::textColourId, Colours::white);
    titleLabel.setColour(Label::backgroundColourId, Colours::transparentBlack);

    formatManager.registerBasicFormats();
}

MainComponent::~MainComponent() {
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
    
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    mixerSource.addInputSource(&player1, false);
    mixerSource.addInputSource(&player2, false);
}

void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) {
    mixerSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources() {
    player1.releaseResources();
    player2.releaseResources();
    mixerSource.releaseResources();
}

//==============================================================================
void MainComponent::paint(Graphics& g) {
    auto bounds = getLocalBounds();
    g.setGradientFill(ColourGradient(
        Colour(30, 30, 50),
        0.0f, 0.0f,
        Colour(15, 15, 30),
        0.0f, static_cast<float>(bounds.getHeight()),
        false));
    g.fillAll();
    
    g.setColour(Colours::white.withAlpha(0.03f));
    for (int y = 0; y < bounds.getHeight(); y += 20) {
        g.drawHorizontalLine(y, 0, bounds.getWidth());
    }
    
    g.setColour(Colours::white.withAlpha(0.2f));
    g.drawRect(bounds, 1);
    
    Rectangle<int> titleArea = bounds.removeFromTop(60);
    g.setColour(Colours::blue.withAlpha(0.1f));
    g.fillRect(titleArea.reduced(bounds.getWidth() / 4, 15));
}

void MainComponent::resized() {
    auto area = getLocalBounds().reduced(15);
    
    titleLabel.setBounds(area.removeFromTop(40));
    
    area.removeFromTop(20);
    
    auto leftDeckArea = area.removeFromLeft(area.getWidth() / 2 - 10);
    auto rightDeckArea = area.removeFromRight(area.getWidth() - 10);
    
    deckGUI1.setBounds(leftDeckArea);
    deckGUI2.setBounds(rightDeckArea);
}

