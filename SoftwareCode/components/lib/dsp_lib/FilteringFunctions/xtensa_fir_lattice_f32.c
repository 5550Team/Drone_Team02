#include "xtensa_math.h"


void xtensa_fir_lattice_f32(
  const xtensa_fir_lattice_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize)
{
  float32_t *pState;                             /* State pointer */
  float32_t *pCoeffs = S->pCoeffs;               /* Coefficient pointer */
  float32_t *px;                                 /* temporary state pointer */
  float32_t *pk;                                 /* temporary coefficient pointer */




  float32_t fcurr, fnext, gcurr, gnext;          /* temporary variables */
  uint32_t numStages = S->numStages;             /* Length of the filter */
  uint32_t blkCnt, stageCnt;                     /* temporary variables for counts */

  pState = &S->pState[0];

  blkCnt = blockSize;

  while (blkCnt > 0U)
  {
    /* f0(n) = x(n) */
    fcurr = *pSrc++;

    /* Initialize coeff pointer */
    pk = pCoeffs;

    /* Initialize state pointer */
    px = pState;

    /* read g0(n-1) from state buffer */
    gcurr = *px;

    /* for sample 1 processing */
    /* f1(n) = f0(n) +  K1 * g0(n-1) */
    fnext = fcurr + ((*pk) * gcurr);
    /* g1(n) = f0(n) * K1  +  g0(n-1) */
    gnext = (fcurr * (*pk++)) + gcurr;

    /* save f0(n) in state buffer */
    *px++ = fcurr;

    /* f1(n) is saved in fcurr
       for next stage processing */
    fcurr = fnext;

    stageCnt = (numStages - 1U);

    /* stage loop */
    while (stageCnt > 0U)
    {
      /* read g2(n) from state buffer */
      gcurr = *px;

      /* save g1(n) in state buffer */
      *px++ = gnext;

      /* Sample processing for K2, K3.... */
      /* f2(n) = f1(n) +  K2 * g1(n-1) */
      fnext = fcurr + ((*pk) * gcurr);
      /* g2(n) = f1(n) * K2  +  g1(n-1) */
      gnext = (fcurr * (*pk++)) + gcurr;

      /* f1(n) is saved in fcurr1
         for next stage processing */
      fcurr = fnext;

      stageCnt--;

    }

    /* y(n) = fN(n) */
    *pDst++ = fcurr;

    blkCnt--;

  }
}

/**
 * @} end of FIR_Lattice group
 */
