#include "xtensa_math.h"



void xtensa_biquad_cascade_df2T_f32(
const xtensa_biquad_cascade_df2T_instance_f32 * S,
float32_t * pSrc,
float32_t * pDst,
uint32_t blockSize)
{

   float32_t *pIn = pSrc;                         /*  source pointer            */
   float32_t *pOut = pDst;                        /*  destination pointer       */
   float32_t *pState = S->pState;                 /*  State pointer             */
   float32_t *pCoeffs = S->pCoeffs;               /*  coefficient pointer       */
   float32_t acc1;                                /*  accumulator               */
   float32_t b0, b1, b2, a1, a2;                  /*  Filter coefficients       */
   float32_t Xn1;                                 /*  temporary input           */
   float32_t d1, d2;                              /*  state variables           */
   uint32_t sample, stage = S->numStages;         /*  loop counters             */




   do
   {
      /* Reading the coefficients */
      b0 = *pCoeffs++;
      b1 = *pCoeffs++;
      b2 = *pCoeffs++;
      a1 = *pCoeffs++;
      a2 = *pCoeffs++;

      /*Reading the state values */
      d1 = pState[0];
      d2 = pState[1];


      sample = blockSize;

      while (sample > 0U)
      {
         /* Read the input */
         Xn1 = *pIn++;

         /* y[n] = b0 * x[n] + d1 */
         acc1 = (b0 * Xn1) + d1;

         /* Store the result in the accumulator in the destination buffer. */
         *pOut++ = acc1;

         /* Every time after the output is computed state should be updated. */
         /* d1 = b1 * x[n] + a1 * y[n] + d2 */
         d1 = ((b1 * Xn1) + (a1 * acc1)) + d2;

         /* d2 = b2 * x[n] + a2 * y[n] */
         d2 = (b2 * Xn1) + (a2 * acc1);

         /* decrement the loop counter */
         sample--;
      }

      /* Store the updated state variables back into the state array */
      *pState++ = d1;
      *pState++ = d2;

      /* The current stage input is given as the output to the next stage */
      pIn = pDst;

      /*Reset the output working pointer */
      pOut = pDst;

      /* decrement the loop counter */
      stage--;

   } while (stage > 0U);


}

/**
   * @} end of BiquadCascadeDF2T group
   */
