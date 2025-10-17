#include "xtensa_math.h"


void xtensa_biquad_cascade_df1_f32(
  const xtensa_biquad_casd_df1_inst_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize)
{
  float32_t *pIn = pSrc;                         /*  source pointer            */
  float32_t *pOut = pDst;                        /*  destination pointer       */
  float32_t *pState = S->pState;                 /*  pState pointer            */
  float32_t *pCoeffs = S->pCoeffs;               /*  coefficient pointer       */
  float32_t acc;                                 /*  Simulates the accumulator */
  float32_t b0, b1, b2, a1, a2;                  /*  Filter coefficients       */
  float32_t Xn1, Xn2, Yn1, Yn2;                  /*  Filter pState variables   */
  float32_t Xn;                                  /*  temporary input           */
  uint32_t sample, stage = S->numStages;         /*  loop counters             */




  do
  {
    /* Reading the coefficients */
    b0 = *pCoeffs++;
    b1 = *pCoeffs++;
    b2 = *pCoeffs++;
    a1 = *pCoeffs++;
    a2 = *pCoeffs++;

    /* Reading the pState values */
    Xn1 = pState[0];
    Xn2 = pState[1];
    Yn1 = pState[2];
    Yn2 = pState[3];

    /*      The variables acc holds the output value that is computed:
     *    acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1]   + a2 * y[n-2]
     */

    sample = blockSize;

    while (sample > 0U)
    {
      /* Read the input */
      Xn = *pIn++;

      /* acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */
      acc = (b0 * Xn) + (b1 * Xn1) + (b2 * Xn2) + (a1 * Yn1) + (a2 * Yn2);

      /* Store the result in the accumulator in the destination buffer. */
      *pOut++ = acc;

      /* Every time after the output is computed state should be updated. */
      /* The states should be updated as:    */
      /* Xn2 = Xn1    */
      /* Xn1 = Xn     */
      /* Yn2 = Yn1    */
      /* Yn1 = acc   */
      Xn2 = Xn1;
      Xn1 = Xn;
      Yn2 = Yn1;
      Yn1 = acc;

      /* decrement the loop counter */
      sample--;
    }

    /*  Store the updated state variables back into the pState array */
    *pState++ = Xn1;
    *pState++ = Xn2;
    *pState++ = Yn1;
    *pState++ = Yn2;

    /*  The first stage goes from the input buffer to the output buffer. */
    /*  Subsequent numStages  occur in-place in the output buffer */
    pIn = pDst;

    /* Reset the output pointer */
    pOut = pDst;

    /* decrement the loop counter */
    stage--;

  } while (stage > 0U);


}
