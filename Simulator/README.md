MaxxECU Dashboard Simulator

Overview
--------
This simulator runs the CM7 TouchGFX UI on a host PC using TouchGFX Simulator libraries and provides a simulated CM4 CAN source. It writes directly into the shared `g_maxxecu_data` struct so the CM7 Model/Presenter/View/AlarmManager code runs unmodified.

Prerequisites
-------------
- TouchGFX Simulator SDK (set `TOUCHGFX_SIM_PATH` to the SDK folder)
- CMake 3.20+
- Compiler with C++14 support
- Qt5 (optional) if you want the graphical SimControlPanel

Building
--------
Create a build directory and configure with CMake. Example:

```bash
mkdir build && cd build
cmake -DSIM_USE_QT=ON -DTOUCHGFX_SIM_PATH=/path/to/touchgfx/sim ..
cmake --build . -- -j
```

Running
-------
Run the produced `dashboard_sim` binary. The TouchGFX simulator will call the `touchgfx_simulator_setup()` and `touchgfx_simulator_loop()` hooks, which drive the simulated CAN traffic.

Using the Control Panel
-----------------------
If Qt is enabled, a `SimControlPanel` window will appear. Use the scenario dropdown to switch scenarios, set manual overrides, inject knock events, or simulate CAN disconnect.

Extending scenarios
-------------------
Edit `Simulator/SimCANDriver.cpp` to tune or add scenarios. The simulator aims to produce physically plausible, ramped and smoothed values using `sineWave()` and `ramp()` helpers.

Limitations
-----------
- The simulator bypasses HSEM and writes directly to shared memory. This is fine for host development but not representative of multi-core timing.
- UI painters (ArcGauge/FillBar) are skeletons for now; integrate CanvasWidgetRenderer for accurate rendering.

License
-------
This code is provided as-is for development and testing purposes.
