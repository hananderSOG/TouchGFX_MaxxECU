#include "EnginePresenter.hpp"

EnginePresenter::EnginePresenter(EngineView &v, Model &m)
    : view(v), model(m)
{
}

EnginePresenter::~EnginePresenter() {}

void EnginePresenter::activate()
{
}

void EnginePresenter::deactivate()
{
}

void EnginePresenter::tick()
{
    model.tick();
    AlarmManager::instance().evaluate(model.getSnapshot());
    const Model::DashboardSnapshot &snapshot = model.getSnapshot();

    view.setLambda(snapshot.lambda_a, snapshot.lambda_b, snapshot.lambda_avg);
    view.setBoost(snapshot.boost_target_kpa,
                  snapshot.wastegate_pressure_kpa,
                  snapshot.boost_solenoid_duty);
    view.setIgnitionTiming(snapshot.ignition_timing);
    view.setFuelPressure(snapshot.fuel_pressure_kpa);
    view.setFuelDuty(snapshot.fuel_inj_duty);
    view.setEthanolContent(snapshot.fuel_ethanol_pct);
    view.setMAP(snapshot.map_kpa);
    view.setThrottle(snapshot.throttle_pct);
    view.updateAlarmOverlay();
}
