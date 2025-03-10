/*
  ==============================================================================
  Updated by: Dmitrii Danev
  Update date: May 24, 2024

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "DeckGUI.h"

/**
 * @class MainComponent
 * @brief Main application component for the OtoDecks DJ application
 * 
 * Provides the main application layout with two DJ decks for mixing audio.
 * Handles audio processing and routing between decks.
 */
class MainComponent : public AudioAppComponent {
public:
    //==========================================================================
    // Construction and destruction
    //==========================================================================
    
    /** Constructor */
    MainComponent();
    
    /** Destructor */
    ~MainComponent() override;

    //==========================================================================
    // AudioAppComponent overrides for audio processing
    //==========================================================================
    
    /** Prepares audio processing */
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    
    /** Processes audio blocks */
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    
    /** Releases audio resources */
    void releaseResources() override;

    //==========================================================================
    // Component overrides
    //==========================================================================
    
    /** Draws the component */
    void paint(Graphics& g) override;
    
    /** Handles component layout */
    void resized() override;

private:
    //==========================================================================
    // UI Components
    //==========================================================================
    Label titleLabel;
    
    //==========================================================================
    // Audio format management
    //==========================================================================
    AudioFormatManager formatManager;
    AudioThumbnailCache thumbCache{100};

    //==========================================================================
    // Audio players and decks
    //==========================================================================
    DJAudioPlayer player1{formatManager};
    DeckGUI deckGUI1{&player1, formatManager, thumbCache};

    DJAudioPlayer player2{formatManager};
    DeckGUI deckGUI2{&player2, formatManager, thumbCache};

    //==========================================================================
    // Audio mixing
    //==========================================================================
    MixerAudioSource mixerSource;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
