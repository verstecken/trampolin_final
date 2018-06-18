#include <Chrono.h>
#include "taste.h"
#include "sensor.h"

Sensor sensor;
Taste tasten[60];

Chrono playTimer;
Chrono playTimer2;
Chrono playTimer3;
Chrono offTimer;
Chrono halfTimer;
Chrono jumpTimer;
Chrono veloTimer;
Chrono partTimer;

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
int jump_depth;

// composition vars
int blackkeys[] = {1, 3, 6, 8, 11, 13, 15, 18, 20, 23, 25, 27, 30, 32, 35, 37, 39, 42, 44, 47, 49, 51, 54, 56, 59};
int whitekeys[] = {0, 2, 4, 5, 7, 9, 10, 12, 14, 16, 17, 19, 21, 22, 24, 26, 28, 29, 31, 33, 34, 36, 38, 40, 41, 43, 45, 46, 48, 50, 52, 53, 55, 57, 58};
int welcome[] = {6, 13, 20, 21, 25, 30, 32, 35, 37, 42, 59, 54};
bool direction = true;
int note = 0;
int note2 = 0;
int note3 = 0;
int newVelo = 0;
int xNote;

int part = 0;
int partNote = 0;
int enterModeFirstTime = true; // wieder auf true setzen bei modewechsel
int enterPartFirstTime = true; // wieder auf true setzen bei partwechselwechsel
int partNote_minus3;
int partNote_minus5;
int partNote_minus8;



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

  if (abs(lastValues[MAX_LAST_VALUES - 1] - x) > TOLERANCE) {

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
    Serial.print(jump_interval);
    Serial.print("JUMPS # ");
    Serial.println(jump_counter);
    jumpTimer.restart();
    jump_counter++;
    jump_depth = (int)lastValues[1];

    Serial.print("TIEFE: ");
    Serial.println();

    if (mode == 3) {
      if (jump_counter % 2 == 0) {
        Taste::overrideVelo(2);
      } else {
        Taste::overrideVelo(50);
      }
    }
    if (mode == 5) {
        Taste::setPedal(jump_counter % 8 < 4); // gibt true or false
    }
    if (mode == 6) {
      //partNote = map(jump_depth, 0, 40, 4, 60);
      partNote = jump_depth;
      Serial.print("PARTNOTE: ");
      Serial.println(partNote);
      partNote_minus3 = partNote-3;
      partNote_minus5 = partNote-5;
      partNote_minus8 = partNote-8;
      part = 0;
      playTimer.restart(); 
    }
  }

  /*if ((int)lastValues[0] == 0 && (int)lastValues[1] == 0 && lastValues[2] > 0.5) {
      //tasten[54].play(); // aufkommen
    }*/

  int testmode = 6; // beware the jump counter

  if(testmode > 0) {

  if (mode == 0 && x > 1) {
    gotoMode(testmode);
  }

  } else {

    /* CHANGING THE MODE */

    // got some input
    if (x > 1) {
      offTimer.stop();
    }

    // transition mode 0 to 1 on input
    if (mode == 0 && x > 1) {
      Taste::setPedal(true);
      gotoMode(1);
    }

    // transition mode 2 to 3 after 3 jumps
    if (mode == 2 && jump_counter > 2) {
      gotoMode(3);
      playTimer.stop();
      Taste::setPedal(false);
    }

    // transition mode 3 to 4 after 10 jumps
    if (mode == 3 && jump_counter > 10) {
      gotoMode(4);
      Taste::overrideVelo(0);
      stopAll();
    }

    if (mode == 4 && jump_counter > 24) {
      gotoMode(5);
      stopAll();
    }
    if (mode == 5 && jump_counter > 28) {
      gotoMode(6);
      stopAll();
    }
    
  }

  // back to mode 0
  if (mode != 0 && x < 1) {
    if (!offTimer.isRunning()) {
      offTimer.restart();
    }

    if (offTimer.hasPassed(900)) {
      stopAll();
    }
    if (offTimer.hasPassed(1500)) {
      Serial.println("Abgestiegen");
      gotoMode(0);
    }
  }




  switch (mode) {
    case 0:
      offTimer.stop();
      jump_counter = 0;

      break;
    case 1:

      /*if (playTimer.hasPassed(500)) {
        playTimer.restart();
        tasten[30].play(150, 10);
        }*/

      for (int i = 0; i < 12; i++) {
        tasten[welcome[i]].playDelayed(i * (300 -10*i), 200);
        tasten[welcome[i]].playDelayed(i * (200 -10*i), 200);
      }
      gotoMode(2);

      break;
    case 2: // waiting to start jumping
    
      break;
    case 3:
      if (!playTimer.isRunning()) {

        for (int i = 0; i < 25; i++) {
          if (direction) {
            tasten[blackkeys[i]].playDelayed(i * 70, 60, 50);
          } else {
            tasten[blackkeys[24 - i]].playDelayed(i * 70, 60, 50);
          }
        }
        for (int i = 0; i < 35; i++) {
          if (direction) {
            tasten[whitekeys[i]].playDelayed(i * 70, 60, 50);
          } else {
            tasten[whitekeys[34 - i]].playDelayed(i * 70, 60, 50);
          }
        }


        playTimer.restart();
      } else if (playTimer.hasPassed(25 * 70)) {
        playTimer.stop();
        direction = !direction;
      }




      break;
    case 4:

      if (playTimer.hasPassed(jump_interval / 2, true)) {
        tasten[35].play(150, 10);
      }

      break;
    case 5:

      if (veloTimer.hasPassed(20*70, true)) {
        Taste::overrideVelo(newVelo % 50 + 1);
        newVelo++;
        Serial.println(newVelo % 50 + 1);
      }

      if (playTimer.hasPassed(70, true) && !tasten[note % 20].isPlaying()) {
        tasten[note % 20].play(50);
        note++;
      }
      if (playTimer2.hasPassed(70, true) && !tasten[note2 % 20 + 20].isPlaying()) {
        tasten[note2 % 20 + 20].play(50);
        note2++;
      }

      if (playTimer3.hasPassed(70, true) && !tasten[note3 % 20 + 40].isPlaying()) {
        tasten[note3 % 20 + 40].play(50);
        note3++;
      }

      break;
    case 6:
      
      if(enterModeFirstTime == true) {
        partTimer.restart();
        enterModeFirstTime = false;
        Serial.println("first time entering case");
      }

      switch(part) {
      
      case 0: // part 0
        if(playTimer.hasPassed(23, true)) {
          
          xNote = map((int)x, 0, 40, 0, 60);

          if(x > 1) {
            tasten[xNote].play(40, 127);
          }
          //if(partTimer.hasPassed(2000, true)) { //part++; };
        }
      break;
      case 1: // part 1: waiting for next jump

      break;

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

void stopAll() {
  for (int i = 0; i < 60; i++) {
    tasten[i].stop();
  }
}

