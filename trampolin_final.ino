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
Chrono repeatTimer; // mode 7

double offset = 0;
double x, y;
int mode = 0;
int jump_counter = 0;
const int MAX_LAST_VALUES = 3;
const float TOLERANCE = 0;
float lastValues[MAX_LAST_VALUES];
int total_time = millis();
int last_jump_time = 0;
int jump_interval = 0;
int jump_depth = 0;
int jumps_when_enter = 0;

// composition vars
int blackkeys[] = {1, 3, 6, 8, 11, 13, 15, 18, 20, 23, 25, 27, 30, 32, 35, 37, 39, 42, 44, 47, 49, 51, 54, 56, 59};
int whitekeys[] = {0, 2, 4, 5, 7, 9, 10, 12, 14, 16, 17, 19, 21, 22, 24, 26, 28, 29, 31, 33, 34, 36, 38, 40, 41, 43, 45, 46, 48, 50, 52, 53, 55, 57, 58};
int welcome[] = {6, 13, 20, 21, 25, 30, 32, 35, 37, 42, 59, 54};
bool direction = true;
bool b_direction = true;
bool w_direction = true;
int note = 0;
int note2 = 0;
int note3 = 0;
int newVelo = 0;
int xNote;

int part = 0;
int partNote = 0;
int partNote2 = 0;
bool enterModeFirstTime[20]; // wieder auf true setzen bei modewechsel
bool enterPartFirstTime[20]; // wieder auf true setzen bei partwechselwechsel
int partNote_minus3;
int partNote_minus5;
int partNote_minus8;

int record[900];

int mayor[] = {0, 2, 2, 1, 2, 2, 2, 1}; // 0 2 4 5 7 9 11 12
int minor[] = {0, 2, 1, 2, 2, 1, 2, 2};
int minor_seven[] = {3, 4, 3, 2};

int scaled;

int rec = 0;
int bla = 0;

void setup()
{
  sensor.init();
  Taste::midibegin();
  Serial.begin(115200);
  pinMode(11, OUTPUT);       // Taster
  pinMode(12, INPUT_PULLUP); // Taster

  for (int i = 0; i < 60; i++)
  {
    tasten[i].setNote(i + 31);
  }

  repeatTimer.stop();
  //memset(enterModeFirstTime, true, sizeof(enterModeFirstTime)); // wird in case 0 eh gemacht
}

void loop()
{
  sensor.update();
  //printDegrees();
  x = sensor.getDegreeX();
  y = sensor.getDegreeY();
  total_time = millis();

  if (abs(lastValues[MAX_LAST_VALUES - 1] - x) > TOLERANCE)
  {
    for (int i = 0; i < MAX_LAST_VALUES - 1; i++)
    {
      lastValues[i] = lastValues[i + 1];
    }
    lastValues[MAX_LAST_VALUES - 1] = x;
    /*Serial.print(lastValues[0]);      Serial.print("'");      Serial.print(lastValues[1]);      Serial.print("'");      Serial.println(lastValues[2]);*/
  }

  /* JUMP PEAK TRIGGER */

  if (lastValues[0] < lastValues[1] && lastValues[1] > lastValues[2] && abs(x) > 18 && jumpTimer.hasPassed(200))
  {
    jump_interval = total_time - last_jump_time;
    last_jump_time = total_time;
    jumpTimer.restart();
    jump_counter++;
    jump_depth = (int)lastValues[1];
    offTimer.stop();

    Serial.print(" JUMP #");    Serial.print(jump_counter);    Serial.print("\tINTERVAL: ");    Serial.print(jump_interval);    Serial.print("\tDEPTH: ");
    Serial.println(jump_depth);
  }

  /*if((int)lastValues[0] == 0 && (int)lastValues[1] == 0 && lastValues[2] > 0.5) {
      if(mode == 7) {
      }
    }*/

  /* CHANGING THE MODE */  

  int testmode = 0; // beware the jump counter

  if (testmode > 0)
  {
    if (mode == 0 && x > 1)
    {
      gotoMode(testmode);
    }
  }
  else
  {

    // got some input
    if (x > 1)
    {
      offTimer.stop();
    }

    // transition mode 0 to 1 on input
    if (mode == 0 && x > 1)
    {
      gotoMode(1);
    }

    // transition mode 2 to 3 after 3 jumps
    if (mode == 2 && jump_counter == 2)
    {
      gotoMode(3);
    }

    // transition mode 3 to 4 after 10 jumps
    if (mode == 3 && jump_counter == 6)
    {
      gotoMode(4);
    }

    if (mode == 4 && jump_counter == 24)
    {
      gotoMode(5);
      stopAll();
    }
    if (mode == 5 && jump_counter == 28)
    {
      gotoMode(6);
      stopAll();
    }

    // transition mode 6 to 7 after 4 jumps later
    if (mode == 6 && jump_counter == 32)
    {
      gotoMode(7);
      stopAll();
    }
    // transition mode 7 to 8
    if (mode == 7 && jump_counter == 38)
    {
      gotoMode(8);
      stopAll();
    }
  }

  // back to mode 0
  if (mode != 0 && x < 1)
  {
    if (!offTimer.isRunning())
    {
      offTimer.restart();
    }

    if (offTimer.hasPassed(1500))
    {
      Serial.println("Abgestiegen");
      gotoMode(0);
      stopAll();
    }
  }

  /* COMPOSITION  */

  switch (mode)
  {
  case 0:

    offTimer.stop();
    jump_counter = 0;
    memset(enterModeFirstTime, true, sizeof(enterModeFirstTime));
    Taste::overrideVelo(0);

    break;
  case 1:

    if(entering(1)) {
      Taste::setPedal(true);
      Serial.println("welcome! entering");

      for (int i = 0; i < 12; i++) // play welcome chord, waiting to start jumping
      {
        tasten[welcome[i]].playDelayed(i * (300 - 10 * i), 200);
        tasten[welcome[i]].playDelayed(i * (200 - 10 * i), 200);
      }
    }
    //gotoMode(2);

    break;
  case 2: 

    break;
  case 3:

  {

    if (entering(3))
    {
      jumps_when_enter = jump_counter;
      playTimer.stop();
      Taste::setPedal(false);
    }

    if (onJump())
    {
      if (jump_counter % 2 == 0)
      {
        //Taste::overrideVelo(2);
      }
      else
      {
        //Taste::overrideVelo(50);
      }
    }

    int jump_counter_mode3 = jump_counter - jumps_when_enter;

    const int tempo = 100 - ((jump_counter_mode3)*5); // ist irgendwann zu schnell und hört auf
    //const int tempo = 30;
    Serial.println(tempo);

    if (x < 1)
    {

      if (playTimer.hasPassed(tempo, true))
      {

        tasten[blackkeys[partNote]].play(70, 100);
        tasten[whitekeys[partNote2]].play(70, 100);

        if (b_direction)
        {
          partNote++;
          if (partNote > 24)
          {
            partNote = 23;
            b_direction = !b_direction;
          }
        }
        else
        {
          partNote--;
          if (partNote < 0)
          {
            partNote = 1;
            b_direction = !b_direction;
          }
        }

        if (w_direction)
        {
          partNote2++;
          if (partNote2 > 34)
          {
            partNote2 = 33;
            w_direction = !w_direction;
          }
        }
        else
        {
          partNote2--;
          if (partNote2 < 0)
          {
            partNote2 = 1;
            w_direction = !w_direction;
          }
        }
      }
    }
  }

  break;
  case 4:

    if (enterModeFirstTime[4])
    {
      jumps_when_enter = jump_counter;
      enterModeFirstTime[4] = false;
      Taste::setPedal(true);
      Taste::overrideVelo(0);
      stopAll();
    }

    if (onJump()) {
      playTimer.restart();
      xNote = map(jump_depth, 18, 40, 0, 59);
      tasten[xNote].play(150, 100);
      tasten[xNote + 1].play(150, 100);
      tasten[xNote + 5].play(150, 100);
    }

    // xNote wird beim Trigger definiert

    if (playTimer.hasPassed(jump_interval / 3) & playTimer.isRunning())
    {
      Serial.println(xNote);
      tasten[xNote + 12].play(80, 100);
      tasten[xNote + 1 + 12].play(80, 100);
      tasten[xNote + 5 + 12].play(80, 100);
      playTimer.stop();
    }

    /*if (playTimer.hasPassed(jump_interval / 3 * 2) & playTimer.isRunning())
    {
      Serial.println(xNote);
      tasten[xNote+5+24].play(150, 100);
      playTimer.stop();
    }*/

    break;
  case 5:
    if(onJump()) {
      Taste::setPedal(jump_counter % 8 < 4); // gibt true or false
    }

    if (veloTimer.hasPassed(20 * 70, true))
    {
      Taste::overrideVelo(newVelo % 50 + 1);
      newVelo++;
      Serial.println(newVelo % 50 + 1);
    }

    if (playTimer.hasPassed(70, true) && !tasten[note % 20].isPlaying())
    {
      tasten[note % 20].play(50);
      note++;
    }
    if (playTimer2.hasPassed(70, true) && !tasten[note2 % 20 + 20].isPlaying())
    {
      tasten[note2 % 20 + 20].play(50);
      note2++;
    }

    if (playTimer3.hasPassed(70, true) && !tasten[note3 % 20 + 40].isPlaying())
    {
      tasten[note3 % 20 + 40].play(50);
      note3++;
    }

    break;
  case 6: // mode 6 with parts

    if (enterModeFirstTime[6] == true)
    {
      partTimer.restart();
      enterModeFirstTime[6] = false;
      Serial.println("first time entering case");
    }

    if(onJump()) {
      partNote = jump_depth;
      Serial.print("PARTNOTE: ");
      Serial.println(partNote);
      partNote_minus3 = partNote - 3;
      partNote_minus5 = partNote - 5;
      partNote_minus8 = partNote - 8;
      part = 0;
      playTimer.restart();
    }

    switch (part)
    {

    case 0: // part 0
      if (playTimer.hasPassed(23, true))
      {

        xNote = map((int)x, 0, 40, 0, 59);

        if (x > 1)
        {
          if (xNote != 17)
          {
          tasten[xNote].play(40, 127);
          }
        }
        //if(partTimer.hasPassed(2000, true)) { //part++; };
      }
      break;
    case 1: // part 1: waiting for next jump

      break;
    }

    break;
  case 7: // mode 7: like 6 but with echo

  {
    if(onJump()) {
      partNote = jump_depth;
      playTimer.restart();
      repeatTimer.restart();
    }

    const int interval = 35;      // milliseconds between reorded/played notes
    const int offset = 0;         // echo notes offset
    const int max_echo_notes = 7; // how many last recorded notes

    if (x > 2)
    {
      xNote = map((int)x, 0, 40, 0, 59);

      if (playTimer.hasPassed(interval, true))
      {
        if (xNote != 17)
        {
          tasten[xNote].play(40, 127);
        }
        record[rec] = xNote;
        rec++;
      }
    }

    if (repeatTimer.hasPassed(jump_interval / 3) && repeatTimer.isRunning() && jump_interval < 1400)
    {
      Serial.println("Playing Echo");

      for (int i = rec < max_echo_notes ? rec : rec - max_echo_notes; i < rec; i++)
      {
        if ((record[i] + offset) != 17 && (record[i] + offset) < 60)
        {
          tasten[(record[i] + offset) % 60].playDelayed(i * interval, 40, 127);
        }
      }

      repeatTimer.stop();
      rec = 0;
      memset(record, 0, sizeof(record));
    }
  }
  break;
  case 8: // mode 8 working with scales

    if (enterModeFirstTime[8])
    {
      Taste::setPedal(true);
      Serial.println("Mode 8 Pedal wurde getätigt.");
      enterModeFirstTime[8] = false;
    }

    if (onJump())
    {
      partNote = jump_depth;
      playTimer.restart();
    }

    for (int i = 0, scaled = 6; scaled < 60; scaled += minor_seven[i % 4], i++)
    {
      tasten[scaled].playDelayed(i * 50, 50, 40);
    }
    gotoMode(9);

    break;
  case 9: // mode 9 nothing

    break;
  case 10: // mode 10 from middle to the sides

    if (entering(10))
    {
      Taste::setPedal(true);
      partNote = 30;
      partNote2 = 29;
      Serial.println("first time entering mode 10");
    }

    {


      if (playTimer.hasPassed(30, true))
      {
        //partNote += minor[partNote % 8];

        if (!direction)
        {
          tasten[partNote].play(80, 127);
          partNote++;
        }
        else
        {
          tasten[partNote2].play(80, 127);
          partNote2--;
        }


          if (partNote > 59)
          {
            partNote = 30;
          }
          if (partNote2 < 0)
          {
            partNote2 = 29;
            gotoMode(11);
          }

        direction = !direction;
      }

    }
    break;
  case 11:
    if(entering(11)) {
      partNote = 1;
    }

    if(onJump() && jump_counter%4 < 2) {
      Taste::overrideVelo(1);
    } else if(onJump() && jump_counter%4 > 2) {
      Taste::overrideVelo(100);
    }

    if(playTimer.hasPassed(100, true)) {
      tasten[random(0,59)].play(60);
      if(partNote%5 == 0) {
        playTimer.delay(200);
      }
    }

    partNote++;

    break;
  }

  if (digitalRead(12) == LOW)
  {
    sensor.calibrate();
  }
  updateTasten();
}

void updateTasten()
{
  for (int i = 0; i < 60; i++)
  {
    tasten[i].update();
  }
}

void printDegrees()
{
  Serial.print("X: ");
  Serial.print(x);
  Serial.print(" Y: ");
  Serial.println(y);
}

void gotoMode(int m)
{
  if (mode != m)
  {
    mode = m;
    Taste::overrideVelo(0);
    Serial.print("Mode: ");
    Serial.println(m);
  }
}

bool onJump() {
    if(jumpTimer.elapsed() == 0 || jumpTimer.elapsed() == 1 || jumpTimer.elapsed() == 2) {
      return true;
    } else {
      return false;
    }
}

bool entering(int mode) {
  if (enterModeFirstTime[mode]) {
    enterModeFirstTime[mode] = false;
    return true;
  } else {
    return false;
  }
}

void stopAll()
{
  for (int i = 0; i < 60; i++)
  {
    tasten[i].stop();
  }
}
