#pragma once

#include "kve/kve_common.h"

#include <stddef.h>
#include <stdint.h>


#define KVE_STORAGE_IS_VALID(a) (a != SIZE_MAX)

#define KVE_STORAGE_INVALID_ADDRESS (SIZE_MAX)

#define END_TAG_LENDTH 2

typedef struct itemHeader_s {
  uint16_t full_length;
  uint8_t key_length;
} __attribute((packed)) kveItemHeader_t;

int kveStorageWriteItem(kveMemory_t *kve, size_t address, const char* key, const void* buffer, size_t length);

uint16_t kveStorageWriteHole(kveMemory_t *kve, size_t address, size_t full_length);

uint16_t kveStorageWriteEnd(kveMemory_t *kve, size_t address);

void kveStorageMoveMemory(kveMemory_t *kve, size_t sourceAddress, size_t destinationAddress, size_t length);

size_t kveStorageFindItemByKey(kveMemory_t *kve, size_t address, const char * key);

size_t kveStorageFindEnd(kveMemory_t *kve, size_t address);

size_t kveStorageFindHole(kveMemory_t *kve, size_t address);

size_t kveStorageFindNextItem(kveMemory_t *kve, size_t address);

kveItemHeader_t kveStorageGetItemInfo(kveMemory_t *kve, size_t address);

size_t kveStorageGetKeyLength(kveItemHeader_t header);

size_t kveStorageGetBufferLength(kveItemHeader_t header);

size_t kveStorageGetKey(kveMemory_t *kve, size_t address, kveItemHeader_t header, char* key, size_t maxLength);

size_t kveStorageGetBuffer(kveMemory_t *kve, size_t address, kveItemHeader_t header, void* buffer, size_t maxLength);