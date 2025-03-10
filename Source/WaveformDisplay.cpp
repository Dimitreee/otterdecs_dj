/*
  ==============================================================================
  Updated by: Dmitrii Danev
  Update date: May 24, 2024

    WaveformDisplay.cpp
    Created: 14 Mar 2020 3:50:16pm
    Author:  matthew

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveformDisplay.h"

//==============================================================================
WaveformDisplay::WaveformDisplay(AudioFormatManager& formatManagerToUse,
                               AudioThumbnailCache& cacheToUse) :
    audioThumb(1000, formatManagerToUse, cacheToUse) {
    audioThumb.addChangeListener(this);
    
    // Register as a mouse listener
    addMouseListener(this, true);
    
    // Set mouse cursor to pointing hand to indicate clickable area
    setMouseCursor(MouseCursor::PointingHandCursor);
}

WaveformDisplay::~WaveformDisplay() {
    audioThumb.removeChangeListener(this);
    removeMouseListener(this);
}

void WaveformDisplay::paint(Graphics& g) {
    // Create a nice gradient background for the waveform
    Rectangle<float> bounds = getLocalBounds().toFloat();
    g.setGradientFill(ColourGradient(
        Colour(20, 20, 30),
        0, 0,
        Colour(10, 10, 20),
        0, bounds.getHeight(),
        false));
    g.fillAll();
    
    // Draw a more attractive border
    g.setColour(Colours::grey.withAlpha(0.5f));
    g.drawRoundedRectangle(bounds.reduced(1.0f), 3.0f, 1.0f);
    
    if (fileLoaded) {
        // Draw waveform with better colors
        Rectangle<int> waveformBounds = getLocalBounds().reduced(4);
        g.setColour(Colours::cadetblue.withAlpha(0.7f));  // Nicer color for waveform
        
        audioThumb.drawChannel(g,
            waveformBounds,
            0,
            audioThumb.getTotalLength(),
            0,
            1.0f
        );
        
        // Draw time markers
        g.setColour(Colours::darkgrey.withAlpha(0.3f));
        for (int i = 1; i < 10; ++i) {
            float xPos = waveformBounds.getX() + (waveformBounds.getWidth() * i / 10.0f);
            g.drawVerticalLine(xPos, waveformBounds.getY(), waveformBounds.getBottom());
        }
        
        // Draw playhead position with a more attractive style
        auto playheadX = waveformBounds.getX() + position * waveformBounds.getWidth();
        
        // Draw playhead line
        g.setColour(Colours::lime.withAlpha(0.8f));
        g.drawLine(playheadX, waveformBounds.getY(), playheadX, waveformBounds.getBottom(), 2.0f);
        
        // Draw playhead position marker at the top
        g.setColour(Colours::lime);
        Path triangle;
        float triangleSize = 6.0f;
        triangle.addTriangle(
            playheadX, waveformBounds.getY(), 
            playheadX - triangleSize, waveformBounds.getY() - triangleSize,
            playheadX + triangleSize, waveformBounds.getY() - triangleSize);
        g.fillPath(triangle);
        
        // Add highlight if dragging
        if (isDragging) {
            // Show position as percentage while dragging
            g.setColour(Colours::white);
            g.setFont(14.0f);
            String posText = String(int(position * 100)) + "%";
            g.drawText(posText, getLocalBounds().reduced(8), Justification::topRight, false);
            
            // Add a subtle highlight effect
            g.setColour(Colours::white.withAlpha(0.1f));
            g.fillRect(bounds.getX(), bounds.getY(), 
                      playheadX - bounds.getX(), bounds.getHeight());
        }
    } else {
        // Display message when no file is loaded - with better styling
        g.setColour(Colours::white.withAlpha(0.7f));
        g.setFont(Font(18.0f, Font::italic));
        g.drawText("Drop Audio File Here", getLocalBounds(),
                  Justification::centred, true);
        
        // Add hint for interaction
        g.setFont(Font(14.0f));
        g.setColour(Colours::lightgrey.withAlpha(0.6f));
        g.drawText("Click & drag to seek", getLocalBounds().removeFromBottom(25),
                  Justification::centred, true);
    }
}

void WaveformDisplay::resized() {
    // No child components to resize
}

void WaveformDisplay::loadURL(URL audioURL) {
    audioThumb.clear();
    fileLoaded = audioThumb.setSource(new URLInputSource(audioURL));
    
    if (fileLoaded) {
        DBG("WaveformDisplay: Successfully loaded file");
        position = 0.0;
    } else {
        DBG("WaveformDisplay: Failed to load file: " + audioURL.toString(false));
    }
    
    repaint();
}

void WaveformDisplay::changeListenerCallback(ChangeBroadcaster* source) {
    // Audio thumbnail has changed, so repaint the component
    if (source == &audioThumb) {
        DBG("WaveformDisplay: Thumbnail changed");
        repaint();
    }
}

void WaveformDisplay::setPositionRelative(double pos) {
    // Only repaint if the position has actually changed
    if (pos != position && pos >= 0.0 && pos <= 1.0)
    {
        position = pos;
        repaint();
    }
}

//==============================================================================
// Mouse Interaction Methods
//==============================================================================

// Written by me without any assistance
void WaveformDisplay::mouseDown(const MouseEvent& event) {
    if (fileLoaded) {
        isDragging = true;
        double clickPosition = xToPosition(event.x);
        
        // Update internal position
        setPositionRelative(clickPosition);
        
        // Notify listeners about position change
        notifyPositionChanged(clickPosition);
        
        DBG("WaveformDisplay: Seek to position: " + String(clickPosition));
    }
}

void WaveformDisplay::mouseDrag(const MouseEvent& event) {
    if (fileLoaded && isDragging) {
        double dragPosition = xToPosition(event.x);
        
        // Update internal position
        setPositionRelative(dragPosition);
        
        // Notify listeners about position change
        notifyPositionChanged(dragPosition);
        
        DBG("WaveformDisplay: Drag to position: " + String(dragPosition));
    }
}

void WaveformDisplay::mouseUp(const MouseEvent& event) {
    isDragging = false;
    repaint(); // Refresh to remove any drag-specific visual elements
}

//==============================================================================
// Helper Methods
//==============================================================================

double WaveformDisplay::xToPosition(int x) const {
    // Convert x coordinate to a position between 0 and 1
    // Account for the display borders
    Rectangle<int> usableArea = getLocalBounds().reduced(4);
    double relativeX = static_cast<double>(x - usableArea.getX()) / usableArea.getWidth();
    
    // Ensure bounds are respected
    return jlimit(0.0, 1.0, relativeX);
}

void WaveformDisplay::notifyPositionChanged(double pos) {
    // Call the callback if it's set
    if (onPositionChange)
    {
        onPositionChange(pos);
    }
}
