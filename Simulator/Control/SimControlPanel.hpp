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

private:
    QComboBox* m_scenarioBox;
    QLineEdit* m_signalName;
    QLineEdit* m_signalValue;
    QPushButton* m_setBtn;
    QPushButton* m_clearAllBtn;
};

#else // SIM_USE_QT

// Minimal stub for non-Qt builds
class SimControlPanel {
public:
    SimControlPanel() {}
};

#endif // SIM_USE_QT

#endif // SIMULATOR
