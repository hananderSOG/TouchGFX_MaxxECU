cmake_minimum_required(VERSION 3.20)
project(MaxxECU_Dashboard_Simulator)

set(CMAKE_CXX_STANDARD 14)

# TouchGFX simulator path (set by user)
set(TOUCHGFX_SIM_PATH "" CACHE PATH "Path to TouchGFX simulator libraries")

option(SIM_USE_QT "Use Qt5 for SimControlPanel" ON)

add_definitions(-DSIMULATOR -DTOUCH_GFX_SIMULATOR)

# Simulator sources
set(SIM_SOURCES
  SimulatorMain.cpp
  SimCANDriver.cpp
  ../CM7/TouchGFX/App/model/Model.cpp
  ../CM7/TouchGFX/App/presenters/MainPresenter.cpp
  ../CM7/TouchGFX/App/presenters/EnginePresenter.cpp
  ../CM7/TouchGFX/App/presenters/TempPresenter.cpp
  ../CM7/TouchGFX/App/views/MainView.cpp
  ../CM7/TouchGFX/App/views/EngineView.cpp
  ../CM7/TouchGFX/App/views/TempView.cpp
  ../CM7/TouchGFX/App/alarm/AlarmManager.cpp
  ../CM7/TouchGFX/App/alarm/AlarmOverlay.cpp
  ../CM7/TouchGFX/App/widgets/ArcGauge.cpp
  ../CM7/TouchGFX/App/widgets/FillBar.cpp
  SimCANDriver.cpp
)

# Stub out CM4 hardware files
set(SIM_STUBS
  Stubs/HAL_stub.cpp
  Stubs/HSEM_stub.cpp
)

if(SIM_USE_QT)
  find_package(Qt5 COMPONENTS Widgets REQUIRED)
  list(APPEND SIM_SOURCES Control/SimControlPanel_Qt.cpp)
  set(SIM_LIBS Qt5::Widgets)
else()
  list(APPEND SIM_SOURCES Control/SimControlPanel_Win32.cpp)
  set(SIM_LIBS "")
endif()

add_executable(dashboard_sim
  ${SIM_SOURCES}
  ${SIM_STUBS}
)

target_include_directories(dashboard_sim PRIVATE
  ../CM7/TouchGFX/App
  ../CM4/Core/Inc
  ../Shared/Inc
  ${TOUCHGFX_SIM_PATH}/include
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(dashboard_sim
  ${TOUCHGFX_SIM_PATH}/lib/libtouchgfx-sim.a
  ${SIM_LIBS}
)
