/*
 * Sequencer.cpp
 *
 *  Created on: Oct 22, 2018
 *      Author: dean
 */

#include "sequencer.h"
#include "Arduino.h"
#include "sampler.h"

#define SEC_PER_MIN 60
#define WAIT_TC16_REGS_SYNC(x) while(x->COUNT16.SYNCBUSY.bit.ENABLE);

int Sequencer::_step = 0;
uint32_t Sequencer::_overlay[SEQ_NUM_STEPS];
uint16_t Sequencer::_activeSteps = 0xFFFF;
uint16_t Sequencer::_activeVoices[SEQ_NUM_STEPS];
int8_t Sequencer::_activeSound = -1;
bool Sequencer::_writing = false;
bool Sequencer::_running = false;

bool Sequencer::_stutter = false;
uint16_t Sequencer::_tempo = SEQ_TEMPO_DEFAULT;
uint16_t Sequencer::_stutterMult = 0;
uint16_t Sequencer::_tempTempo = SEQ_TEMPO_DEFAULT;
uint16_t Sequencer::_stutterCount = 0;
bool Sequencer::_stutterStopping = false;

int Sequencer::_newStep = -1;
int Sequencer::_oldStep = -1;

extern Sampler sampler;

static inline void resetTC (Tc* TCx)
{
	// Disable TCx
	TCx->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
	WAIT_TC16_REGS_SYNC(TCx)

	// Reset TCx
	TCx->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
	WAIT_TC16_REGS_SYNC(TCx)
	while (TCx->COUNT16.CTRLA.bit.SWRST);

}

static inline void acknowledgeInterrupt()
{
	if (SEQ_TC->COUNT16.INTFLAG.bit.MC0 == 1) {
		SEQ_TC->COUNT16.INTFLAG.bit.MC0 = 1;
	}
}

static inline void enableTimer()
{
	SEQ_TC->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
	WAIT_TC16_REGS_SYNC(SEQ_TC)

}
static inline void disableTimer()
{
	SEQ_TC->COUNT16.CTRLA.bit.ENABLE = 0;
	WAIT_TC16_REGS_SYNC(SEQ_TC)

}

void Sequencer::run()
{
	_stutterMult = 0;
	_stutterStopping = false;
	_stutterCount = 0;
	_stutter = false;
	enableTimer();
	_running = true;
}

void Sequencer::stop()
{
	disableTimer();
	_running = false;
	_step = 0;
	updateOverlay();
}

void Sequencer::setBPM(float bpm)
{
	disableTimer();

	SEQ_TC->COUNT16.CC[0].reg = (uint16_t)( (48000000UL/1024) / (bpm/SEC_PER_MIN));
	WAIT_TC16_REGS_SYNC(SEQ_TC);

	if(_running)
		enableTimer();
}

void Sequencer::begin()
{
	updateOverlay();
	NVIC_DisableIRQ(SEQ_IRQn);
	NVIC_ClearPendingIRQ(SEQ_IRQn);
	NVIC_SetPriority(SEQ_IRQn, 0);

	MCLK->APBAMASK.reg |= MCLK_APBAMASK_TC1;

	// use 12Mhz clock
	GCLK->PCHCTRL[SEQ_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK1 | (1 << GCLK_PCHCTRL_CHEN_Pos);

	resetTC(SEQ_TC);

	SEQ_TC->COUNT16.WAVE.reg = TC_WAVE_WAVEGEN_MFRQ;  // Set TONE_TC mode as match frequency

	SEQ_TC->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16 | TC_CTRLA_PRESCALER_DIV1024;
	WAIT_TC16_REGS_SYNC(SEQ_TC)

	SEQ_TC->COUNT16.CC[0].reg = (uint16_t)( (48000000UL/1024) / (_tempo/SEC_PER_MIN));
	WAIT_TC16_REGS_SYNC(SEQ_TC)

	// Enable the interrupt request
	SEQ_TC->COUNT16.INTENSET.bit.MC0 = 1;

	NVIC_EnableIRQ(SEQ_IRQn);
}

void Sequencer::toggleStep(uint8_t step)
{
	if(step > 15) return;
	_activeSteps ^= (1UL << step);
	updateOverlay();
}

void Sequencer::toggleVoice(uint8_t step)
{
	if(step > 15 || _activeSound > 15) return;
	_activeVoices[step] ^= (1UL << _activeSound);
	updateOverlay();
}

void Sequencer::toggleWriting()
{
	_writing = !_writing;
	updateOverlay();
}

void Sequencer::setStutter(uint8_t mult)
{
	if(mult)
		_stutterMult = mult;
	else _stutterStopping = true;
}

void Sequencer::changeTempo(bool inc)
{
	if(!_stutter){
		// don't change during a stutter and constrain to min and max vals
		if(inc) _tempTempo+=1;
		else _tempTempo-=1;

		_tempTempo = min(max(_tempTempo, SEQ_TEMPO_MIN), SEQ_TEMPO_MAX);
	}
	if(!_running){
		_tempo = Sequencer::_tempTempo;
		setBPM(_tempo);
	}
}

void Sequencer::setStep(uint8_t step)
{
	_newStep = step;
}

void Sequencer::updateOverlay()
{
	memset(_overlay, 0, sizeof(uint32_t)*SEQ_NUM_STEPS);
	if(_writing){
		for(int i=0; i<SEQ_NUM_STEPS; i++){
			// light it up if it's active on this voice
			if(_activeVoices[i] & (1UL << _activeSound))
				_overlay[i] = SEQ_STEP_ACTIVE_LIGHT;
		}
		_overlay[_activeSound] |= SEQ_SOUND_ACTIVE;
	}
	else{
		for(int i=0; i<SEQ_NUM_STEPS; i++){
			// light it up if it's active
			if(_activeSteps & (1UL << i))
				_overlay[i] = SEQ_STEP_ACTIVE_LIGHT;
		}
	}

	if( (_activeSteps & (1UL << _step)) && _running){
		_overlay[_step] = SEQ_STEP_LIGHT;
	}
}

void Sequencer::runStep()
{
	if(_activeSteps & (1UL << _step)){
		for(int i=0; i<SEQ_VOICES_MAX; i++){
			if(_activeVoices[_step] & (1UL << i)){
				// play all sounds in this step
				sampler.playSound(i);
			}
		}
	}
	updateOverlay();

	if(!_stutter){
		_step = (_step + 1)%SEQ_NUM_STEPS;
		if(_oldStep > -1) _oldStep = (_oldStep + 1)%SEQ_NUM_STEPS;
	}
	else
		_stutterCount++;
}

// this gets called when the timer fires
void SEQ_Handler()
{
	if(Sequencer::_newStep > -1){
		if(Sequencer::_oldStep == -1) Sequencer::_oldStep = Sequencer::_step;
		Sequencer::_step = Sequencer::_newStep;
		Sequencer::_newStep = -1;
	}
	if(Sequencer::_stutterMult && Sequencer::_tempTempo != Sequencer::_tempo * Sequencer::_stutterMult){
		// a stutter has been requested on this step
		Sequencer::_stutter = true;
		Sequencer::_tempTempo = Sequencer::_tempo * Sequencer::_stutterMult;
		Sequencer::setBPM(Sequencer::_tempTempo);
		Sequencer::_stutterCount = 0;
	}
	else if(Sequencer::_stutterStopping && Sequencer::_tempTempo != Sequencer::_tempo
			&& Sequencer::_stutterCount%Sequencer::_stutterMult == 0){
		// the stutter has ended
		Sequencer::_stutter = false;
		Sequencer::_stutterStopping = false;
		Sequencer::_tempTempo = Sequencer::_tempo;
		Sequencer::setBPM(Sequencer::_tempo);
		// advance to where we should be
		Sequencer::_step = (Sequencer::_step + Sequencer::_stutterCount/Sequencer::_stutterMult)%SEQ_NUM_STEPS;
		Sequencer::_stutterMult = 0;
	}
	else if(!Sequencer::_stutter && Sequencer::_tempo != Sequencer::_tempTempo){
		Sequencer::_tempo = Sequencer::_tempTempo;
		Sequencer::setBPM(Sequencer::_tempo);
	}

	Sequencer::runStep();

	acknowledgeInterrupt();
}
