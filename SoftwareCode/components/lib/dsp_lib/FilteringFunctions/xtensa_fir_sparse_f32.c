#include "xtensa_math.h"

void xtensa_fir_sparse_f32(
  xtensa_fir_sparse_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  float32_t * pScratchIn,
  uint32_t blockSize)
{

  float32_t *pState = S->pState;                 /* State pointer */
  float32_t *pCoeffs = S->pCoeffs;               /* Coefficient pointer */
  float32_t *px;                                 /* Scratch buffer pointer */
  float32_t *py = pState;                        /* Temporary pointers for state buffer */
  float32_t *pb = pScratchIn;                    /* Temporary pointers for scratch buffer */
  float32_t *pOut;                               /* Destination pointer */
  int32_t *pTapDelay = S->pTapDelay;             /* Pointer to the array containing offset of the non-zero tap values. */
  uint32_t delaySize = S->maxDelay + blockSize;  /* state length */
  uint16_t numTaps = S->numTaps;                 /* Number of filter coefficients in the filter  */
  int32_t readIndex;                             /* Read index of the state buffer */
  uint32_t tapCnt, blkCnt;                       /* loop counters */
  float32_t coeff = *pCoeffs++;                  /* Read the first coefficient value */



  /* BlockSize of Input samples are copied into the state buffer */
  /* StateIndex points to the starting position to write in the state buffer */
  xtensa_circularWrite_f32((int32_t *) py, delaySize, &S->stateIndex, 1,
                        (int32_t *) pSrc, 1, blockSize);


  /* Read Index, from where the state buffer should be read, is calculated. */
  readIndex = ((int32_t) S->stateIndex - (int32_t) blockSize) - *pTapDelay++;

  /* Wraparound of readIndex */
  if (readIndex < 0)
  {
    readIndex += (int32_t) delaySize;
  }

  /* Working pointer for state buffer is updated */
  py = pState;

  /* blockSize samples are read from the state buffer */
  xtensa_circularRead_f32((int32_t *) py, delaySize, &readIndex, 1,
                       (int32_t *) pb, (int32_t *) pb, blockSize, 1,
                       blockSize);

  /* Working pointer for the scratch buffer */
  px = pb;

  /* Working pointer for destination buffer */
  pOut = pDst;




  blkCnt = blockSize;

  while (blkCnt > 0U)
  {
    /* Perform Multiplications and store in destination buffer */
    *pOut++ = *px++ * coeff;

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* Load the coefficient value and
   * increment the coefficient buffer for the next set of state values */
  coeff = *pCoeffs++;

  /* Read Index, from where the state buffer should be read, is calculated. */
  readIndex = ((int32_t) S->stateIndex - (int32_t) blockSize) - *pTapDelay++;

  /* Wraparound of readIndex */
  if (readIndex < 0)
  {
    readIndex += (int32_t) delaySize;
  }

  /* Loop over the number of taps. */
  tapCnt = (uint32_t) numTaps - 2U;

  while (tapCnt > 0U)
  {

    /* Working pointer for state buffer is updated */
    py = pState;

    /* blockSize samples are read from the state buffer */
    xtensa_circularRead_f32((int32_t *) py, delaySize, &readIndex, 1,
                         (int32_t *) pb, (int32_t *) pb, blockSize, 1,
                         blockSize);

    /* Working pointer for the scratch buffer */
    px = pb;

    /* Working pointer for destination buffer */
    pOut = pDst;

    blkCnt = blockSize;

    while (blkCnt > 0U)
    {
      /* Perform Multiply-Accumulate */
      *pOut++ += *px++ * coeff;

      /* Decrement the loop counter */
      blkCnt--;
    }

    /* Load the coefficient value and
     * increment the coefficient buffer for the next set of state values */
    coeff = *pCoeffs++;

    /* Read Index, from where the state buffer should be read, is calculated. */
    readIndex =
      ((int32_t) S->stateIndex - (int32_t) blockSize) - *pTapDelay++;

    /* Wraparound of readIndex */
    if (readIndex < 0)
    {
      readIndex += (int32_t) delaySize;
    }

    /* Decrement the tap loop counter */
    tapCnt--;
  }

	/* Compute last tap without the final read of pTapDelay */

	/* Working pointer for state buffer is updated */
	py = pState;

	/* blockSize samples are read from the state buffer */
	xtensa_circularRead_f32((int32_t *) py, delaySize, &readIndex, 1,
											 (int32_t *) pb, (int32_t *) pb, blockSize, 1,
											 blockSize);

	/* Working pointer for the scratch buffer */
	px = pb;

	/* Working pointer for destination buffer */
	pOut = pDst;

	blkCnt = blockSize;

	while (blkCnt > 0U)
	{
		/* Perform Multiply-Accumulate */
		*pOut++ += *px++ * coeff;

		/* Decrement the loop counter */
		blkCnt--;
	}


}

/**
 * @} end of FIR_Sparse group
 */
