

#include "xtensa_math.h"


void xtensa_negate_f32(
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize)
{
  uint32_t blkCnt;                               /* loop counter */




  /* Initialize blkCnt with number of samples */
  blkCnt = blockSize;

  while (blkCnt > 0U)
  {
    /* C = -A */
    /* Negate and then store the results in the destination buffer. */
    *pDst++ = -*pSrc++;

    /* Decrement the loop counter */
    blkCnt--;
  }
}

/**
 * @} end of negate group
 */
