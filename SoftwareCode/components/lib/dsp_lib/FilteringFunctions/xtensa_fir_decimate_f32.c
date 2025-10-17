#include "xtensa_math.h"


void xtensa_fir_decimate_f32(
  const xtensa_fir_decimate_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize)
{
  float32_t *pState = S->pState;                 /* State pointer */
  float32_t *pCoeffs = S->pCoeffs;               /* Coefficient pointer */
  float32_t *pStateCurnt;                        /* Points to the current sample of the state */
  float32_t *px, *pb;                            /* Temporary pointers for state and coefficient buffers */
  float32_t sum0;                                /* Accumulator */
  float32_t x0, c0;                              /* Temporary variables to hold state and coefficient values */
  uint32_t numTaps = S->numTaps;                 /* Number of filter coefficients in the filter */
  uint32_t i, tapCnt, blkCnt, outBlockSize = blockSize / S->M;  /* Loop counters */



  /* S->pState buffer contains previous frame (numTaps - 1) samples */
  /* pStateCurnt points to the location where the new input data should be written */
  pStateCurnt = S->pState + (numTaps - 1U);

  /* Total number of output samples to be computed */
  blkCnt = outBlockSize;

  while (blkCnt > 0U)
  {
    /* Copy decimation factor number of new input samples into the state buffer */
    i = S->M;

    do
    {
      *pStateCurnt++ = *pSrc++;

    } while (--i);

    /* Set accumulator to zero */
    sum0 = 0.0f;

    /* Initialize state pointer */
    px = pState;

    /* Initialize coeff pointer */
    pb = pCoeffs;

    tapCnt = numTaps;

    while (tapCnt > 0U)
    {
      /* Read coefficients */
      c0 = *pb++;

      /* Fetch 1 state variable */
      x0 = *px++;

      /* Perform the multiply-accumulate */
      sum0 += x0 * c0;

      /* Decrement the loop counter */
      tapCnt--;
    }

    /* Advance the state pointer by the decimation factor
     * to process the next group of decimation factor number samples */
    pState = pState + S->M;

    /* The result is in the accumulator, store in the destination buffer. */
    *pDst++ = sum0;

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* Processing is complete.
   ** Now copy the last numTaps - 1 samples to the start of the state buffer.
   ** This prepares the state buffer for the next function call. */

  /* Points to the start of the state buffer */
  pStateCurnt = S->pState;

  /* Copy numTaps number of values */
  i = (numTaps - 1U);

  /* copy data */
  while (i > 0U)
  {
    *pStateCurnt++ = *pState++;

    /* Decrement the loop counter */
    i--;
  }


}

/**
 * @} end of FIR_decimate group
 */
