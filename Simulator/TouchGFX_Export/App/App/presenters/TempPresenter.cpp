#include "TempPresenter.hpp"

TempPresenter::TempPresenter(TempView &v, Model &m)
    : view(v), model(m)
{
}

TempPresenter::~TempPresenter() {}

void TempPresenter::activate()
{
}

void TempPresenter::deactivate()
{
}

void TempPresenter::tick()
{
    model.tick();
    AlarmManager::instance().evaluate(model.getSnapshot());
    const Model::DashboardSnapshot &snapshot = model.getSnapshot();

    view.setCoolantTemp(snapshot.coolant_temp);
    view.setIntakeTemp(snapshot.intake_air_temp);
    view.setOilTemp(snapshot.oil_temp);
    view.setECUTemp(snapshot.ecu_temp);
    view.setEGT(snapshot.egt, 8);
    view.setExhaustHighest(snapshot.exhaust_temp_highest);
    view.updateAlarmOverlay();
}
