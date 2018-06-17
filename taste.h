#ifndef TASTE_H
#define TASTE_H
 
#include <Arduino.h>
#include <Chrono.h>
 
class Taste {
public:
		Taste();
		~Taste();
		void play(int time=300, int velo=50);
		void setNote(int midinote);
		void update();
		static void midibegin();
		
private:
		int midinote;
		Chrono timer;
		int duration;
};
 
#endif
