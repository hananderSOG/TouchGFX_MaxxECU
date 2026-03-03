#ifdef SIMULATOR

// In the PC simulator we don't actually have separate cores or physical
// shared memory - the global variables are simply regular heap storage.
// This stub exists so that linker references resolve when CM7 code
// attempts to call shared memory helpers. All functions are no-ops.

#include <cstdint>

extern "C" void SharedMemory_Init(void) {}
extern "C" void SharedMemory_Lock(void) {}
extern "C" void SharedMemory_Unlock(void) {}

#endif // SIMULATOR
