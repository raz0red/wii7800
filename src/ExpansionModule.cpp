/*
 Memory map:

 POKEY1            $0450    $045F     16 bytes
 POKEY2*           $0460    $046F     16 bytes
 XCTRL             $0470    $047F     1 byte
 RAM               $4000    $7FFF     16384 bytes

XCTRL Bit Description

+-------------------------------+
| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
+-------------------------------+
  |   |   |   |   |   |   |   |
  |   |   |   |   |   |   |   +-- Bank select bit 0 \
  |   |   |   |   |   |   +------ Bank select bit 1  | Totally 128 KByte in 16 KByte banks
  |   |   |   |   |   +---------- Bank select bit 3 /
  |   |   |   |   +-------------- Enable memory bit (1 = Memory enabled, 0 after power on)
  |   |   |   +------------------ Enable POKEY bit** (1 = POKEY enabled, 0 after power on)
  |   |   |
 NA  NA  NA = Not Available or Not Used

* = Can be mounted piggy back on the first POKEY. Description how to do this will come when i have tried it out.
** This bit controls both POKEY chip select signals.


The mapping is totally non compatible with pretty much everything. 
There is a bank select latch located at $0470 and the POKEY is located at $0450 
(There's also a chip select output ($0460) on the PLD which alows you to simply piggy back a second POKEY). 
Since the PLD is reconfigurable I could map the POKEY (or the RAM for that matter) to pretty much anything 
if you wanted to. However since the PLD is soldered under the POKEY this needs to be configured before delivery.
*/

#include <string.h>
#include "ExpansionModule.h"

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"
#endif

#define RAM_SIZE 0x20000
byte xm_ram[RAM_SIZE] = {0};
byte xm_reg = 0;
byte xm_bank = 0;
bool xm_pokey_enabled = false;
bool xm_mem_enabled = false;

void xm_Reset() {
  memset(xm_ram, 0, RAM_SIZE);
  xm_bank = 0;
  xm_reg = 0;
  xm_mem_enabled = false;
  xm_pokey_enabled = false;
}

byte xm_Read(word address) {
    if (xm_pokey_enabled && (address >= 0x0450 && address < 0x0460)) {
        byte b = pokey_GetRegister(0x4000 + (address - 0x0450));
#if 0        
        net_print_string(NULL, 0, "Read from Pokey1: %x %d\n", address, b);
#endif        
        return b;
    } else if (xm_pokey_enabled && (address >= 0x0460 && address < 0x0470)) {
        byte b = pokey_GetRegister(0x4000 + (address - 0x0460));
#if 0        
        net_print_string(NULL, 0, "Read from Pokey2: %x %d\n", address, b);
#endif        
        return b;
    } else if (xm_mem_enabled && (address >= 0x4000 && address < 0x8000)) {
        byte b = xm_ram[(xm_bank * 0x4000) + (address - 0x4000)];
#if 0        
        net_print_string(NULL, 0, "Read from XM RAM: %x %d\n", address, b);
#endif        
        return b;
    } else if (address >= 0x0470 && address < 0x0480) {
#if 0        
        net_print_string(NULL, 0, "Read from XCTRL 0x0470: %x\n", address);
#endif        
        // TODO: Should the value be returned?
    }
    return 0;
}

void xm_Write(word address, byte data) {
    if (xm_pokey_enabled && (address >= 0x0450 && address < 0x0460)) {
#if 0        
        net_print_string(NULL, 0, "Wrote to Pokey1: %x %d\n", address, data);
#endif        
        pokey_SetRegister(0x4000 + (address - 0x0450), data);
    } else if (xm_pokey_enabled && (address >= 0x0460 && address < 0x0470)) {
#if 0        
        net_print_string(NULL, 0, "Wrote to Pokey2: %x %d\n", address, data);
#endif        
        pokey_SetRegister(0x4000 + (address - 0x0460), data);
    } else if (xm_mem_enabled && (address >= 0x4000 && address < 0x8000)) {
#if 0        
        net_print_string(NULL, 0, "Wrote to XM RAM: %x %d\n", address, data);
#endif        
        xm_ram[(xm_bank * 0x4000) + (address - 0x4000)] = data;
    } else if (address >= 0x0470 && address < 0x0480) {
#ifdef WII_NETTRACE            
        net_print_string(NULL, 0, "Wrote to XCTRL 0x0470: %x %d\n", address,
                         data);
#endif                         
        xm_reg = data;
        xm_bank = xm_reg & 7;
        xm_pokey_enabled = (xm_reg & 0x10);
        xm_mem_enabled = (xm_reg & 0x08);
#ifdef WII_NETTRACE            
        net_print_string(NULL, 0, "xm_reg: %d, xm_bank: %d, xm_pokey_enabled: %d, xm_mem_enabled: %d\n", 
            xm_reg, xm_bank, xm_pokey_enabled, xm_mem_enabled);
#endif                         

    } 
}
