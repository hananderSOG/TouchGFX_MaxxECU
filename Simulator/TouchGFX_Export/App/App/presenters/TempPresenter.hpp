#pragma once

#include "model/Model.hpp"
#include "views/TempView.hpp"

class TempPresenter
{
public:
    TempPresenter(TempView &v, Model &m);
    virtual ~TempPresenter();

    void activate();
    void deactivate();
    void tick();

private:
    TempView &view;
    Model &model;
};
