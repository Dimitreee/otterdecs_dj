/*
  ==============================================================================
  Updated by: Dmitrii Danev
  Update date: May 24, 2024

    DJAudioPlayer.h
    Created: 13 Mar 2020 4:22:22pm
    Author:  matthew

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

/**
 * @class DJAudioPlayer
 * @brief Audio player class that handles playback of audio files with transport controls
 *
 * Provides functionality for loading, playing, and manipulating audio files including
 * gain control, speed adjustment, and position control.
 */
class DJAudioPlayer : public AudioSource {
public:
    /**
     * Constructor for DJAudioPlayer
     * @param formatManager Reference to the AudioFormatManager to use for loading audio files
     */
    DJAudioPlayer(AudioFormatManager& formatManager);
    
    /** Destructor */
    ~DJAudioPlayer();

    //==========================================================================
    // AudioSource overrides
    //==========================================================================
    
    /** Prepares the player for playback */
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    
    /** Gets the next block of audio data */
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    
    /** Releases resources used by the player */
    void releaseResources() override;

    //==========================================================================
    // Playback control methods
    //==========================================================================
    
    /** Loads an audio file from a URL */
    void loadURL(URL audioURL);
    
    /** Sets the gain (volume) level (0.0 to 1.0) */
    void setGain(double gain);
    
    /** Sets the playback speed ratio (0.0 to 100.0) */
    void setSpeed(double ratio);
    
    /** Sets the playback position in seconds */
    void setPosition(double posInSecs);
    
    /** Sets the playback position as a proportion of the total length (0.0 to 1.0) */
    void setPositionRelative(double pos);
    
    /** Starts playback */
    void start();
    
    /** Stops playback */
    void stop();

    /** Gets the current relative position of the playhead (0.0 to 1.0) */
    double getPositionRelative();

private:
    AudioFormatManager& formatManager;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource; 
    ResamplingAudioSource resampleSource{&transportSource, false, 2};
};




