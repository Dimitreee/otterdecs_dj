/*
  ==============================================================================
  Updated by: Dmitrii Danev
  Update date: May 24, 2024

    DeckGUI.h
    Created: 13 Mar 2020 6:44:48pm
    Author:  matthew

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"

/**
 * @class DeckGUI
 * @brief GUI component for controlling audio playback
 * 
 * Provides user interface for controlling audio playback, including
 * transport controls, volume/speed adjustment, and waveform display.
 */
class DeckGUI : public Component,
                public Button::Listener,
                public Slider::Listener,
                public FileDragAndDropTarget,
                public Timer {
public:
    /**
     * Constructor for DeckGUI
     * @param player Pointer to the DJAudioPlayer that this GUI will control
     * @param formatManagerToUse Reference to the AudioFormatManager for audio file loading
     * @param cacheToUse Reference to the AudioThumbnailCache for waveform display
     */
    DeckGUI(DJAudioPlayer* player,
           AudioFormatManager& formatManagerToUse,
           AudioThumbnailCache& cacheToUse);
    
    /** Destructor */
    ~DeckGUI() override;

    //==========================================================================
    // Component overrides
    //==========================================================================
    
    /** Draws the component */
    void paint(Graphics&) override;
    
    /** Handles component resizing and child component layout */
    void resized() override;

    //==========================================================================
    // Button::Listener override
    //==========================================================================
    
    /** Handles button click events */
    void buttonClicked(Button* button) override;

    //==========================================================================
    // Slider::Listener override
    //==========================================================================
    
    /** Handles slider value changes */
    void sliderValueChanged(Slider* slider) override;

    //==========================================================================
    // FileDragAndDropTarget overrides
    //==========================================================================
    
    /** Determines if component accepts file drag */
    bool isInterestedInFileDrag(const StringArray& files) override;
    
    /** Handles dropped files */
    void filesDropped(const StringArray& files, int x, int y) override;

    //==========================================================================
    // Timer override
    //==========================================================================
    
    /** Updates playhead position periodically */
    void timerCallback() override;

private:
    /**
     * Helper method to load audio file from URL
     * @param fileURL URL of the audio file to load
     */
    void loadFileFromURL(const URL& fileURL);
    
    //==========================================================================
    // UI Components
    //==========================================================================
    
    // Buttons
    TextButton playButton{"PLAY"};
    TextButton stopButton{"STOP"};
    TextButton loadButton{"LOAD"};
    
    // Sliders
    Slider volSlider;
    Slider speedSlider;
    Slider posSlider;
    
    // File chooser for loading files
    std::unique_ptr<FileChooser> fileChooser;
    
    // Waveform display
    WaveformDisplay waveformDisplay;
    
    // Reference to the audio player
    DJAudioPlayer* player;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckGUI)
};
