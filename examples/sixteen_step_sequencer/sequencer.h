/*
 * sequencer.h
 *
 *  Created on: Oct 22, 2018
 *      Author: dean
 */

#ifndef SEQUENCER_H_
#define SEQUENCER_H_

#include <Arduino.h>

#define SEQ_TC TC1
#define SEQ_IRQn TC1_IRQn
#define SEQ_Handler TC1_Handler
#define SEQ_GCLK_ID TC1_GCLK_ID

#define SEQ_TEMPO_DEFAULT 300

#define SEQ_NUM_STEPS 16

#define SEQ_VOICES_MAX 16

#define SEQ_STEP_LIGHT 0xFF0000
#define SEQ_STEP_ACTIVE_LIGHT 0x0F0000
#define SEQ_SOUND_ACTIVE 0x00000F

#define SEQ_TEMPO_MIN 100
#define SEQ_TEMPO_MAX 800

#define SEQ_TEMPO_STEP 2

class Sequencer {
public:
	Sequencer() {}
	~Sequencer() {}

	void begin();
	static void setBPM(float bpm);
	void run();
	void stop();
	bool isRunning() { return _running; }
	int getStep() { return _step; }
	uint32_t *getOverlay() { return _overlay; }
	static void runStep();
	void toggleStep(uint8_t step);
	void toggleVoice(uint8_t step);
	bool isWriting() { return _writing; }
	void toggleWriting();
	void setActiveSound(int8_t sound){
		_activeSound = sound;
		updateOverlay();
	}
	int8_t getActiveSound(){ return _activeSound; }
	void setStutter(uint8_t mult);
	void changeTempo(bool inc);
	void setStep(uint8_t step);
	void resetStep() { if(_oldStep > -1) _step = _oldStep; _oldStep = -1; }

	static bool _stutter, _stutterStopping;
	static uint16_t _tempo, _stutterMult, _tempTempo, _stutterCount;
	static int _step;
	static int _newStep, _oldStep;

private:
	static void updateOverlay();
	static int8_t _activeSound;
	static bool _running;
	static bool _writing;
	static uint16_t _activeSteps;
	static uint16_t _activeVoices[SEQ_NUM_STEPS];
	static uint32_t _overlay[SEQ_NUM_STEPS];
};


#endif /* SEQUENCER_H_ */
