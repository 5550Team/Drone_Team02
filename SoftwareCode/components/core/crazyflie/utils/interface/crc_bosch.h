#ifndef _crc_h
#define _crc_h

#include <inttypes.h>

#define FALSE   0
#define TRUE    !FALSE

/* select the CRC standard from the list that follows */
#define CRC32

/* CRC standard list */
#if defined(CRC8)

typedef uint8_t crc;

#define CRC_NAME                "CRC-8"
#define POLYNOMIAL              0x07
#define CHECK_VALUE             0x04
#define INITIAL_REMAINDER       0x00
#define FINAL_XOR_VALUE         0x00
#define REFLECT                 0

#elif defined(CRC16C)

typedef unsigned short crc;

#define CRC_NAME                "CRC-16/CCITT-FALSE"
#define POLYNOMIAL              0x1021
#define CHECK_VALUE             0x29B1
#define INITIAL_REMAINDER       0xFFFF
#define FINAL_XOR_VALUE         0x0000
#define REFLECT                 0

#elif defined(CRC16A)

typedef unsigned short crc;

#define CRC_NAME                "CRC-16/ARC"
#define POLYNOMIAL              0xA001
#define CHECK_VALUE             0xBB3D
#define INITIAL_REMAINDER       0x0000
#define FINAL_XOR_VALUE         0x0000
#define REFLECT                 1

#elif defined(CRC16B)

typedef unsigned short crc;

#define CRC_NAME                "CRC-16/BUYPASS"
#define POLYNOMIAL              0x8005
#define CHECK_VALUE             0xFEE8
#define INITIAL_REMAINDER       0x0000
#define FINAL_XOR_VALUE         0x0000
#define REFLECT                 0

#elif defined(CRC32)

typedef unsigned long crc;

#define CRC_NAME                "CRC-32"
#define POLYNOMIAL              0xEDB88320
#define CHECK_VALUE             0xCBF43926
#define INITIAL_REMAINDER       0xFFFFFFFF
#define FINAL_XOR_VALUE         0xFFFFFFFF
#define RESIDUE                 0xDEBB20e3
#define REFLECT                 1

#elif defined(CRC32B)

typedef unsigned long crc;

#define CRC_NAME                "CRC-32/BZIP2"
#define POLYNOMIAL              0x04C11DB7
#define CHECK_VALUE             0xFC891918
#define INITIAL_REMAINDER       0xFFFFFFFF
#define FINAL_XOR_VALUE         0xFFFFFFFF
#define RESIDUE                 0xC704DD7B
#define REFLECT                 0

#else

#error "one of CRC standard has to be #define'd."

#endif

#define WIDTH           (8 * sizeof(crc))
#define TOPBIT         (1 << (WIDTH - 1))

void crcTableInit(crc* crcTable);
crc crcByBit(const uint8_t* message, uint32_t bytesToProcess,
             crc remainder, crc finalxor);
crc crcByByte(const uint8_t* message, uint32_t bytesToProcess,
              crc remainder, crc finalxor, crc* crcTable);

#endif /* _crc_h */
