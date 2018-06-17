#include "taste.h" //include the declaration for this class
#include <MIDI.h>

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI); // using TX1 for sending MIDI signal


static void Taste::midibegin() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

Taste::Taste() {
  timer.stop();
}

Taste::~Taste() {
  /*nothing to destruct*/
}

void Taste::play(int duration, int velo) {

  if(!timer.isRunning()) { 
    Serial.print("Note: ");
    Serial.print(midinote);
    Serial.print(" Duration: ");
    Serial.println(duration);
  
    this->duration = duration;
    timer.restart();
    MIDI.sendNoteOn(midinote, velo, 1);
  }
}

void Taste::update() {

  if (timer.hasPassed(duration) && timer.isRunning()) {
    timer.stop();
    MIDI.sendNoteOff(midinote, 0, 1);
  }

}

bool Taste::isPlaying() {
  return timer.isRunning();
}

void Taste::setNote(int midinote) {
  this->midinote = midinote;
}
