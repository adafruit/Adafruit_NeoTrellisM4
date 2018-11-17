/*
 * controls.cpp
 *
 *  Created on: Oct 22, 2018
 *      Author: dean
 */

#include "controls.h"
#include "sampler.h"
#include "filters.h"
#include "sequencer.h"
#include "recorder.h"

Adafruit_NeoTrellisM4 trellis = Adafruit_NeoTrellisM4();
Adafruit_ADXL343 accel = Adafruit_ADXL343(123, &Wire1);

static const byte ROWS = 4; // four rows
static const byte COLS = 8; // eight columns

extern Sampler sampler;
extern Sequencer sequencer;
extern Recorder recorder;

extern AudioEffectBitcrusher   bitCrushL;
extern AudioEffectBitcrusher   bitCrushR;

extern AudioMixer4        volOutL;
extern AudioMixer4        volOutR;

void Controls::begin()
{
  Serial.println("Control initialization");
	trellis.begin();
	trellis.setBrightness(255);
  trellis.autoUpdateNeoPixels(false);
  trellis.show();

  Serial.println("Trellis started");
  
	memset(_overlay, 0, sizeof(uint32_t)*NUM_KEYS);

	_overlay[KEY_SOUND] = OVERLAY_VOICE_OFF;
	_overlay[KEY_WRITE] = OVERLAY_VOICE_OFF;

	_overlay[KEY_JUMP] = OVERLAY_SEQ_OFF;
	_overlay[KEY_STUTTER] = OVERLAY_SEQ_OFF;
	_overlay[KEY_STUTTER_FAST] = OVERLAY_SEQ_OFF;

	_overlay[KEY_LPF] = OVERLAY_FX_OFF;
	_overlay[KEY_HPF] = OVERLAY_FX_OFF;
	_overlay[KEY_CRUSH] = OVERLAY_FX_OFF;

	_overlay[KEY_SOLO_REC] = OVERLAY_SOLO_OFF;
	_overlay[KEY_SOLO_KIT] = OVERLAY_SOLO_OFF;

	_overlay[KEY_TEMPO_UP] = OVERLAY_TEMPO_OFF;
	_overlay[KEY_TEMPO_DOWN] = OVERLAY_TEMPO_OFF;

	_overlay[KEY_VOL_UP] = OVERLAY_VOL_OFF;
	_overlay[KEY_VOL_DOWN] = OVERLAY_VOL_OFF;

  /* Initialise the sensor */
  if(!accel.begin()) {
	  /* There was a problem detecting the ADXL343 ... check your connections */
	  Serial.println("Ooops, no ADXL343 detected");
	  while(1);
  } else {
    Serial.println("ADXL343 detected");
  }

  accel.setRange(ADXL343_RANGE_16_G);
  // displaySetRange(ADXL343_RANGE_8_G);
  // displaySetRange(ADXL343_RANGE_4_G);
  // displaySetRange(ADXL343_RANGE_2_G);
}

void Controls::volumeOverlay()
{
	memset(_overlayVol, 0, sizeof(uint32_t)*COLS);
	int ticks = (int)((_vol - VOL_MIN)*10);
	for(int i=0; i<COLS; i++){
		if(i <= ticks) _overlayVol[i] = 0x00FF00;
	}
}

void Controls::tempoOverlay(bool inc)
{
	if(!_tempoHoldCount){
		sequencer.changeTempo(inc);
		_tempoHoldCount = TEMPO_HOLD_INC;

		memset(_overlayTempo, 0, sizeof(uint32_t)*COLS);
		int div = (SEQ_TEMPO_MAX / COLS);
		int mod = sequencer._tempo%div;
		int base = sequencer._tempo/div;
		int remainder = map(mod, 0, div, 0, 255);
		for(int i=0; i<COLS; i++){
			if(i == base) _overlayTempo[i] = remainder << 8;
			else if(i < base) _overlayTempo[i] = 0x00FF00;
		}
	}
	else _tempoHoldCount--;

	_tempoShowCount = PERSIST;
}

void Controls::stateNormal()
{
	_overlay[KEY_REC] = 0;

	while(trellis.available())
	{
	keypadEvent e = trellis.read();
	int keyindex = e.bit.KEY;
	if(e.bit.EVENT == KEY_JUST_PRESSED){

	  if(!trellis.isPressed(KEY_JUMP)){
		  if(e.bit.KEY <= SEQ_NUM_STEPS && !trellis.isPressed(KEY_SOUND)
				  && !sequencer.isWriting()){
			  sequencer.toggleStep(keyindex);
		  }
		  else if(e.bit.KEY <= SEQ_NUM_STEPS && sequencer.isWriting() && !trellis.isPressed(KEY_SOUND)){
			  sequencer.toggleVoice(keyindex);
		  }
	  }

	  switch(e.bit.KEY){
		case(KEY_BUILTIN1):
		case(KEY_BUILTIN2):
		case(KEY_BUILTIN3):
		case(KEY_BUILTIN4):
		case(KEY_BUILTIN5):
		case(KEY_BUILTIN6):
		case(KEY_BUILTIN7):
		case(KEY_BUILTIN8):
		case(KEY_SAMPLE1):
		case(KEY_SAMPLE2):
		case(KEY_SAMPLE3):
		case(KEY_SAMPLE4):
		case(KEY_SAMPLE5):
		case(KEY_SAMPLE6):
		case(KEY_SAMPLE7):
		case(KEY_SAMPLE8):
		  if(trellis.isPressed(KEY_SOUND)){
			  _overlay[keyindex] = OVERLAY_VOICE_ON;
			  sampler.playSound(keyindex);
			  sequencer.setActiveSound(keyindex);
		  }
		  else if(trellis.isPressed(KEY_JUMP)){
			  sequencer.setStep(keyindex);
		  }
		  break;

		case(KEY_LPF):
			if(filterActive != FILTER_LPF){
				if(filterActive == FILTER_HPF)
					_overlay[KEY_HPF] = OVERLAY_FX_OFF;

				_overlay[keyindex] = OVERLAY_FX_ON;
				myFilterL.begin(fir_list[FILTER_LPF].coeffs, fir_list[FILTER_LPF].num_coeffs);
				myFilterR.begin(fir_list[FILTER_LPF].coeffs, fir_list[FILTER_LPF].num_coeffs);
				filterActive = FILTER_LPF;
			}
			else{
				_overlay[keyindex] = OVERLAY_FX_OFF;
				filterActive = FILTER_OFF;
		        myFilterL.begin(FIR_PASSTHRU, 0);
		        myFilterR.begin(FIR_PASSTHRU, 0);
			}
		  break;
		case(KEY_HPF):
			if(filterActive != FILTER_HPF){
				if(filterActive == FILTER_LPF)
					_overlay[KEY_LPF] = OVERLAY_FX_OFF;

				_overlay[keyindex] = OVERLAY_FX_ON;
				myFilterL.begin(fir_list[FILTER_HPF].coeffs, fir_list[FILTER_HPF].num_coeffs);
				myFilterR.begin(fir_list[FILTER_HPF].coeffs, fir_list[FILTER_HPF].num_coeffs);
				filterActive = FILTER_HPF;
			}
			else{
				_overlay[keyindex] = OVERLAY_FX_OFF;
				filterActive = FILTER_OFF;
		        myFilterL.begin(FIR_PASSTHRU, 0);
		        myFilterR.begin(FIR_PASSTHRU, 0);
			}
		  break;
		case(KEY_CRUSH):
				if(_crushOn){
					_crushOn = false;
					_overlay[keyindex] = OVERLAY_FX_OFF;
					bitCrushL.bits(16);
					bitCrushL.sampleRate(44100);
					bitCrushR.bits(16);
					bitCrushR.sampleRate(44100);
				}
				else{
					_crushOn = true;
					_overlay[keyindex] = OVERLAY_FX_ON;
					bitCrushL.bits(8);
					bitCrushR.bits(8);

				}
		  break;
		case(KEY_SOLO_REC):
			if(sampler.getSolo() == SOLO_REC){
				// turn it off
				sampler.setSolo(SOLO_NONE);
				_overlay[keyindex] = OVERLAY_SOLO_OFF;
			}
			else {
				if(sampler.getSolo() == SOLO_KIT)
					_overlay[KEY_SOLO_KIT] = OVERLAY_SOLO_OFF;
				_overlay[keyindex] = OVERLAY_SOLO_ON;
				sampler.setSolo(SOLO_REC);
			}
			break;
		case(KEY_SOLO_KIT):
			if(sampler.getSolo() == SOLO_KIT){
				// turn it off
				sampler.setSolo(SOLO_NONE);
				_overlay[keyindex] = OVERLAY_SOLO_OFF;
			}
			else {
				if(sampler.getSolo() == SOLO_REC)
					_overlay[KEY_SOLO_REC] = OVERLAY_SOLO_OFF;
				_overlay[keyindex] = OVERLAY_SOLO_ON;
				sampler.setSolo(SOLO_KIT);
			}
			break;
		case(KEY_PLAY):
			  if(sequencer.isRunning()){
				  // stop the sequencer
				  sequencer.stop();
				  _overlay[keyindex] = 0;
			  }
			  else{
				  // start the sequencer
				  sequencer.run();
				  _overlay[keyindex] = OVERLAY_PLAY_ON;
			  }
		  break;
		case(KEY_WRITE):
			sequencer.toggleWriting();
		  break;
		case(KEY_STUTTER):
			sequencer.setStutter(STUTTER_MULT);
			_overlay[keyindex] = OVERLAY_SEQ_ON;
			_overlay[KEY_STUTTER_FAST] = OVERLAY_SEQ_OFF;
		  break;
		case(KEY_STUTTER_FAST):
			sequencer.setStutter(STUTTER_FAST_MULT);
			_overlay[keyindex] = OVERLAY_SEQ_ON;
			_overlay[KEY_STUTTER] = OVERLAY_SEQ_OFF;
		  break;
		case(KEY_JUMP):
			_overlay[keyindex] = OVERLAY_SEQ_ON;
		  break;
		case(KEY_VOL_UP):
			_volShowCount = PERSIST;
			if(_vol < VOL_MAX) _vol += VOL_INC;
			volOutL.gain(0, _vol);
			volOutR.gain(0, _vol);
			volumeOverlay();
			break;
		case(KEY_VOL_DOWN):
			_volShowCount = PERSIST;
			if(_vol > VOL_MIN) _vol -= VOL_INC;
			volOutL.gain(0, _vol);
			volOutR.gain(0, _vol);
			volumeOverlay();
			break;
		default:
		  break;
	  }
	}
	else if(e.bit.EVENT == KEY_JUST_RELEASED){
		switch(e.bit.KEY){
			case(KEY_BUILTIN1):
			case(KEY_BUILTIN2):
			case(KEY_BUILTIN3):
			case(KEY_BUILTIN4):
			case(KEY_BUILTIN5):
			case(KEY_BUILTIN6):
			case(KEY_BUILTIN7):
			case(KEY_BUILTIN8):
			case(KEY_SAMPLE1):
			case(KEY_SAMPLE2):
			case(KEY_SAMPLE3):
			case(KEY_SAMPLE4):
			case(KEY_SAMPLE5):
			case(KEY_SAMPLE6):
			case(KEY_SAMPLE7):
			case(KEY_SAMPLE8):
			  _overlay[keyindex] = 0;
			  break;
			case(KEY_STUTTER):
			  if(!trellis.isPressed(KEY_STUTTER_FAST))
				  sequencer.setStutter(0);
			  else{
				  sequencer.setStutter(STUTTER_FAST_MULT);
				  _overlay[KEY_STUTTER_FAST] = OVERLAY_SEQ_ON;
			  }
			  _overlay[keyindex] = OVERLAY_SEQ_OFF;
			  break;
			case(KEY_STUTTER_FAST):
			  if(!trellis.isPressed(KEY_STUTTER))
				  sequencer.setStutter(0);
			  else{
				  sequencer.setStutter(STUTTER_MULT);
				  _overlay[KEY_STUTTER] = OVERLAY_SEQ_ON;
			  }
			  _overlay[keyindex] = OVERLAY_SEQ_OFF;
			  break;
			case(KEY_JUMP):
			  _overlay[keyindex] = OVERLAY_SEQ_OFF;
			  sequencer.resetStep();
			  break;
		}
	}
	}

	// crush accel
	if(_crushOn){
		readAccelBitcrush();
	}

	if(filterActive != FILTER_OFF){
		readAccelFilter();
	}

	// tempo changing
	if(trellis.isPressed(KEY_TEMPO_UP))
		tempoOverlay(true);

	else if(trellis.isPressed(KEY_TEMPO_DOWN))
		tempoOverlay(false);

	// do our overlay
	if(trellis.isPressed(KEY_SOUND)) _overlay[KEY_SOUND] = 0x0000FF;
	else _overlay[KEY_SOUND] = OVERLAY_VOICE_OFF;

	if(sequencer.isWriting()) _overlay[KEY_WRITE] = 0x0000FF;
	else _overlay[KEY_WRITE] = OVERLAY_VOICE_OFF;

	for(int i=0; i<NUM_KEYS; i++){
		trellis.setPixelColor(i, trellis.getPixelColor(i) | _overlay[i]);
	}

	// do the sequencers overlay
	uint32_t *overlay = sequencer.getOverlay();
	for(int i=0; i<SEQ_NUM_STEPS; i++){
		trellis.setPixelColor(i, trellis.getPixelColor(i) | *overlay++);
	}

	// do volume overlay if necessary
	if(_volShowCount){
		_volShowCount--;
		for(int i=0; i<8; i++){
			trellis.setPixelColor(VOLUME_OFFSET+i, _overlayVol[i]);
		}
	}

	// do tempo overlay if necessary
	if(_tempoShowCount){
		_tempoShowCount--;
		for(int i=0; i<8; i++){
			trellis.setPixelColor(TEMPO_OFFSET+i, _overlayTempo[i]);
		}
	}
	else _tempoHoldCount = 0;
}

void Controls::recOverlay()
{
	for(int i=0; i<NUM_KEYS; i++){
		trellis.setPixelColor(i, trellis.getPixelColor(i) | _overlay[i]);
	}

	// do the sequencers overlay
	uint32_t *overlay = sequencer.getOverlay();
	for(int i=0; i<SEQ_NUM_STEPS; i++){
		trellis.setPixelColor(i, trellis.getPixelColor(i) | *overlay++);
	}
  trellis.show();
}

void Controls::stateRecording()
{
	_overlay[KEY_REC] = OVERLAY_REC_ON;

	for(int i=KEY_SAMPLE1; i<=KEY_SAMPLE8; i++){
		if(trellis.isPressed(i)){
			// light it up
			_overlay[i] = 0x0000FF;
			recOverlay();

			recorder.startRecording(i-KEY_SAMPLE1);

			while(trellis.isPressed(i)){
				trellis.tick();
				recorder.continueRecording();
			}
			_overlay[i] = 0;
			recorder.stopRecording();
			recOverlay();

			break;
		}
	}

	// discard all other events
	while(trellis.available())
	{
		trellis.read();
	}

	recOverlay();
}

void Controls::run()
{
	trellis.tick();
	trellis.fill(0);

	if(!sequencer.isRunning() && trellis.isPressed(KEY_REC))
		stateRecording();
	else
		stateNormal();

  trellis.show();
}

void Controls::readAccelFilter()
{
	/* Get a new sensor event */
	sensors_event_t event;
	accel.getEvent(&event);

	float accel = event.acceleration.y;
	//map from -7 to 7
	int amap = int(accel*1000);
	int sr;
	if(filterActive == FILTER_LPF){
		sr = map(amap, -7000, 7000, 1, 15000);
		calculateLPF(sr);
	}
	else if(filterActive == FILTER_HPF)
	{
		sr = map(amap, -7000, 7000, 1, 5000);
		calculateHPF(sr);
	}
}

void Controls::readAccelBitcrush()
{
  /* Get a new sensor event */
  sensors_event_t event;
  accel.getEvent(&event);

  float accel = event.acceleration.y;
  //map from -7 to 7
  int amap = int(accel*1000);
  int sr = map(amap, -7000, 7000, 2200, 22000);

  bitCrushL.sampleRate(sr);
  bitCrushR.sampleRate(sr);
}
