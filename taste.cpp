#include "taste.h" //include the declaration for this class
#include <MIDI.h>

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI); // using TX1 for sending MIDI signal


static void Taste::midibegin() {
	MIDI.begin(MIDI_CHANNEL_OMNI);	
}

Taste::Taste(){

}
 
Taste::~Taste(){/*nothing to destruct*/}
 
void Taste::play(int time, int velo){
	Serial.print("Note: ");
	Serial.print(midinote);
	Serial.print(" Länge: ");
	Serial.println(time);
	
	duration = time;
	timer.restart();
	MIDI.sendNoteOn(midinote, velo, 1);
}
 
void Taste::update(){

	if(timer.hasPassed(duration) && timer.isRunning()) {
		timer.stop();	
      	MIDI.sendNoteOff(midinote, 0, 1);	
	}

}
 
void Taste::setNote(int midinote){
	this->midinote = midinote;
}
