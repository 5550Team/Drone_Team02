

#include "xtensa_math.h"


void xtensa_cmplx_conj_f32(
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t numSamples)
{
  uint32_t blkCnt;                               /* loop counter */


  blkCnt = numSamples;


  while (blkCnt > 0U)
  {
    /* realOut + j (imagOut) = realIn + j (-1) imagIn */
    /* Calculate Complex Conjugate and then store the results in the destination buffer. */
    *pDst++ = *pSrc++;
    *pDst++ = -*pSrc++;

    /* Decrement the loop counter */
    blkCnt--;
  }
}

/**
 * @} end of cmplx_conj group
 */
