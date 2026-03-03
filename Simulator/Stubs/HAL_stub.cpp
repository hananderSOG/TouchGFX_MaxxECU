#ifdef SIMULATOR

#include <chrono>
#include <thread>

static auto sim_start = std::chrono::steady_clock::now();

extern "C" uint32_t HAL_GetTick() {
  auto now = std::chrono::steady_clock::now();
  return (uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(now - sim_start).count();
}

extern "C" void HAL_Delay(uint32_t ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

#endif // SIMULATOR
