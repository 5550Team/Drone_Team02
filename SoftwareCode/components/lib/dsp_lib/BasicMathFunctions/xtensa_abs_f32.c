

#include "xtensa_math.h"
#include <math.h>


void xtensa_abs_f32(
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize)
{
  uint32_t blkCnt;                               /* loop counter */


  /* Initialize blkCnt with number of samples */
  blkCnt = blockSize;

  while (blkCnt > 0U)
  {
    /* C = |A| */
    /* Calculate absolute and then store the results in the destination buffer. */
    *pDst++ = fabsf(*pSrc++);

    /* Decrement the loop counter */
    blkCnt--;
  }
}

/**
 * @} end of BasicAbs group
 */
