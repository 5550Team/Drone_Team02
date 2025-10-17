#include "xtensa_math.h"


void xtensa_biquad_cascade_df2T_init_f32(
  xtensa_biquad_cascade_df2T_instance_f32 * S,
  uint8_t numStages,
  float32_t * pCoeffs,
  float32_t * pState)
{
  /* Assign filter stages */
  S->numStages = numStages;

  /* Assign coefficient pointer */
  S->pCoeffs = pCoeffs;

  /* Clear state buffer and size is always 2 * numStages */
  memset(pState, 0, (2U * (uint32_t) numStages) * sizeof(float32_t));

  /* Assign state pointer */
  S->pState = pState;
}

/**
 * @} end of BiquadCascadeDF2T group
 */
