/*
 * controls.h
 *
 *  Created on: Oct 22, 2018
 *      Author: dean
 */

#ifndef CONTROLS_H_
#define CONTROLS_H_

#include <Adafruit_NeoTrellisM4.h>
#include <Adafruit_ADXL343.h>

#define NUM_KEYS 32

#define KEY_BUILTIN1 		0
#define KEY_BUILTIN2 		1
#define KEY_BUILTIN3 		2
#define KEY_BUILTIN4 		3
#define KEY_BUILTIN5 		4
#define KEY_BUILTIN6 		5
#define KEY_BUILTIN7 		6
#define KEY_BUILTIN8 		7

#define KEY_SAMPLE1 		8
#define KEY_SAMPLE2 		9
#define KEY_SAMPLE3 		10
#define KEY_SAMPLE4 		11
#define KEY_SAMPLE5 		12
#define KEY_SAMPLE6 		13
#define KEY_SAMPLE7 		14
#define KEY_SAMPLE8 		15

#define KEY_SOUND			16
#define KEY_STUTTER			17
#define KEY_JUMP			18
#define KEY_CRUSH			19
#define KEY_SOLO_KIT		20
#define KEY_TEMPO_UP		21
#define KEY_VOL_UP			22
#define KEY_REC				23

#define KEY_WRITE			24
#define KEY_STUTTER_FAST	25
#define KEY_LPF				26
#define KEY_HPF				27
#define KEY_SOLO_REC		28
#define KEY_TEMPO_DOWN		29
#define KEY_VOL_DOWN		30
#define KEY_PLAY			31

#define STUTTER_MULT		2
#define STUTTER_FAST_MULT	4

#define PERSIST 80

#define VOLUME_OFFSET 16
#define VOL_MAX 1.0
#define VOL_MIN 0.2
#define VOL_INC 0.1

#define TEMPO_OFFSET 24
#define TEMPO_HOLD_INC 2

#define FILTER_OFF -1
#define FILTER_LPF 0
#define FILTER_HPF 1

#define OVERLAY_VOICE_OFF 	0x00000F
#define OVERLAY_VOICE_ON 	0x0000FF

#define OVERLAY_SEQ_OFF		0x000F00
#define OVERLAY_SEQ_ON		0x00FF00

#define OVERLAY_FX_OFF		0x050500
#define OVERLAY_FX_ON		0x7F7F00

#define OVERLAY_SOLO_OFF	0x050005
#define OVERLAY_SOLO_ON		0x7F007F

#define OVERLAY_TEMPO_OFF	0x0F0400

#define OVERLAY_VOL_OFF   	0x000F0F

#define OVERLAY_REC_ON		0xFF0000

#define OVERLAY_PLAY_ON		0x7F7F7F

class Controls {
public:
	Controls() {
		_volShowCount = 0;
		_tempoShowCount = 0;
		_tempoHoldCount = 0;
		_vol = 1.0;
		_crushOn = false;
	}
	~Controls() {}

	void begin();
	void run();
	void volumeOverlay();
	void tempoOverlay(bool inc);
	void stateNormal();
	void stateRecording();
	void recOverlay();
private:
	void readAccelBitcrush();
	void readAccelFilter();

	uint32_t _overlay[NUM_KEYS];
	uint32_t _overlayVol[8];
	uint32_t _overlayTempo[8];
	int _volShowCount;
	int _tempoShowCount;
	int _tempoHoldCount;
	bool _crushOn;
	float _vol;
};


#endif /* CONTROLS_H_ */
