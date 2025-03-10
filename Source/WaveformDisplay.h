/*
  ==============================================================================
  Updated by: Dmitrii Danev
  Update date: May 24, 2024

    WaveformDisplay.h
    Created: 14 Mar 2020 3:50:16pm
    Author:  matthew

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

/**
 * @class WaveformDisplay
 * @brief Component for displaying audio waveforms
 * 
 * Displays an audio waveform and playback position indicator for the loaded audio file.
 * Supports click-to-seek and dragging to scrub through audio.
 */
class WaveformDisplay : public Component, 
                       public ChangeListener {
public:
    /**
     * Callback function for position changes via user interaction
     * This will be called when the user clicks or drags the waveform
     */
    std::function<void(double)> onPositionChange;

    /**
     * Constructor for WaveformDisplay
     * @param formatManagerToUse Reference to the AudioFormatManager for loading audio files
     * @param cacheToUse Reference to the AudioThumbnailCache for caching audio thumbnails
     */
    WaveformDisplay(AudioFormatManager& formatManagerToUse,
                   AudioThumbnailCache& cacheToUse);
    
    /** Destructor */
    ~WaveformDisplay();

    //==========================================================================
    // Component overrides
    //==========================================================================
    
    /** Draws the waveform and playhead */
    void paint(Graphics&) override;
    
    /** Handles component resizing */
    void resized() override;

    //==========================================================================
    // ChangeListener override
    //==========================================================================
    
    /** Responds to changes in the audio thumbnail */
    void changeListenerCallback(ChangeBroadcaster* source) override;

    //==========================================================================
    // MouseListener overrides
    //==========================================================================
    
    /** Handles mouse button press - seeks to clicked position */
    void mouseDown(const MouseEvent& event) override;
    
    /** Handles mouse drag - scrubs audio to dragged position */
    void mouseDrag(const MouseEvent& event) override;
    
    /** Handles mouse button release */
    void mouseUp(const MouseEvent& event) override;

    //==========================================================================
    // Public methods
    //==========================================================================
    
    /** Loads an audio file from a URL */
    void loadURL(URL audioURL);

    /** 
     * Sets the relative position of the playhead
     * @param pos Position between 0.0 and 1.0
     */
    void setPositionRelative(double pos);

private:
    /** Converts x-coordinate to relative position (0-1) */
    double xToPosition(int x) const;
    
    /** Triggers the position change callback if set */
    void notifyPositionChanged(double pos);

    AudioThumbnail audioThumb;
    bool fileLoaded = false;
    double position = 0.0;
    bool isDragging = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};
