#ifdef SIMULATOR
#ifdef SIM_USE_QT

#include "SimControlPanel.hpp"
#include "../Shared/Inc/shared_data.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QString>
#include <QStringList>

SimControlPanel::SimControlPanel(QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle("MaxxECU Simulator");
    setFixedSize(300,600);

    QVBoxLayout* main = new QVBoxLayout();

    QLabel* lbl = new QLabel("Scenario:");
    main->addWidget(lbl);
    m_scenarioBox = new QComboBox();
    m_scenarioBox->addItem("IDLE");
    m_scenarioBox->addItem("ACCELERATION");
    m_scenarioBox->addItem("CRUISE");
    m_scenarioBox->addItem("OVERHEATING");
    m_scenarioBox->addItem("LOW_OIL");
    m_scenarioBox->addItem("KNOCK_EVENT");
    m_scenarioBox->addItem("LAUNCH");
    m_scenarioBox->addItem("ALL_WARNINGS");
    m_scenarioBox->addItem("ALL_CRITICAL");
    m_scenarioBox->addItem("CAN_TIMEOUT");
    main->addWidget(m_scenarioBox);

    connect(m_scenarioBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onScenarioChanged(int)));

    main->addSpacing(10);
    QLabel* ovLabel = new QLabel("Manual override (name/value):");
    main->addWidget(ovLabel);
    QHBoxLayout* ovRow = new QHBoxLayout();
    m_signalName = new QLineEdit();
    m_signalName->setPlaceholderText("RPM, COOLANT, OILP, BOOST, LAMBDA");
    m_signalValue = new QLineEdit();
    m_setBtn = new QPushButton("SET");
    ovRow->addWidget(m_signalName);
    ovRow->addWidget(m_signalValue);
    ovRow->addWidget(m_setBtn);
    main->addLayout(ovRow);

    m_clearAllBtn = new QPushButton("CLEAR ALL");
    main->addWidget(m_clearAllBtn);

    connect(m_setBtn, &QPushButton::clicked, this, &SimControlPanel::onSetOverride);
    connect(m_clearAllBtn, &QPushButton::clicked, this, &SimControlPanel::onClearAll);

    // inject buttons row 1
    QHBoxLayout* injectRow1 = new QHBoxLayout();
    QPushButton* knockBtn = new QPushButton("KNOCK");
    QPushButton* launchBtn = new QPushButton("LAUNCH");
    injectRow1->addWidget(knockBtn);
    injectRow1->addWidget(launchBtn);
    main->addLayout(injectRow1);
    connect(knockBtn, &QPushButton::clicked, this, &SimControlPanel::onInjectKnock);
    connect(launchBtn, &QPushButton::clicked, this, &SimControlPanel::onInjectLaunch);

    // inject buttons row 2
    QHBoxLayout* injectRow2 = new QHBoxLayout();
    QPushButton* revBtn = new QPushButton("REVLIMIT");
    QPushButton* shiftBtn = new QPushButton("SHIFTCUT");
    injectRow2->addWidget(revBtn);
    injectRow2->addWidget(shiftBtn);
    main->addLayout(injectRow2);
    connect(revBtn, &QPushButton::clicked, this, &SimControlPanel::onInjectRevlimit);
    connect(shiftBtn, &QPushButton::clicked, this, &SimControlPanel::onInjectShiftcut);

    QPushButton* canBtn = new QPushButton("CAN DISCONNECT");
    main->addWidget(canBtn);
    connect(canBtn, &QPushButton::clicked, this, &SimControlPanel::onInjectCANDisconnect);

    main->addSpacing(10);

    // live values section
    QLabel* liveLabel = new QLabel("Live values:");
    main->addWidget(liveLabel);
    m_rpmLabel = new QLabel("RPM:    ----");
    m_speedLabel = new QLabel("Speed:  ---- km/h");
    m_coolantLabel = new QLabel("Coolant: --.- °C");
    m_oilLabel = new QLabel("Oil:     ---- kPa");
    m_boostLabel = new QLabel("Boost:   ---- kPa");
    m_lambdaLabel = new QLabel("Lambda:  ----");
    m_gearLabel = new QLabel("Gear:    -");
    main->addWidget(m_rpmLabel);
    main->addWidget(m_speedLabel);
    main->addWidget(m_coolantLabel);
    main->addWidget(m_oilLabel);
    main->addWidget(m_boostLabel);
    main->addWidget(m_lambdaLabel);
    main->addWidget(m_gearLabel);

    main->addSpacing(10);
    QLabel* alarmsHeader = new QLabel("Active alarms:");
    main->addWidget(alarmsHeader);
    m_alarmsLabel = new QLabel("(none)");
    main->addWidget(m_alarmsLabel);

    // update timer for display
    QTimer* uiTimer = new QTimer(this);
    connect(uiTimer, &QTimer::timeout, this, [this]() {
        using namespace std;
        // read shared data directly
        extern volatile MaxxECU_Data_t g_maxxecu_data;
        m_rpmLabel->setText(QString("RPM: %1").arg((int)g_maxxecu_data.EngineSpeed));
        m_speedLabel->setText(QString("Speed: %1 km/h").arg(g_maxxecu_data.VehicleSpeed, 0, 'f', 1));
        m_coolantLabel->setText(QString("Coolant: %1 °C").arg(g_maxxecu_data.CoolantTemp, 0, 'f', 1));
        m_oilLabel->setText(QString("Oil: %1 kPa").arg(g_maxxecu_data.Engine_Oil_Pressure, 0, 'f', 0));
        m_boostLabel->setText(QString("Boost: %1 kPa").arg(g_maxxecu_data.MAP, 0, 'f', 0));
        m_lambdaLabel->setText(QString("Lambda: %1").arg(g_maxxecu_data.Lambda_Average, 0, 'f', 3));
        m_gearLabel->setText(QString("Gear: %1").arg((int)g_maxxecu_data.GearPosn));
        // simple alarm list
        QStringList alarms;
        if (g_maxxecu_data.Revlimit_active) alarms << "revlimit";
        if (g_maxxecu_data.Knock_detected) alarms << "knock";
        if (g_maxxecu_data.Launch_control_active) alarms << "launch";
        if (g_maxxecu_data.Antilag_active) alarms << "antilag";
        if (g_maxxecu_data.Shiftcut_active) alarms << "shiftcut";
        // coolant warning/crit
        if (g_maxxecu_data.CoolantTemp > 105) alarms << "coolant CRIT";
        else if (g_maxxecu_data.CoolantTemp > 95) alarms << "coolant WARN";
        // oil pressure
        if (g_maxxecu_data.Engine_Oil_Pressure < 100) alarms << "oil CRIT";
        else if (g_maxxecu_data.Engine_Oil_Pressure < 150) alarms << "oil WARN";
        m_alarmsLabel->setText(alarms.isEmpty() ? "(none)" : alarms.join(", "));
    });
    uiTimer->start(250);

    setLayout(main);
}

void SimControlPanel::onScenarioChanged(int idx)
{
    SimScenario sc = static_cast<SimScenario>(idx);
    SimCANDriver::instance().setScenario(sc);
}

void SimControlPanel::onSetOverride()
{
    QString name = m_signalName->text();
    QString val = m_signalValue->text();
    if (name.isEmpty() || val.isEmpty()) return;
    float v = val.toFloat();
    SimCANDriver::instance().overrideSignal(name.toStdString().c_str(), v);
}

void SimControlPanel::onClearAll()
{
    SimCANDriver::instance().clearAllOverrides();
}

void SimControlPanel::onInjectKnock()
{
    // set knock flag for a short time via override
    SimCANDriver::instance().overrideSignal("KNOCK", 1.0f);
    // schedule clear via single-shot timer
    QTimer::singleShot(250, [](){ SimCANDriver::instance().clearOverride("KNOCK"); });
}

void SimControlPanel::onInjectLaunch()
{
    // simply switch to LAUNCH scenario for a short burst
    SimCANDriver::instance().setScenario(SimScenario::LAUNCH);
}

void SimControlPanel::onInjectRevlimit()
{
    SimCANDriver::instance().overrideSignal("REVLIMIT", 1.0f);
    QTimer::singleShot(500, [](){ SimCANDriver::instance().clearOverride("REVLIMIT"); });
}

void SimControlPanel::onInjectShiftcut()
{
    SimCANDriver::instance().overrideSignal("SHIFTCUT", 1.0f);
    QTimer::singleShot(80, [](){ SimCANDriver::instance().clearOverride("SHIFTCUT"); });
}

void SimControlPanel::onInjectCANDisconnect()
{
    // simulate ECU loss by flipping to timeout scenario
    SimCANDriver::instance().setScenario(SimScenario::CAN_TIMEOUT);
}

#endif // SIM_USE_QT
#endif // SIMULATOR
