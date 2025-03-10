/*
I’ve updated most of the files by renaming, adding new features, refactoring the codebase, and using the project we developed as a base. While there are many differences now, the core functionality remains similar to what it was.

*/
/*
  ==============================================================================
  Updated by: Dmitrii Danev
  Update date: May 24, 2024

    DeckGUI.cpp
    Created: 13 Mar 2020 6:44:48pm
    Author:  matthew

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "DeckGUI.h"

//==============================================================================
DeckGUI::DeckGUI(DJAudioPlayer* player, 
                AudioFormatManager& formatManagerToUse,
                AudioThumbnailCache& cacheToUse) 
    : player(player), 
      waveformDisplay(formatManagerToUse, cacheToUse) {
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loadButton);
    
    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(posSlider);

    addAndMakeVisible(waveformDisplay);

    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);

    volSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);

    playButton.setColour(TextButton::buttonColourId, Colour(0, 180, 0));
    playButton.setColour(TextButton::textColourOffId, Colours::white);
    playButton.setColour(TextButton::buttonOnColourId, Colour(0, 220, 0));
    playButton.setTooltip("Start playback");
    
    stopButton.setColour(TextButton::buttonColourId, Colour(180, 0, 0));
    stopButton.setColour(TextButton::textColourOffId, Colours::white);
    stopButton.setColour(TextButton::buttonOnColourId, Colour(220, 0, 0));
    stopButton.setTooltip("Stop playback");
    
    loadButton.setColour(TextButton::buttonColourId, Colour(0, 90, 160));
    loadButton.setColour(TextButton::textColourOffId, Colours::white);
    loadButton.setColour(TextButton::buttonOnColourId, Colour(0, 130, 210));
    loadButton.setTooltip("Load a new audio file");

    volSlider.setRange(0.0, 1.0);
    volSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 15);
    volSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    volSlider.setColour(Slider::thumbColourId, Colours::orange);
    volSlider.setColour(Slider::rotarySliderFillColourId, Colours::orange);
    volSlider.setColour(Slider::textBoxTextColourId, Colours::white);
    volSlider.setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
    volSlider.setTooltip("Adjust volume");
    
    speedSlider.setRange(0.1, 5.0, 0.01);
    speedSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 15);
    speedSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    speedSlider.setValue(1.0);
    speedSlider.setColour(Slider::thumbColourId, Colours::lightblue);
    speedSlider.setColour(Slider::rotarySliderFillColourId, Colours::lightblue);
    speedSlider.setColour(Slider::textBoxTextColourId, Colours::white);
    speedSlider.setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
    speedSlider.setTooltip("Adjust playback speed");
    
    posSlider.setRange(0.0, 1.0);
    posSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    posSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    posSlider.setColour(Slider::thumbColourId, Colours::lime);
    posSlider.setColour(Slider::trackColourId, Colours::darkgrey);
    posSlider.setTooltip("Seek through audio");
    
    fileChooser = std::make_unique<FileChooser>("Select an audio file...");

    waveformDisplay.onPositionChange = [this, player](double pos) {
        player->setPositionRelative(pos);
        posSlider.setValue(pos, dontSendNotification);
    };

    startTimer(100);
}

DeckGUI::~DeckGUI() {
    stopTimer();
}

void DeckGUI::paint(Graphics& g) {
    auto bounds = getLocalBounds();
    g.setGradientFill(ColourGradient(
        Colour(40, 40, 70), 0.0f, 0.0f,
        Colour(20, 20, 40), 0.0f, static_cast<float>(bounds.getHeight()),
        false));
    g.fillAll();
    
    g.setColour(Colours::white.withAlpha(0.3f));
    g.drawRect(getLocalBounds(), 1);
    
    g.setFont(16.0f);
    g.setColour(Colours::white);
    g.drawText("DECK", getLocalBounds().removeFromTop(25), Justification::centred, false);
}

void DeckGUI::resized() {
    auto area = getLocalBounds().reduced(10);
    
    area.removeFromTop(20);
    
    auto waveformHeight = area.getHeight() * 0.35;
    waveformDisplay.setBounds(area.removeFromTop(waveformHeight));
    
    auto posHeight = 20;
    posSlider.setBounds(area.removeFromTop(posHeight).reduced(5, 0));
    
    area.removeFromTop(10);
    
    auto rotaryHeight = area.getHeight() * 0.40;
    auto controlsArea = area.removeFromTop(rotaryHeight);
    auto leftColumn = controlsArea.removeFromLeft(controlsArea.getWidth() / 2);
    auto rightColumn = controlsArea;
    
    volSlider.setBounds(leftColumn.reduced(15));
    speedSlider.setBounds(rightColumn.reduced(15));
    
    area.removeFromTop(10);
    
    auto buttonHeight = jmin(40, area.getHeight() / 2);
    auto buttonArea = area.removeFromTop(buttonHeight);
    
    int buttonWidth = (buttonArea.getWidth() - 30) / 3;
    playButton.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));
    buttonArea.removeFromLeft(10);
    stopButton.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));
    buttonArea.removeFromLeft(10);
    loadButton.setBounds(buttonArea.reduced(5));
}

void DeckGUI::buttonClicked(Button* button) {
    if (button == &playButton) {
        DBG("Play button clicked");
        player->start();
    }
    else if (button == &stopButton) {
        DBG("Stop button clicked");
        player->stop();
    }
    else if (button == &loadButton) {
        auto fileChooserFlags = FileBrowserComponent::canSelectFiles;
        
        fileChooser->launchAsync(fileChooserFlags, [this](const FileChooser& chooser) {
            File chosenFile = chooser.getResult();
            if (chosenFile.existsAsFile()) {
                URL fileURL = URL(chosenFile);
                loadFileFromURL(fileURL);
            }
        });
    }
}

void DeckGUI::sliderValueChanged(Slider* slider) {
    if (slider == &volSlider) {
        player->setGain(slider->getValue());
    }
    else if (slider == &speedSlider) {
        player->setSpeed(slider->getValue());
    }
    else if (slider == &posSlider) {
        player->setPositionRelative(slider->getValue());
        waveformDisplay.setPositionRelative(slider->getValue());
    }
}

bool DeckGUI::isInterestedInFileDrag(const StringArray& files) {
    for (const auto& file : files) {
        if (File(file).hasFileExtension("wav;mp3;aif;aiff"))
            return true;
    }
    return false;
}

void DeckGUI::filesDropped(const StringArray& files, int x, int y) {
    DBG("File(s) dropped on DeckGUI");
    
    if (files.size() > 0) {
        URL fileURL = URL(File(files[0]));
        loadFileFromURL(fileURL);
    }
}

void DeckGUI::timerCallback() {
    if (!waveformDisplay.isMouseButtonDown()) {
        waveformDisplay.setPositionRelative(player->getPositionRelative());
        
        if (!posSlider.isMouseButtonDown()) {
            posSlider.setValue(player->getPositionRelative(), dontSendNotification);
        }
    }
}

void DeckGUI::loadFileFromURL(const URL& fileURL) {
    DBG("Loading file: " + fileURL.toString(false));
    player->loadURL(fileURL);
    waveformDisplay.loadURL(fileURL);
}


    

