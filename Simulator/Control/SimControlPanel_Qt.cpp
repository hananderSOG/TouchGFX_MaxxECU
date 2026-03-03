#ifdef SIMULATOR
#ifdef SIM_USE_QT

#include "SimControlPanel.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QString>

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

    QPushButton* knockBtn = new QPushButton("INJECT KNOCK");
    main->addWidget(knockBtn);
    connect(knockBtn, &QPushButton::clicked, this, &SimControlPanel::onInjectKnock);

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

#endif // SIM_USE_QT
#endif // SIMULATOR
