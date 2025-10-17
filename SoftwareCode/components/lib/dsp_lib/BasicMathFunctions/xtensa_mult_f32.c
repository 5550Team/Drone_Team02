
#include "xtensa_math.h"

void xtensa_mult_f32(
  float32_t * pSrcA,
  float32_t * pSrcB,
  float32_t * pDst,
  uint32_t blockSize)
{
  uint32_t blkCnt;                               /* loop counters */

  /* Initialize blkCnt with number of samples */
  blkCnt = blockSize;


  while (blkCnt > 0U)
  {
    /* C = A * B */
    /* Multiply the inputs and store the results in output buffer */
    *pDst++ = (*pSrcA++) * (*pSrcB++);

    /* Decrement the blockSize loop counter */
    blkCnt--;
  }
}

/**
 * @} end of BasicMult group
 */
