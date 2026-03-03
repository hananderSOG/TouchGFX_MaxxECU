#include "shared_data.h"

/* place the shared data in D3 domain SRAM; the linker script should ensure
   this address, but we can also force a section if desired. */

volatile MaxxECU_Data_t g_maxxecu_data __attribute__((section(".D3_SRAM")));
