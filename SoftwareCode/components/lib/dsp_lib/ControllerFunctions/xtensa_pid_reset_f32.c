#include "xtensa_math.h"

void xtensa_pid_reset_f32(
  xtensa_pid_instance_f32 * S)
{

  /* Clear the state buffer.  The size will be always 3 samples */
  memset(S->state, 0, 3U * sizeof(float32_t));
}

