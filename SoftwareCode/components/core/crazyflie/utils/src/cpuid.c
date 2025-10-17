#include "cpuid.h"

unsigned char * cpuidGetId()
{
  return (unsigned char *)(0x1FFFF7E8);
}

int cpuIdGetFlashSize()
{
  return *((short*)(4*1024*1024));
}
