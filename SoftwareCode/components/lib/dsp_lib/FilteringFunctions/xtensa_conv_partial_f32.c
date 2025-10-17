#include "xtensa_math.h"


xtensa_status xtensa_conv_partial_f32(
  float32_t * pSrcA,
  uint32_t srcALen,
  float32_t * pSrcB,
  uint32_t srcBLen,
  float32_t * pDst,
  uint32_t firstIndex,
  uint32_t numPoints)
{




  float32_t *pIn1 = pSrcA;                       /* inputA pointer */
  float32_t *pIn2 = pSrcB;                       /* inputB pointer */
  float32_t sum;                                 /* Accumulator */
  uint32_t i, j;                                 /* loop counters */
  xtensa_status status;                             /* status of Partial convolution */

  /* Check for range of output samples to be calculated */
  if ((firstIndex + numPoints) > ((srcALen + (srcBLen - 1U))))
  {
    /* Set status as XTENSA_ARGUMENT_ERROR */
    status = XTENSA_MATH_ARGUMENT_ERROR;
  }
  else
  {
    /* Loop to calculate convolution for output length number of values */
    for (i = firstIndex; i <= (firstIndex + numPoints - 1); i++)
    {
      /* Initialize sum with zero to carry on MAC operations */
      sum = 0.0f;

      /* Loop to perform MAC operations according to convolution equation */
      for (j = 0U; j <= i; j++)
      {
        /* Check the array limitations for inputs */
        if ((((i - j) < srcBLen) && (j < srcALen)))
        {
          /* z[i] += x[i-j] * y[j] */
          sum += pIn1[j] * pIn2[i - j];
        }
      }
      /* Store the output in the destination buffer */
      pDst[i] = sum;
    }
    /* set status as XTENSA_SUCCESS as there are no argument errors */
    status = XTENSA_MATH_SUCCESS;
  }
  return (status);

}

/**
 * @} end of PartialConv group
 */
