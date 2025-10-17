

#include "xtensa_math.h"



void xtensa_offset_f32(
  float32_t * pSrc,
  float32_t offset,
  float32_t * pDst,
  uint32_t blockSize)
{
  uint32_t blkCnt;                               /* loop counter */


  /* Initialize blkCnt with number of samples */
  blkCnt = blockSize;

  while (blkCnt > 0U)
  {
    /* C = A + offset */
    /* Add offset and then store the result in the destination buffer. */
    *pDst++ = (*pSrc++) + offset;

    /* Decrement the loop counter */
    blkCnt--;
  }
}

/**
 * @} end of offset group
 */
