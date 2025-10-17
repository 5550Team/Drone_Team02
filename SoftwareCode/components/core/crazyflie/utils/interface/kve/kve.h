#pragma once

#include "kve/kve_common.h"

#include <stddef.h>
#include <stdbool.h>

void kveDefrag(kveMemory_t *kve);

bool kveStore(kveMemory_t *kve, char* key, const void* buffer, size_t length);

size_t kveFetch(kveMemory_t *kve, const char* key, void* buffer, size_t bufferLength);

bool kveDelete(kveMemory_t *kve, char* key);

void kveFormat(kveMemory_t *kve);

bool kveCheck(kveMemory_t *kve);
