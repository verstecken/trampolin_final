#ifndef TASTE_H
#define TASTE_H

#include <Arduino.h>
#include <Chrono.h>

class Taste {
  public:
    Taste();
    ~Taste();
    void play(int time = 300, int velo = 50);
    void playDelayed(int delay, int time = 300, int velo = 50);
    void setNote(int midinote);
    void update();
    static void midibegin();
    bool isPlaying();
    void stop();
    static void overrideVelo(int velo);
    static void setPedal(bool press);

  private:
    static int globalVelo;
    int midinote;
    Chrono timer;
    int duration;

    Chrono delayTimer;
    int delay;
    int delayDuration;
    int delayVelo;
};

#endif
