/*
 * recorder.cpp
 *
 *  Created on: Oct 23, 2018
 *      Author: dean
 */

#include "recorder.h"
#include "Adafruit_SPIFlash.h"
#include <Arduino.h>
Adafruit_FlashTransport_QSPI flashTransport(PIN_QSPI_SCK, PIN_QSPI_CS,
                                            PIN_QSPI_IO0, PIN_QSPI_IO1,
                                            PIN_QSPI_IO2, PIN_QSPI_IO3);
Adafruit_SPIFlash flash(&flashTransport);

AudioInputAnalogStereo Recorder::audioInput(PIN_MIC, 0);
AudioRecordQueue Recorder::queue1;
AudioConnection patchCord1(Recorder::audioInput, 0, Recorder::queue1, 0);

void Recorder::begin() {
  if (!flash.begin()) {
    Serial.println("Could not find flash on QSPI bus!");
    __BKPT();
    while (1)
      ;
  }
}

void Recorder::startRecording(uint8_t slot) {
  // for(uint32_t i=0; i<REC_BLOCKS_PER_FILE; i++){
  //	flash.eraseBlock(BLOCK(slot*REC_FILESIZE) + i);
  //}
  flash.eraseBlock(BLOCK(slot * REC_FILESIZE));
  _currentAddr = REC_FILESIZE * slot + SFLASH_SECTOR_SIZE;
  // flash.eraseSector(SECTOR(_currentAddr));
  _totalSize = SFLASH_SECTOR_SIZE;
  _slot = slot;
  queue1.begin();
}

void Recorder::stopRecording() {
  queue1.end();
  while (queue1.available() > 0) {
    if (_totalSize < REC_FILESIZE - SFLASH_SECTOR_SIZE) {
      flash.writeBuffer(_currentAddr, (byte *)queue1.readBuffer(),
                        AUDIO_BLOCK_SAMPLES * 2);
      _currentAddr += AUDIO_BLOCK_SAMPLES * 2;
      _totalSize += AUDIO_BLOCK_SAMPLES * 2;

      // if(_totalSize%SFLASH_SECTOR_SIZE == 0)
      // flash.eraseSector(SECTOR(_currentAddr));
      if (_totalSize % SFLASH_BLOCK_SIZE == 0)
        flash.eraseBlock(BLOCK(_currentAddr));
    }
    queue1.freeBuffer();
  }
  // set the sample size
  flash.eraseSector(SECTOR(REC_FILESIZE * _slot));
  _totalSize -= SFLASH_SECTOR_SIZE;
  flash.writeBuffer((uint32_t)_slot * REC_FILESIZE, (byte *)&_totalSize,
                    sizeof(uint32_t));
}

void Recorder::continueRecording() {
  if (queue1.available() >= 1) {
    if (_totalSize < REC_FILESIZE - SFLASH_SECTOR_SIZE) {
      // write buffer to QSPI
      flash.writeBuffer(_currentAddr, (byte *)queue1.readBuffer(),
                        AUDIO_BLOCK_SAMPLES * 2);
      _currentAddr += AUDIO_BLOCK_SAMPLES * 2;
      _totalSize += AUDIO_BLOCK_SAMPLES * 2;

      // if(_totalSize%W25Q16BV_SECTORSIZE == 0)
      // flash.eraseSector(SECTOR(_currentAddr));
      if (_totalSize % SFLASH_BLOCK_SIZE == 0)
        flash.eraseBlock(BLOCK(_currentAddr));
    }

    queue1.freeBuffer();
  }
}
