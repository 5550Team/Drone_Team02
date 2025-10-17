#include "xtensa_math.h"


void xtensa_fir_sparse_init_f32(
  xtensa_fir_sparse_instance_f32 * S,
  uint16_t numTaps,
  float32_t * pCoeffs,
  float32_t * pState,
  int32_t * pTapDelay,
  uint16_t maxDelay,
  uint32_t blockSize)
{
  /* Assign filter taps */
  S->numTaps = numTaps;

  /* Assign coefficient pointer */
  S->pCoeffs = pCoeffs;

  /* Assign TapDelay pointer */
  S->pTapDelay = pTapDelay;

  /* Assign MaxDelay */
  S->maxDelay = maxDelay;

  /* reset the stateIndex to 0 */
  S->stateIndex = 0U;

  /* Clear state buffer and size is always maxDelay + blockSize */
  memset(pState, 0, (maxDelay + blockSize) * sizeof(float32_t));

  /* Assign state pointer */
  S->pState = pState;

}

