#include "xtensa_math.h"

void xtensa_conv_f32(
  float32_t * pSrcA,
  uint32_t srcALen,
  float32_t * pSrcB,
  uint32_t srcBLen,
  float32_t * pDst)
{




  float32_t *pIn1 = pSrcA;                       /* inputA pointer */
  float32_t *pIn2 = pSrcB;                       /* inputB pointer */
  float32_t sum;                                 /* Accumulator */
  uint32_t i, j;                                 /* loop counters */

  /* Loop to calculate convolution for output length number of times */
  for (i = 0U; i < ((srcALen + srcBLen) - 1U); i++)
  {
    /* Initialize sum with zero to carry out MAC operations */
    sum = 0.0f;

    /* Loop to perform MAC operations according to convolution equation */
    for (j = 0U; j <= i; j++)
    {
      /* Check the array limitations */
      if ((((i - j) < srcBLen) && (j < srcALen)))
      {
        /* z[i] += x[i-j] * y[j] */
        sum += pIn1[j] * pIn2[i - j];
      }
    }
    /* Store the output in the destination buffer */
    pDst[i] = sum;
  }


}

/**
 * @} end of Conv group
 */
