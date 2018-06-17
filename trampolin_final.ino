#include <Chrono.h>
#include "taste.h"
#include "sensor.h"

Sensor sensor;
Taste tasten[60];

Chrono playTimer;
Chrono offTimer;
Chrono halfTimer;
Chrono jumpTimer;

double offset = 0;
double x, y;
int mode = 0;
int jump_counter = 0;
const int MAX_LAST_VALUES = 3;
const float TOLERANCE = 0;
float lastValues[MAX_LAST_VALUES];
int total_time;
int last_jump_time;
int jump_interval;

void setup() {
  sensor.init();
  Taste::midibegin();
  Serial.begin(115200);
  pinMode(11, OUTPUT);      // Taster
  pinMode(12, INPUT_PULLUP); // Taster

  for (int i = 0; i < 60; i++) {
    tasten[i].setNote(i + 31);
  }
}

void loop() {
  sensor.update();
  //debug();
  x = sensor.getDegreeX();
  y = sensor.getDegreeY();
  total_time = millis();

  if(abs(lastValues[MAX_LAST_VALUES - 1] - x) > TOLERANCE) {
  
    for (int i = 0; i < MAX_LAST_VALUES - 1; i++) {
      lastValues[i] = lastValues[i + 1];
    }
    lastValues[MAX_LAST_VALUES - 1] = x;
    
  /*Serial.print(lastValues[0]);
  Serial.print("'");
  Serial.print(lastValues[1]);
  Serial.print("'");
  Serial.println(lastValues[2]);*/
  }

  // TRIGGER
  if (lastValues[0] < lastValues[1] && lastValues[1] > lastValues[2] && abs(x) > 18 && jumpTimer.hasPassed(200)) {
    //tasten[5].play(); // comming up
    jump_interval = total_time - last_jump_time;
    last_jump_time = total_time;
    Serial.print("INTERVAL: ");
    Serial.println(jump_interval);
    jumpTimer.restart();
    jump_counter++;
  }

  /*if ((int)lastValues[0] == 0 && (int)lastValues[1] == 0 && lastValues[2] > 0.5) {
      //tasten[54].play(); // aufkommen
  }*/

  


  if (x > 1) {
    offTimer.stop();
  }

  if (mode == 0 && x > 1) {
    gotoMode(1);
  }
  
  if (mode == 1 && jump_counter > 2) {
    gotoMode(2);
    playTimer.restart();
  }
  
  if (mode == 2 && jump_counter > 10) {
    gotoMode(3);
    playTimer.restart();
  }




  // back to mode 0
  if (mode != 0 && x < 1) {
    if (!offTimer.isRunning()) {
      offTimer.restart();
    }

    if (offTimer.hasPassed(1400)) {
      gotoMode(0);
      Serial.println("off nach timer");
    }
  }




  switch (mode) {
    case 0:
      offTimer.stop();
      jump_counter = 0;

      break;
    case 1:

      if (playTimer.hasPassed(500)) {
        playTimer.restart();
        tasten[30].play(150, 10);
      }

      break;
    case 2:

      if (playTimer.hasPassed(jump_interval)) {
        playTimer.restart();
        tasten[33].play(150, 10);
      }

      break;
    case 3:

      if (playTimer.hasPassed(jump_interval/2)) {
        playTimer.restart();
        tasten[35].play(150, 10);
      }

      break;
  }

  if (digitalRead(12) == LOW) {
    sensor.calibrate();
  }
  updateTasten();
}


void updateTasten() {
  for (int i = 0; i < 60; i++) {
    tasten[i].update();
  }
}

void debug() {
  Serial.print("X: ");
  Serial.print(x);
  Serial.print(" Y: ");
  Serial.println(y);
}

void gotoMode(int m) {
  if (mode != m) {
    mode = m;
    Serial.print("Mode: ");
    Serial.println(m);
  }
}

