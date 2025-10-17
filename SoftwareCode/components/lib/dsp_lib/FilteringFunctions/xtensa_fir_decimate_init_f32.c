#include "xtensa_math.h"


xtensa_status xtensa_fir_decimate_init_f32(
  xtensa_fir_decimate_instance_f32 * S,
  uint16_t numTaps,
  uint8_t M,
  float32_t * pCoeffs,
  float32_t * pState,
  uint32_t blockSize)
{
  xtensa_status status;

  /* The size of the input block must be a multiple of the decimation factor */
  if ((blockSize % M) != 0U)
  {
    /* Set status as ARM_MATH_LENGTH_ERROR */
    status = XTENSA_MATH_LENGTH_ERROR;
  }
  else
  {
    /* Assign filter taps */
    S->numTaps = numTaps;

    /* Assign coefficient pointer */
    S->pCoeffs = pCoeffs;

    /* Clear state buffer and size is always (blockSize + numTaps - 1) */
    memset(pState, 0, (numTaps + (blockSize - 1U)) * sizeof(float32_t));

    /* Assign state pointer */
    S->pState = pState;

    /* Assign Decimation Factor */
    S->M = M;

    status = XTENSA_MATH_SUCCESS;
  }

  return (status);

}

/**
 * @} end of FIR_decimate group
 */
