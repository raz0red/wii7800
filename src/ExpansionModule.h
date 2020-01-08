#ifndef EXPANSIONMODULE_H
#define EXPANSIONMODULE_H

#include "Cartridge.h"
#include "Memory.h"

extern bool xm_pokey_enabled;
extern bool xm_mem_enabled;

void xm_Reset();
byte xm_Read(word address);
void xm_Write(word address, byte data);
#endif
