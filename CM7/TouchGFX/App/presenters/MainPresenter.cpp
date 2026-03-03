#include "MainPresenter.hpp"

MainPresenter::MainPresenter(MainView &v, Model &m)
    : view(v), model(m)
{
}

MainPresenter::~MainPresenter() {}

void MainPresenter::activate()
{
    // nothing special for now
}

void MainPresenter::deactivate()
{
    // nothing special for now
}

void MainPresenter::tick()
{
    model.tick();
    // evaluate alarms based on freshest snapshot
    AlarmManager::instance().evaluate(model.getSnapshot());
    const Model::DashboardSnapshot &snapshot = model.getSnapshot();

    view.setRPM(snapshot.rpm);
    view.setSpeed(snapshot.vehicle_speed);
    view.setGear(snapshot.gear);
    view.setCoolantTemp(snapshot.coolant_temp);
    view.setOilPressure(snapshot.oil_pressure_kpa);
    view.setWarnings(snapshot.revlimit_active,
                     snapshot.knock_detected,
                     snapshot.launch_control_active,
                     snapshot.antilag_active,
                     snapshot.shiftcut_active);
    view.setCANTimeout(snapshot.can_timeout);
    // update overlay after pushing values
    view.updateAlarmOverlay();
}
