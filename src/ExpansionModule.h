#ifndef EXPANSIONMODULE_H
#define EXPANSIONMODULE_H

#include "Cartridge.h"
#include "Memory.h"

#define XM_RAM_SIZE 0x20000

extern byte xm_ram[XM_RAM_SIZE];
extern bool xm_pokey_enabled;
extern bool xm_mem_enabled;
extern byte xm_reg;
extern byte xm_bank;


void xm_Reset();
byte xm_Read(word address);
void xm_Write(word address, byte data);
#endif
