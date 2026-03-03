#ifdef SIMULATOR

#include "SimCANDriver.hpp"
// Optionally show a Qt control panel when building with Qt support.
#if defined(SIM_USE_QT)
#include <QApplication>
#include "Control/SimControlPanel.hpp"
static QApplication* g_qapp = nullptr;
static SimControlPanel* g_panel = nullptr;
#endif

// TouchGFX simulator calls these hooks if provided
extern "C" void touchgfx_simulator_setup()
{
    SimCANDriver::instance().init();
    SimCANDriver::instance().setScenario(SimScenario::IDLE);
#if defined(SIM_USE_QT)
    if (!g_qapp) {
        int argc = 0;
        char* argv[] = { nullptr };
        g_qapp = new QApplication(argc, argv);
        g_panel = new SimControlPanel();
        g_panel->show();
    }
#endif
}

extern "C" void touchgfx_simulator_loop(uint32_t delta_ms)
{
    SimCANDriver::instance().tick(delta_ms);
#if defined(SIM_USE_QT)
    if (g_qapp) {
        g_qapp->processEvents();
    }
#endif
}

#endif // SIMULATOR
