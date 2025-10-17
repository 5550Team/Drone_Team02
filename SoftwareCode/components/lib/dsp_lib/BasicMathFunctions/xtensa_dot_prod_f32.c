

#include "xtensa_math.h"


void xtensa_dot_prod_f32(
  float32_t * pSrcA,
  float32_t * pSrcB,
  uint32_t blockSize,
  float32_t * result)
{
  float32_t sum = 0.0f;                          /* Temporary result storage */
  uint32_t blkCnt;                               /* loop counter */




  /* Initialize blkCnt with number of samples */
  blkCnt = blockSize;



  while (blkCnt > 0U)
  {
    /* C = A[0]* B[0] + A[1]* B[1] + A[2]* B[2] + .....+ A[blockSize-1]* B[blockSize-1] */
    /* Calculate dot product and then store the result in a temporary buffer. */
    sum += (*pSrcA++) * (*pSrcB++);

    /* Decrement the loop counter */
    blkCnt--;
  }
  /* Store the result back in the destination buffer */
  *result = sum;
}

/**
 * @} end of dot_prod group
 */
