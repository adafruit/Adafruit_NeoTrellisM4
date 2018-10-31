#include "Adafruit_ZeroDMA.h"
#include "wiring_private.h"

// change to 1 for A1
#define DAC_OUTPUT_PIN 1

Adafruit_ZeroDMA dma0;
DmacDescriptor *desc;
static ZeroDMAstatus    stat;

extern const uint16_t sine[44100];

#define AUDIO_TC TC2
#define AUDIO_IRQn TC2_IRQn
#define AUDIO_Handler TC2_Handler
#define AUDIO_GCLK_ID TC2_GCLK_ID
#define AUDIO_TC_FREQ 44100

#define WAIT_TC8_REGS_SYNC(x) while(x->COUNT8.SYNCBUSY.bit.ENABLE || x->COUNT8.SYNCBUSY.bit.SWRST);

void dummyCallback(Adafruit_ZeroDMA *dma)
{
  //do nothing. This is currently required for the DMA library
}

void setup() {
  stat = dma0.allocate();

  analogWrite(A0, 2048);
  analogWrite(A1, 2048);
  
  GCLK->PCHCTRL[AUDIO_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK0_Val | (1 << GCLK_PCHCTRL_CHEN_Pos);
  AUDIO_TC->COUNT8.WAVE.reg = TC_WAVE_WAVEGEN_NFRQ;
  
  AUDIO_TC->COUNT8.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  WAIT_TC8_REGS_SYNC(AUDIO_TC)

  AUDIO_TC->COUNT8.CTRLA.reg = TC_CTRLA_SWRST;
  WAIT_TC8_REGS_SYNC(AUDIO_TC)
  while (AUDIO_TC->COUNT8.CTRLA.bit.SWRST);
  
  AUDIO_TC->COUNT8.CTRLA.reg |= TC_CTRLA_MODE_COUNT8 | TC_CTRLA_PRESCALER_DIV16;
  WAIT_TC8_REGS_SYNC(AUDIO_TC)

  AUDIO_TC->COUNT8.PER.reg = (uint8_t)( (SystemCoreClock >> 4) / AUDIO_TC_FREQ);
  WAIT_TC8_REGS_SYNC(AUDIO_TC)
  
  AUDIO_TC->COUNT8.CTRLA.reg |= TC_CTRLA_ENABLE;
  WAIT_TC8_REGS_SYNC(AUDIO_TC)
  
  dma0.setTrigger(TC2_DMAC_ID_OVF);
  dma0.setAction(DMA_TRIGGER_ACTON_BEAT);
  
  desc = dma0.addDescriptor(
    (void *)sine,           // move data from here
    (void *)(&DAC->DATA[DAC_OUTPUT_PIN]),
    44100,               // this many...
    DMA_BEAT_SIZE_HWORD,               // bytes/hword/words
    true,                             // increment source addr?
    false);
  //desc.BTCTRL.bit.BLOCKACT = DMA_BLOCK_ACTION_INT;

  dma0.loop(true);
  dma0.setCallback(dummyCallback);

  dma0.startJob();
}

void loop() {
  // put your main code here, to run repeatedly:

}
