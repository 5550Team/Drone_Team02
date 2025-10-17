#include <stdbool.h>
#include <stdint.h>

#include "ootx_decoder.h"


uint16_t betole(uint16_t value)
{
  return ((value&0xff00u)>>8) | ((value&0xffu)<<8);
}

bool ootxDecoderProcessBit(ootxDecoderState_t * state, int data)
{ 
  data &= 1;

  // Synchronization finder
  if (state->nZeros == 17 && data == 1) {
    state->synchronized = true;
    state->bitInWord = 0;
    state->wordReceived = 0;
    state->rxState = rxLength;
    // DEBUG_PRINT("Synchronized!\n");
    return false;
  }
  if (data == 0) {
    state->nZeros += 1;
  } else {
    state->nZeros = 0;
  }

  if (state->synchronized) {
    // Detect the stuffing bit
    if (state->bitInWord == 16) {
      // If the stuffing bit is ==0 -> framing error
      if (data == 0) {
        // DEBUG_PRINT("Unsynchronized!\n");
        state->synchronized = false;
        return false;
      }
      state->bitInWord = 0;

      // At the stuffing bit after CRC1, we are done!
      // TODO: Check CRC!
      if (state->rxState == rxDone) {
        state->synchronized = false;
        return true;
      } else {
        return false;
      }
    }

    state->currentWord = (state->currentWord<<1) | data;
    state->bitInWord += 1;

    // One word received
    if (state->bitInWord == 16) {
      switch (state->rxState) {
        case rxLength:
          state->frameLength = betole(state->currentWord);
          // DEBUG_PRINT("Length %0d\n", state->frameLength);
          if (state->frameLength > OOTX_MAX_FRAME_LENGTH) {
            state->synchronized = false;
            return false;
          }
          state->rxState = rxData;
          break;
        case rxData:
          // DEBUG_PRINT("data[%d]\n", state->wordReceived);
          state->data[state->wordReceived] = betole(state->currentWord);
          state->wordReceived += 1;
          if (2*state->wordReceived >= state->frameLength) {
            state->rxState = rxCrc0;
          }
          break;
        case rxCrc0:
          // DEBUG_PRINT("CRC0\n");
          state->crc32 = betole(state->currentWord);
          state->rxState = rxCrc1;
          break;
        case rxCrc1:
          // DEBUG_PRINT("CRC1\n");
          state->crc32 |= (betole(state->currentWord)<<16ul);
          state->rxState = rxDone;
          break;
        case rxDone: break;
      }
    }
  }
  return false;
}
