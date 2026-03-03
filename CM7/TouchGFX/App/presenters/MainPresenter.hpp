#pragma once

#include "model/Model.hpp"
#include "views/MainView.hpp"

/**
 * Presenter for the main dashboard screen.
 */
class MainPresenter
{
public:
    MainPresenter(MainView &v, Model &m);
    virtual ~MainPresenter();

    void activate();
    void deactivate();
    void tick();

private:
    MainView &view;
    Model &model;
};
