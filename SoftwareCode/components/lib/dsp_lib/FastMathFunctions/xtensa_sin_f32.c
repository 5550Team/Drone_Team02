

#include "xtensa_math.h"
#include "xtensa_common_tables.h"
#include <math.h>

float32_t xtensa_sin_f32(
  float32_t x)
{
  float32_t sinVal, fract, in;                           /* Temporary variables for input, output */
  uint16_t index;                                        /* Index variable */
  float32_t a, b;                                        /* Two nearest output values */
  int32_t n;
  float32_t findex;

  /* Special case for small negative inputs */
  if ((x < 0.0f) && (x >= -1.9e-7f)) {
     return x;
  }

  /* input x is in radians */
  /* Scale the input to [0 1] range from [0 2*PI] , divide input by 2*pi */
  in = x * 0.159154943092f;

  /* Calculation of floor value of input */
  n = (int32_t) in;

  /* Make negative values towards -infinity */
  if (x < 0.0f)
  {
    n--;
  }

  /* Map input value to [0 1] */
  in = in - (float32_t) n;

  /* Calculation of index of the table */
  findex = (float32_t) FAST_MATH_TABLE_SIZE * in;

  index = ((uint16_t)findex) & 0x1ff;

  /* fractional value calculation */
  fract = findex - (float32_t) index;

  /* Read two nearest values of input value from the sin table */
  a = sinTable_f32[index];
  b = sinTable_f32[index+1];

  /* Linear interpolation process */
  sinVal = (1.0f-fract)*a + fract*b;

  /* Return the output value */
  return (sinVal);
}

/**
 * @} end of sin group
 */
