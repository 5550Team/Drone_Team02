#include "xtensa_math.h"


void xtensa_pid_init_f32(
  xtensa_pid_instance_f32 * S,
  int32_t resetStateFlag)
{

  /* Derived coefficient A0 */
  S->A0 = S->Kp + S->Ki + S->Kd;

  /* Derived coefficient A1 */
  S->A1 = (-S->Kp) - ((float32_t) 2.0 * S->Kd);

  /* Derived coefficient A2 */
  S->A2 = S->Kd;

  /* Check whether state needs reset or not */
  if (resetStateFlag)
  {
    /* Clear the state buffer.  The size will be always 3 samples */
    memset(S->state, 0, 3U * sizeof(float32_t));
  }

}
