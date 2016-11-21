#include "sound.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

// Note durations in milliseconds
#define N_FULL 1500
#define N_HALF N_FULL/2
#define N_QUARTER N_HALF/2
#define N_EIGHTH N_QUARTER/2
#define N_SIXTEENTH N_EIGHTH/2
#define N_32 N_SIXTEENTH/2

// DAC device file handle
static int dac;


// Play a frequency for a duration given in milliseconds
static void playFreq(int freq, int duration_ms) {
	// Convert freq to char array
	char str[5];
	sprintf(str, "%d", freq);

	// Write freq to dac
	write(dac, str, 5);

	// Sleep for duration while freq plays
	usleep(duration_ms*1000);
}

// Call at the end of a note sequence
static void finishPlay(void) {
	// Disable dac
	write(dac, "0", 1);
}

// Get frequency for note in a certain octave
static int noteFreq(char note, int octave)
{
	switch (note) {
	case 'C':
		return 16.35 * (2 << (octave - 1));
	case 'D':
		return 18.35 * (2 << (octave - 1));
	case 'E':
		return 20.60 * (2 << (octave - 1));
	case 'F':
		return 21.83 * (2 << (octave - 1));
	case 'G':
		return 24.50 * (2 << (octave - 1));
	case 'A':
		return 27.50 * (2 << (octave - 1));
	case 'B':
		return 30.87 * (2 << (octave - 1));
	default:
		return 0;
	}
}



// Initialize sound system
void initSound() {
	// Open device
	dac = open("/dev/dac", O_RDWR);
}

// Play score sound
void soundWinning() {
	playFreq(noteFreq('C',4), N_EIGHTH);
	playFreq(noteFreq('D',4), N_EIGHTH);
	playFreq(noteFreq('E',4), N_EIGHTH);
	playFreq(noteFreq('G',4), N_QUARTER);
	playFreq(noteFreq('E',4), N_EIGHTH);
	playFreq(noteFreq('G',4), N_HALF);
	finishPlay();
}

// Play score sound
void soundScore() {
	playFreq(noteFreq('G',2), N_QUARTER);
	playFreq(noteFreq('E',2), N_QUARTER);
	playFreq(noteFreq('G',2), N_QUARTER);
	playFreq(noteFreq('E',2), N_QUARTER);
	playFreq(noteFreq('D',2), N_QUARTER);
	playFreq(noteFreq('C',2), N_QUARTER);
	finishPlay();
}

// Play startup sound
void soundStartup() {
	playFreq(noteFreq('A',4), N_EIGHTH);
	playFreq(noteFreq('F',4), N_EIGHTH);
	playFreq(noteFreq('C',4), N_EIGHTH);
	playFreq(noteFreq('G',4), N_EIGHTH);
	playFreq(noteFreq('D',4), N_EIGHTH);
	playFreq(noteFreq('C',4), N_EIGHTH);
	playFreq(noteFreq('A',4), N_EIGHTH);
	playFreq(noteFreq('D',4), N_EIGHTH);
	playFreq(noteFreq('E',4), N_EIGHTH);
	playFreq(noteFreq('F',4), N_EIGHTH);
	playFreq(noteFreq('G',4), N_EIGHTH);
	playFreq(noteFreq('A',4), N_EIGHTH);
	playFreq(noteFreq('F',4), N_EIGHTH);
	playFreq(noteFreq('E',4), N_EIGHTH);
	playFreq(noteFreq('C',4), N_EIGHTH);
	playFreq(noteFreq('A',4), N_EIGHTH);
	finishPlay();
}
