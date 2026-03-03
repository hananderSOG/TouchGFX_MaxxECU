#pragma once

#ifdef SIMULATOR

#include "../SimCANDriver.hpp"

#ifdef SIM_USE_QT
#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

class SimControlPanel : public QWidget {
    Q_OBJECT
public:
    SimControlPanel(QWidget* parent = nullptr);

private slots:
    void onScenarioChanged(int idx);
    void onSetOverride();
    void onClearAll();
    void onInjectKnock();
    void onInjectLaunch();
    void onInjectRevlimit();
    void onInjectShiftcut();
    void onInjectCANDisconnect();

private:
    QComboBox* m_scenarioBox;
    QLineEdit* m_signalName;
    QLineEdit* m_signalValue;
    QPushButton* m_setBtn;
    QPushButton* m_clearAllBtn;
    // live data display
    QLabel* m_rpmLabel;
    QLabel* m_speedLabel;
    QLabel* m_coolantLabel;
    QLabel* m_oilLabel;
    QLabel* m_boostLabel;
    QLabel* m_lambdaLabel;
    QLabel* m_gearLabel;
    QLabel* m_alarmsLabel;
};

#else // SIM_USE_QT

// Minimal stub for non-Qt builds
class SimControlPanel {
public:
    SimControlPanel() {}
};

#endif // SIM_USE_QT

#endif // SIMULATOR
