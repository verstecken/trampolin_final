#include "taste.h"
#include <MIDI.h>  

//MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI); // using TX1 for sending MIDI signal
  
Taste tasten[60];

void setup() {
  Taste::midibegin();
  Serial.begin(115200);

  for(int i = 0; i < 60; i++) {
    tasten[i].setNote(i+31);
  }

  tasten[25].play();

}

void loop() {

  


  for(int i = 0; i < 60; i++) {
    tasten[i].update();
  }  

}
