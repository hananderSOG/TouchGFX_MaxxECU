/*
 * shared_data.h
 *
 * Definitions related to the shared SRAM and semaphores between CM4/CM7.
 */

#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <stdint.h>
#include <stdbool.h>

#include "maxxecu_can.h" /* for MaxxECU_Data_t */

/* base address of D3 domain shared SRAM (see STM32H7 reference manual) */
#define SHARED_SRAM_BASE        ((uintptr_t)0x38000000u)

/* hardware semaphore used to protect access to shared_data
   pick any free semaphore number; make sure CM7 uses the same. */
#define HSEM_ID_MAXXECU         ((uint32_t)1u)

/* memory barriers for safe shared-memory synchronization */
#define SHARED_DMB()    __DMB()
#define SHARED_DSB()    __DSB()

/* Helper macros for semaphore operations.
   CM4 and CM7 can lock with HAL_HSEM_Take/Release; the model code
   should only use these macros to avoid direct HAL calls. */
#define SHARED_HSEM_TRY_TAKE()  (HAL_HSEM_Take(HSEM_ID_MAXXECU, 0) == HAL_OK)
#define SHARED_HSEM_RELEASE()   (HAL_HSEM_Release(HSEM_ID_MAXXECU, 0) == HAL_OK)

/* shared data object located in D3 RAM; actual definition should be
   in a C file linked to both cores. */
extern volatile MaxxECU_Data_t g_maxxecu_data;

#endif /* SHARED_DATA_H */
