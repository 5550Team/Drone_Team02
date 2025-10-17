#include "xtensa_math.h"


xtensa_status xtensa_fir_interpolate_init_f32(
  xtensa_fir_interpolate_instance_f32 * S,
  uint8_t L,
  uint16_t numTaps,
  float32_t * pCoeffs,
  float32_t * pState,
  uint32_t blockSize)
{
  xtensa_status status;

  /* The filter length must be a multiple of the interpolation factor */
  if ((numTaps % L) != 0U)
  {
    /* Set status as XTENSA_MATH_LENGTH_ERROR */
    status = XTENSA_MATH_LENGTH_ERROR;
  }
  else
  {

    /* Assign coefficient pointer */
    S->pCoeffs = pCoeffs;

    /* Assign Interpolation factor */
    S->L = L;

    /* Assign polyPhaseLength */
    S->phaseLength = numTaps / L;

    /* Clear state buffer and size of state array is always phaseLength + blockSize - 1 */
    memset(pState, 0,
           (blockSize +
            ((uint32_t) S->phaseLength - 1U)) * sizeof(float32_t));

    /* Assign state pointer */
    S->pState = pState;

    status = XTENSA_MATH_SUCCESS;
  }

  return (status);

}

 /**
  * @} end of FIR_Interpolate group
  */
