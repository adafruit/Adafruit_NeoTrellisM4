/*
 * recorder.h
 *
 *  Created on: Oct 23, 2018
 *      Author: dean
 */

#ifndef RECORDER_H_
#define RECORDER_H_

#include "Audio.h"

#define REC_BLOCKS_PER_FILE 16
#define REC_FILESIZE (SFLASH_BLOCK_SIZE*REC_BLOCKS_PER_FILE)
#define SECTOR(x) ((int)((x)/SFLASH_SECTOR_SIZE))
#define BLOCK(x) ((int)((x)/SFLASH_BLOCK_SIZE))

class Recorder {
public:
	Recorder() {
		_currentAddr = 0;
		_totalSize = 0;
		_slot = 0;
	}
	~Recorder() {}

	void begin();

	void startRecording(uint8_t slot);
	void continueRecording();
	void stopRecording();

	static AudioRecordQueue queue1;
	static AudioInputAnalogStereo  audioInput;

private:
	uint32_t _totalSize;
	uint32_t _currentAddr;
	uint8_t _slot;
};


#endif /* RECORDER_H_ */
