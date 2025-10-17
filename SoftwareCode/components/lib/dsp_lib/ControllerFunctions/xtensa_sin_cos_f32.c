#include "xtensa_math.h"
#include "xtensa_common_tables.h"


void xtensa_sin_cos_f32(
                      float32_t theta,
                      float32_t * pSinVal,
                      float32_t * pCosVal)
{
    float32_t fract, in;                             /* Temporary variables for input, output */
    uint16_t indexS, indexC;                         /* Index variable */
    float32_t f1, f2, d1, d2;                        /* Two nearest output values */
    float32_t findex, Dn, Df, temp;

    /* input x is in degrees */
    /* Scale the input, divide input by 360, for cosine add 0.25 (pi/2) to read sine table */
    in = theta * 0.00277777777778f;

    if (in < 0.0f)
    {
        in = -in;
    }

    in = in - (int32_t)in;

    /* Calculation of index of the table */
    findex = (float32_t) FAST_MATH_TABLE_SIZE * in;
    indexS = ((uint16_t)findex) & 0x1ff;
    indexC = (indexS + (FAST_MATH_TABLE_SIZE / 4)) & 0x1ff;

    /* fractional value calculation */
    fract = findex - (float32_t) indexS;

    /* Read two nearest values of input value from the cos & sin tables */
    f1 = sinTable_f32[indexC+0];
    f2 = sinTable_f32[indexC+1];
    d1 = -sinTable_f32[indexS+0];
    d2 = -sinTable_f32[indexS+1];

    temp = (1.0f - fract) * f1 + fract * f2;

    Dn = 0.0122718463030f; // delta between the two points (fixed), in this case 2*pi/FAST_MATH_TABLE_SIZE
    Df = f2 - f1;          // delta between the values of the functions

    temp = Dn *(d1 + d2) - 2 * Df;
    temp = fract * temp + (3 * Df - (d2 + 2 * d1) * Dn);
    temp = fract * temp + d1 * Dn;

    /* Calculation of cosine value */
    *pCosVal = fract * temp + f1;

    /* Read two nearest values of input value from the cos & sin tables */
    f1 = sinTable_f32[indexS+0];
    f2 = sinTable_f32[indexS+1];
    d1 = sinTable_f32[indexC+0];
    d2 = sinTable_f32[indexC+1];

    temp = (1.0f - fract) * f1 + fract * f2;

    Df = f2 - f1; // delta between the values of the functions
    temp = Dn*(d1 + d2) - 2*Df;
    temp = fract*temp + (3*Df - (d2 + 2*d1)*Dn);
    temp = fract*temp + d1*Dn;

    /* Calculation of sine value */
    *pSinVal = fract*temp + f1;

    if (theta < 0.0f)
    {
        *pSinVal = -*pSinVal;
    }
}
/**
 * @} end of SinCos group
 */
