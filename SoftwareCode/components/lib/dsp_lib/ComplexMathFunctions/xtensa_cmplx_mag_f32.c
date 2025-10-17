
#include "xtensa_math.h"



void xtensa_cmplx_mag_f32(
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t numSamples)
{
  float32_t realIn, imagIn;                      /* Temporary variables to hold input values */



  while (numSamples > 0U)
  {
    /* out = sqrt((real * real) + (imag * imag)) */
    realIn = *pSrc++;
    imagIn = *pSrc++;
    /* store the result in the destination buffer. */
    xtensa_sqrt_f32((realIn * realIn) + (imagIn * imagIn), pDst++);

    /* Decrement the loop counter */
    numSamples--;
  }


}

/**
 * @} end of cmplx_mag group
 */
