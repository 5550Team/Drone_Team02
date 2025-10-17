#include "xtensa_math.h"


void xtensa_biquad_cascade_stereo_df2T_f32(
const xtensa_biquad_cascade_stereo_df2T_instance_f32 * S,
float32_t * pSrc,
float32_t * pDst,
uint32_t blockSize)
{

    float32_t *pIn = pSrc;                         /*  source pointer            */
    float32_t *pOut = pDst;                        /*  destination pointer       */
    float32_t *pState = S->pState;                 /*  State pointer             */
    float32_t *pCoeffs = S->pCoeffs;               /*  coefficient pointer       */
    float32_t acc1a, acc1b;                        /*  accumulator               */
    float32_t b0, b1, b2, a1, a2;                  /*  Filter coefficients       */
    float32_t Xn1a, Xn1b;                          /*  temporary input           */
    float32_t d1a, d2a, d1b, d2b;                  /*  state variables           */
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
        d1a = pState[0];
        d2a = pState[1];
        d1b = pState[2];
        d2b = pState[3];


        sample = blockSize;

        while (sample > 0U)
        {
            /* Read the input */
            Xn1a = *pIn++; //Channel a
            Xn1b = *pIn++; //Channel b

            /* y[n] = b0 * x[n] + d1 */
            acc1a = (b0 * Xn1a) + d1a;
            acc1b = (b0 * Xn1b) + d1b;

            /* Store the result in the accumulator in the destination buffer. */
            *pOut++ = acc1a;
            *pOut++ = acc1b;

            /* Every time after the output is computed state should be updated. */
            /* d1 = b1 * x[n] + a1 * y[n] + d2 */
            d1a = ((b1 * Xn1a) + (a1 * acc1a)) + d2a;
            d1b = ((b1 * Xn1b) + (a1 * acc1b)) + d2b;

            /* d2 = b2 * x[n] + a2 * y[n] */
            d2a = (b2 * Xn1a) + (a2 * acc1a);
            d2b = (b2 * Xn1b) + (a2 * acc1b);

            /* decrement the loop counter */
            sample--;
        }

        /* Store the updated state variables back into the state array */
        *pState++ = d1a;
        *pState++ = d2a;
        *pState++ = d1b;
        *pState++ = d2b;

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
