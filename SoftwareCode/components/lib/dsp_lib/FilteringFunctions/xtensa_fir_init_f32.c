#include "xtensa_math.h"

void xtensa_fir_init_f32(
  xtensa_fir_instance_f32 * S,
  uint16_t numTaps,
  float32_t * pCoeffs,
  float32_t * pState,
  uint32_t blockSize)
{
  /* Assign filter taps */
  S->numTaps = numTaps;

  /* Assign coefficient pointer */
  S->pCoeffs = pCoeffs;

  /* Clear state buffer and the size of state buffer is (blockSize + numTaps - 1) */
  memset(pState, 0, (numTaps + (blockSize - 1U)) * sizeof(float32_t));

  /* Assign state pointer */
  S->pState = pState;

}


