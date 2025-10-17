#include "xtensa_math.h"


void xtensa_fir_lattice_init_f32(
  xtensa_fir_lattice_instance_f32 * S,
  uint16_t numStages,
  float32_t * pCoeffs,
  float32_t * pState)
{
  /* Assign filter taps */
  S->numStages = numStages;

  /* Assign coefficient pointer */
  S->pCoeffs = pCoeffs;

  /* Clear state buffer and size is always numStages */
  memset(pState, 0, (numStages) * sizeof(float32_t));

  /* Assign state pointer */
  S->pState = pState;

}
