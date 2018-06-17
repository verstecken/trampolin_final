#include "taste.h"
#include "sensor.h"

Sensor sensor;
Taste tasten[60];

void setup() {
  sensor.init();
  Taste::midibegin();
  Serial.begin(115200);

  for (int i = 0; i < 60; i++) {
    tasten[i].setNote(i + 31);
  }

  tasten[25].play();

}

void loop() {
  sensor.update();

  Serial.println(sensor.getDegreeX());
  
  delay(50);
  
  
  updateTasten();
}


void updateTasten() {
  for (int i = 0; i < 60; i++) {
    tasten[i].update();
  }
}

