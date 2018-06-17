#include "taste.h" //include the declaration for this class
#include <MIDI.h>

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI); // using TX1 for sending MIDI signal
int Taste::globalVelo = 0;

static void Taste::overrideVelo(int velo) {
  globalVelo = velo;
}

static void Taste::midibegin() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

static void Taste::setPedal(bool press) {
  if(press) {
    Serial.println("PRESS DOWN ");
    MIDI.sendControlChange(64, 127, 1); 
  } else {
    Serial.println("RELEASE ");
    MIDI.sendControlChange(64, 0, 1); 
  }
}

Taste::Taste() {
  timer.stop();
  delayTimer.stop();
}

Taste::~Taste() {
  /*nothing to destruct*/
}

void Taste::playDelayed(int delay, int duration, int velo) {
  this->delay = delay;
  delayDuration = duration;
  delayVelo = velo;
  delayTimer.restart();
}

void Taste::play(int duration, int velo) {
  if (!timer.isRunning()) {
    Serial.print("Note: ");
    Serial.print(midinote);
    Serial.print(" Duration: ");
    Serial.println(duration);

    this->duration = duration;
    timer.restart();
    if(globalVelo == 0) {
      MIDI.sendNoteOn(midinote, velo, 1);
    } else {
      MIDI.sendNoteOn(midinote, globalVelo, 1);
    }
  }
}

void Taste::update() {
  if (timer.hasPassed(duration) && timer.isRunning()) {
    timer.stop();
    MIDI.sendNoteOff(midinote, 0, 1);
  }

  if (delayTimer.hasPassed(delay) && delayTimer.isRunning()){
    delayTimer.stop();
    play(delayDuration, delayVelo);
  }
}

void Taste::stop() {
  if (delayTimer.isRunning()) {
    delayTimer.stop();
  }
}

bool Taste::isPlaying() {
  return timer.isRunning();
}

void Taste::setNote(int midinote) {
  this->midinote = midinote;
}
