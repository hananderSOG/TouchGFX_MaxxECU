#pragma once

#include "model/Model.hpp"
#include "views/EngineView.hpp"

class EnginePresenter
{
public:
    EnginePresenter(EngineView &v, Model &m);
    virtual ~EnginePresenter();

    void activate();
    void deactivate();
    void tick();

private:
    EngineView &view;
    Model &model;
};
