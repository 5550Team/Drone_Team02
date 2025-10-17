#pragma once

#include <stdbool.h>
#include <stddef.h>

void storageInit();

bool storageTest();

bool storageStore(char* key, const void* buffer, size_t length);

size_t storageFetch(char *key, void* buffer, size_t length);

bool storageDelete(char* key);
