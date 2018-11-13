/*
 * sampler.h
 *
 *  Created on: Oct 22, 2018
 *      Author: dean
 */

#ifndef SAMPLER_H_
#define SAMPLER_H_

#include <Audio.h>

#define NUM_SOUNDS 8
#define NUM_RECORDINGS 8

// WAV files converted to code by wav2sketch
#include "AudioSampleBd01.h"
#include "AudioSampleBd05.h"
#include "AudioSampleCp02.h"
#include "AudioSampleCr01.h"
#include "AudioSampleHh01.h"
#include "AudioSampleOh03.h"
#include "AudioSampleRs01.h"
#include "AudioSampleSd01.h"
#include "AudioStream.h"

#define SOLO_NONE 0
#define SOLO_REC 1
#define SOLO_KIT 2

#define VOL_DEFAULT 1.0
#define VOL_DEFAULT_REC 0.3

class AudioPlayQspiRaw : public AudioStream
{
public:
	AudioPlayQspiRaw(void) : AudioStream(0, NULL) { begin(); }
	void begin(void) {}
	void play(uint32_t addr);
	void stop(void);
	bool isPlaying(void) { return playing; }
	void update(void);
private:
	uint32_t length;
	volatile uint32_t _addr;
	volatile bool playing;
};

class Sampler {
public:
	Sampler() { _solo = SOLO_NONE; }
	~Sampler() {}
	void begin();
	void playSound(uint8_t num);
	uint8_t getSolo(){ return _solo; }
	void setSolo(uint8_t solo);

	static AudioPlayMemory	  sounds[NUM_SOUNDS];
	static AudioPlayQspiRaw	  recordings[NUM_RECORDINGS];
	static const unsigned int *soundFiles[NUM_SOUNDS];
	static AudioMixer4        mix1;
	static AudioMixer4        mix2;
	static AudioMixer4        mix3;
	static AudioMixer4        mix4;
	static AudioMixer4        mix5;
private:
	uint8_t _solo;
};



#endif /* SAMPLER_H_ */
