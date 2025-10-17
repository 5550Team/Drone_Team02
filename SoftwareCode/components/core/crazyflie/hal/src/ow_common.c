#include "ow.h"
#include "mem.h"

static bool handleMemGetSerialNr(const uint8_t selectedMem, uint8_t* serialNr);
static bool handleMemRead(const uint8_t selectedMem, const uint32_t memAddr, const uint8_t readLen, uint8_t* startOfData);
static bool handleMemWrite(const uint8_t selectedMem, const uint32_t memAddr, const uint8_t writeLen, const uint8_t* startOfData);

static MemoryOwHandlerDef_t memHandlerDef = {
  .nrOfMems = 0,
  .size = OW_MAX_SIZE,
  .getSerialNr = handleMemGetSerialNr,
  .read = handleMemRead,
  .write = handleMemWrite,
};

static bool isInit = false;

void owCommonInit() {
  if (isInit) {
    return;
  }

  owScan(&memHandlerDef.nrOfMems);
  memoryRegisterOwHandler(&memHandlerDef);

  isInit = true;
}

bool owCommonTest() {
  return isInit;
}

static bool handleMemGetSerialNr(const uint8_t selectedMem, uint8_t* serialNr) {
  return owGetinfo(selectedMem, (OwSerialNum*)serialNr);
}

static bool handleMemRead(const uint8_t selectedMem, const uint32_t memAddr, const uint8_t readLen, uint8_t* startOfData) {
  bool result = false;

  if (memAddr + readLen <= OW_MAX_SIZE) {
    if (owRead(selectedMem, memAddr, readLen, startOfData)) {
      result = true;
    }
  }

  return result;
}

static bool handleMemWrite(const uint8_t selectedMem, const uint32_t memAddr, const uint8_t writeLen, const uint8_t* startOfData) {
  bool result = false;

  if (memAddr + writeLen <= OW_MAX_SIZE) {
    if (owWrite(selectedMem, memAddr, writeLen, startOfData)) {
      result = true;
    }
  }

  return result;
}
