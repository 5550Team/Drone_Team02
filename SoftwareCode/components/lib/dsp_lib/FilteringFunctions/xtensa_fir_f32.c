#include "xtensa_math.h"

void xtensa_fir_f32(
const xtensa_fir_instance_f32 * S,
float32_t * pSrc,
float32_t * pDst,
uint32_t blockSize)
{
   float32_t *pState = S->pState;                 /* State pointer */
   float32_t *pCoeffs = S->pCoeffs;               /* Coefficient pointer */
   float32_t *pStateCurnt;                        /* Points to the current sample of the state */
   float32_t *px, *pb;                            /* Temporary pointers for state and coefficient buffers */
   uint32_t numTaps = S->numTaps;                 /* Number of filter coefficients in the filter */
   uint32_t i, tapCnt, blkCnt;                    /* Loop counters */

   /* Run the below code for Cortex-M0 */

   float32_t acc;

   /* S->pState points to state array which contains previous frame (numTaps - 1) samples */
   /* pStateCurnt points to the location where the new input data should be written */
   pStateCurnt = &(S->pState[(numTaps - 1U)]);

   /* Initialize blkCnt with blockSize */
   blkCnt = blockSize;

   while (blkCnt > 0U)
   {
      /* Copy one sample at a time into state buffer */
      *pStateCurnt++ = *pSrc++;

      /* Set the accumulator to zero */
      acc = 0.0f;

      /* Initialize state pointer */
      px = pState;

      /* Initialize Coefficient pointer */
      pb = pCoeffs;

      i = numTaps;

      /* Perform the multiply-accumulates */
      do
      {
         /* acc =  b[numTaps-1] * x[n-numTaps-1] + b[numTaps-2] * x[n-numTaps-2] + b[numTaps-3] * x[n-numTaps-3] +...+ b[0] * x[0] */
         acc += *px++ * *pb++;
         i--;

      } while (i > 0U);

      /* The result is store in the destination buffer. */
      *pDst++ = acc;

      /* Advance state pointer by 1 for the next sample */
      pState = pState + 1;

      blkCnt--;
   }

   /* Processing is complete.
   ** Now copy the last numTaps - 1 samples to the starting of the state buffer.
   ** This prepares the state buffer for the next function call. */

   /* Points to the start of the state buffer */
   pStateCurnt = S->pState;

   /* Copy numTaps number of values */
   tapCnt = numTaps - 1U;

   /* Copy data */
   while (tapCnt > 0U)
   {
      *pStateCurnt++ = *pState++;

      /* Decrement the loop counter */
      tapCnt--;
   }

}


/**
* @} end of FIR group
*/
