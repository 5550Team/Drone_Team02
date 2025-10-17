#include "xtensa_math.h"


void xtensa_cmplx_mult_cmplx_f32(
  float32_t * pSrcA,
  float32_t * pSrcB,
  float32_t * pDst,
  uint32_t numSamples)
{
  float32_t a1, b1, c1, d1;                      /* Temporary variables to store real and imaginary values */
  uint32_t blkCnt;                               /* loop counters */

  blkCnt = numSamples;


  while (blkCnt > 0U)
  {
    /* C[2 * i] = A[2 * i] * B[2 * i] - A[2 * i + 1] * B[2 * i + 1].  */
    /* C[2 * i + 1] = A[2 * i] * B[2 * i + 1] + A[2 * i + 1] * B[2 * i].  */
    a1 = *pSrcA++;
    b1 = *pSrcA++;
    c1 = *pSrcB++;
    d1 = *pSrcB++;

    /* store the result in the destination buffer. */
    *pDst++ = (a1 * c1) - (b1 * d1);
    *pDst++ = (a1 * d1) + (b1 * c1);

    /* Decrement the numSamples loop counter */
    blkCnt--;
  }
}

/**
 * @} end of CmplxByCmplxMult group
 */
