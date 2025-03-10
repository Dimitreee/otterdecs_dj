/*
  ==============================================================================
  Updated by: Dmitrii Danev
  Update date: May 24, 2024

  DJAudioPlayer.cpp
  Created: 13 Mar 2020 4:22:22pm
  Author:  matthew

  ==============================================================================
*/

#include "DJAudioPlayer.h"

DJAudioPlayer::DJAudioPlayer(AudioFormatManager& formatManager) 
    : formatManager(formatManager) {
}

DJAudioPlayer::~DJAudioPlayer() {
}

void DJAudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void DJAudioPlayer::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) {
    resampleSource.getNextAudioBlock(bufferToFill);
}

void DJAudioPlayer::releaseResources() {
    transportSource.releaseResources();
    resampleSource.releaseResources();
}

void DJAudioPlayer::loadURL(URL audioURL) {
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));
    if (reader != nullptr) {
        std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true)); 
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);             
        readerSource.reset(newSource.release());          
    }
    else {
        DBG("DJAudioPlayer::loadURL failed to load audio file: " + audioURL.toString(false));
    }
}

void DJAudioPlayer::setGain(double gain) {
    if (gain < 0.0 || gain > 1.0) {
        DBG("DJAudioPlayer::setGain gain should be between 0 and 1, got: " + String(gain));
    }
    else {
        transportSource.setGain(gain);
    }
}

void DJAudioPlayer::setSpeed(double ratio) {
    if (ratio < 0.0 || ratio > 100.0) {
        DBG("DJAudioPlayer::setSpeed ratio should be between 0 and 100, got: " + String(ratio));
    }
    else {
        resampleSource.setResamplingRatio(ratio);
    }
}

void DJAudioPlayer::setPosition(double posInSecs) {
    transportSource.setPosition(posInSecs);
}

void DJAudioPlayer::setPositionRelative(double pos) {
    if (pos < 0.0 || pos > 1.0) {
        DBG("DJAudioPlayer::setPositionRelative pos should be between 0 and 1, got: " + String(pos));
    }
    else {
        double posInSecs = transportSource.getLengthInSeconds() * pos;
        setPosition(posInSecs);
    }
}

void DJAudioPlayer::start() {
    transportSource.start();
}

void DJAudioPlayer::stop() {
    transportSource.stop();
}

double DJAudioPlayer::getPositionRelative() {
    if (transportSource.getLengthInSeconds() > 0.0) {
        return transportSource.getCurrentPosition() / transportSource.getLengthInSeconds();
    }
    return 0.0;
}