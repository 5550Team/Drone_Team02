
#include "xtensa_math.h"


void xtensa_sub_f32(
  float32_t * pSrcA,
  float32_t * pSrcB,
  float32_t * pDst,
  uint32_t blockSize)
{
  uint32_t blkCnt;                               /* loop counter */



  /* Initialize blkCnt with number of samples */
  blkCnt = blockSize;

  while (blkCnt > 0U)
  {
    /* C = A - B */
    /* Subtract and then store the results in the destination buffer. */
    *pDst++ = (*pSrcA++) - (*pSrcB++);

    /* Decrement the loop counter */
    blkCnt--;
  }
}

