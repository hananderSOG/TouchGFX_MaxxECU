#ifdef SIMULATOR

extern "C" uint32_t HAL_HSEM_Take(uint32_t SemID, uint32_t ProcessID) {
  (void)SemID; (void)ProcessID;
  return 0; // HAL_OK
}

extern "C" void HAL_HSEM_Release(uint32_t SemID, uint32_t ProcessID) {
  (void)SemID; (void)ProcessID;
}

#endif // SIMULATOR
