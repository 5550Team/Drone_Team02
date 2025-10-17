/**
 * pca95x4.h - Functions for interfacing PCA95X4 I2C GPIO extender
 */
#ifndef __PCA95X4_H__
#define __PCA95X4_H__

#include <stdint.h>
#include <stdbool.h>

#define PCA95X4_DEFAULT_ADDRESS 0b0100000

#define PCA95X4_INPUT_REG   (0x00)
#define PCA95X4_OUTPUT_REG  (0x01)
#define PCA95X4_POL_REG     (0x02)
#define PCA95X4_CONFIG_REG  (0x03)

#define PCA95X4_P0 (1 << 0)
#define PCA95X4_P1 (1 << 1)
#define PCA95X4_P2 (1 << 2)
#define PCA95X4_P3 (1 << 3)
#define PCA95X4_P4 (1 << 4)
#define PCA95X4_P5 (1 << 5)
#define PCA95X4_P6 (1 << 6)
#define PCA95X4_P7 (1 << 7)

/**
 * Initilize the PCA95X4 sub-system.
 */
void pca95x4Init();

/**
 * Test the PCA95X4 sub-system.
 */
bool pca95x4Test();

/**
 * Set the output register value.
 */
bool pca95x4ConfigOutput(uint32_t val);

/**
 * Set output bits.
 */
bool pca95x4SetOutput(uint32_t mask);

/**
 * Reset output bits.
 */
bool pca95x4ClearOutput(uint32_t mask);

#endif //__PCA95X4_H__
