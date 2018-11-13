/*
 * sampler.cpp
 *
 *  Created on: Oct 22, 2018
 *      Author: dean
 */

#include "sampler.h"
#include "recorder.h"
#include "Adafruit_SPIFlash.h"
#include "Adafruit_QSPI_GD25Q.h"
extern Adafruit_QSPI_GD25Q flash;

AudioPlayMemory    Sampler::sounds[NUM_SOUNDS] = {
		AudioPlayMemory(), AudioPlayMemory(), AudioPlayMemory(),
		AudioPlayMemory(), AudioPlayMemory(), AudioPlayMemory(),
};

AudioPlayQspiRaw Sampler::recordings[NUM_RECORDINGS] = {
		AudioPlayQspiRaw(), AudioPlayQspiRaw(), AudioPlayQspiRaw(), AudioPlayQspiRaw(),
		AudioPlayQspiRaw(), AudioPlayQspiRaw(), AudioPlayQspiRaw(), AudioPlayQspiRaw(),
};

const unsigned int *Sampler::soundFiles[NUM_SOUNDS] = {
		AudioSampleBd01, AudioSampleBd05, AudioSampleCp02, AudioSampleCr01,
		AudioSampleHh01, AudioSampleOh03, AudioSampleRs01, AudioSampleSd01
};

// TODO: samples can be stereo but we're currently only using mono

AudioMixer4        Sampler::mix1;
AudioMixer4        Sampler::mix2;
AudioMixer4        Sampler::mix3;
AudioMixer4        Sampler::mix4;
AudioMixer4        Sampler::mix5;

AudioConnection cBuiltin1(Sampler::sounds[0], 0, Sampler::mix1, 0);
AudioConnection cBuiltin2(Sampler::sounds[1], 0, Sampler::mix1, 1);
AudioConnection cBuiltin3(Sampler::sounds[2], 0, Sampler::mix1, 2);
AudioConnection cBuiltin4(Sampler::sounds[3], 0, Sampler::mix1, 3);
AudioConnection cBuiltin5(Sampler::mix1, 0, Sampler::mix2, 0);
AudioConnection cBuiltin6(Sampler::sounds[4], 0, Sampler::mix2, 1);
AudioConnection cBuiltin7(Sampler::sounds[5], 0, Sampler::mix2, 2);
AudioConnection cBuiltin8(Sampler::sounds[6], 0, Sampler::mix2, 3);
AudioConnection cBuiltin9(Sampler::mix2, 0, Sampler::mix3, 0);
AudioConnection cBuiltin10(Sampler::sounds[7], 0, Sampler::mix3, 1);
AudioConnection cRec1(Sampler::recordings[0], 0, Sampler::mix3, 2);
AudioConnection cRec2(Sampler::recordings[1], 0, Sampler::mix3, 3);
AudioConnection cRec3(Sampler::mix3, 0, Sampler::mix4, 0);
AudioConnection cRec4(Sampler::recordings[2], 0, Sampler::mix4, 1);
AudioConnection cRec5(Sampler::recordings[3], 0, Sampler::mix4, 2);
AudioConnection cRec6(Sampler::recordings[4], 0, Sampler::mix4, 3);
AudioConnection cRec7(Sampler::mix4, 0, Sampler::mix5, 0);
AudioConnection cRec8(Sampler::recordings[5], 0, Sampler::mix5, 1);
AudioConnection cRec9(Sampler::recordings[6], 0, Sampler::mix5, 2);
AudioConnection cRec10(Sampler::recordings[7], 0, Sampler::mix5, 3);

void Sampler::begin()
{
	// reduce the gain on mixer channels, so more than 1
	// sound can play simultaneously without clipping
	setSolo(SOLO_NONE);
}

void Sampler::setSolo(uint8_t solo)
{
	_solo = solo;
	if(_solo == SOLO_KIT){
		mix1.gain(0, VOL_DEFAULT);
		mix1.gain(1, VOL_DEFAULT);
		mix1.gain(2, VOL_DEFAULT);
		mix1.gain(3, VOL_DEFAULT);
		mix2.gain(1, VOL_DEFAULT);
		mix2.gain(2, VOL_DEFAULT);
		mix2.gain(3, VOL_DEFAULT);
		mix3.gain(1, VOL_DEFAULT);
		mix3.gain(2, 0.0);
		mix3.gain(3, 0.0);
		mix4.gain(1, 0.0);
		mix4.gain(2, 0.0);
		mix4.gain(3, 0.0);
		mix5.gain(1, 0.0);
		mix5.gain(2, 0.0);
		mix5.gain(3, 0.0);
	}
	else if(_solo == SOLO_REC){
		mix1.gain(0, 0.0);
		mix1.gain(1, 0.0);
		mix1.gain(2, 0.0);
		mix1.gain(3, 0.0);
		mix2.gain(1, 0.0);
		mix2.gain(2, 0.0);
		mix2.gain(3, 0.0);
		mix3.gain(1, 0.0);
		mix3.gain(2, VOL_DEFAULT_REC);
		mix3.gain(3, VOL_DEFAULT_REC);
		mix4.gain(1, VOL_DEFAULT_REC);
		mix4.gain(2, VOL_DEFAULT_REC);
		mix4.gain(3, VOL_DEFAULT_REC);
		mix5.gain(1, VOL_DEFAULT_REC);
		mix5.gain(2, VOL_DEFAULT_REC);
		mix5.gain(3, VOL_DEFAULT_REC);
	}
	else{
		mix1.gain(0, VOL_DEFAULT);
		mix1.gain(1, VOL_DEFAULT);
		mix1.gain(2, VOL_DEFAULT);
		mix1.gain(3, VOL_DEFAULT);
		mix2.gain(1, VOL_DEFAULT);
		mix2.gain(2, VOL_DEFAULT);
		mix2.gain(3, VOL_DEFAULT);
		mix3.gain(1, VOL_DEFAULT);
		mix3.gain(2, VOL_DEFAULT_REC);
		mix3.gain(3, VOL_DEFAULT_REC);
		mix4.gain(1, VOL_DEFAULT_REC);
		mix4.gain(2, VOL_DEFAULT_REC);
		mix4.gain(3, VOL_DEFAULT_REC);
		mix5.gain(1, VOL_DEFAULT_REC);
		mix5.gain(2, VOL_DEFAULT_REC);
		mix5.gain(3, VOL_DEFAULT_REC);
	}
}

void Sampler::playSound(uint8_t num)
{
	if(num < NUM_SOUNDS)
		sounds[num].play(soundFiles[num]);
	else if(num < NUM_SOUNDS+NUM_RECORDINGS){
		num -= NUM_SOUNDS;
		recordings[num].play(REC_FILESIZE*num);
	}
}

void AudioPlayQspiRaw::play(uint32_t addr)
{
	// get length from file
	flash.readMemory(addr, (byte*)&length, sizeof(uint32_t), false);
	playing = true;
	_addr = addr + W25Q16BV_SECTORSIZE;
	Serial.println(length);
}

void AudioPlayQspiRaw::stop(void)
{
	playing = 0;
}

void AudioPlayQspiRaw::update(void)
{
	audio_block_t *block;
	int16_t *out;
	uint32_t consumed;

	if (!playing) return;
	block = allocate();
	if (block == NULL) return;

	out = block->data;

	if (playing) {
		flash.readMemory(_addr, (uint8_t *)out, AUDIO_BLOCK_SAMPLES*2, false);
		consumed = AUDIO_BLOCK_SAMPLES*2;
		_addr += consumed;
	}

	if (length > consumed) {
		length -= consumed;
	} else {
		playing = 0;
	}
	transmit(block);
	release(block);
}



