

#include "crc_bosch.h"

/* bit-wise crc calculation */
crc crcByBit(const uint8_t* message, uint32_t bytesToProcess,
             crc remainder, crc finalxor)
{
  for (unsigned int byte = 0; byte < bytesToProcess; ++byte)
    {
#if REFLECT
      remainder ^= *(message+byte);
#else
      remainder ^= ( *(message+byte) << (WIDTH - 8) );
#endif

      for(uint8_t bit = 8; bit > 0; --bit)
        {
#if REFLECT
          /* reflect is realized by mirroring algorithm
           * LSB is first to be processed */
          if (remainder & 1)
            remainder = (remainder >> 1) ^ POLYNOMIAL;
          else
            remainder = (remainder >> 1);
#else
          /* MSB is first to be processed */
          if (remainder & TOPBIT)
            remainder = (remainder << 1) ^ POLYNOMIAL;
          else
            remainder = (remainder << 1);
#endif
        }
    }
  return (remainder ^ finalxor);
}

crc crcByByte(const uint8_t* message, uint32_t bytesToProcess,
              crc remainder, crc finalxor, crc* crcTable)
{
  static uint8_t data;
  for (int byte = 0; byte < bytesToProcess; ++byte)
    {
#if REFLECT
      data = (*(message+byte) ^ remainder);
      remainder = *(crcTable+data) ^ (remainder >> 8);
#else
      data = ( *(message+byte) ^ (remainder >> (WIDTH - 8)) );
      remainder = *(crcTable+data) ^ (remainder << 8);
#endif
    }
  return (remainder ^ finalxor);
}

/* creates a lookup-table which is necessary for the crcByByte function */
void crcTableInit(crc* crcTable)
{
  uint8_t dividend = ~0;
  /* fill the table by bit-wise calculations of checksums
   * for each possible dividend */
  do {
      *(crcTable+dividend) = crcByBit(&dividend, 1, 0, 0);
  } while(dividend-- > 0);
}
