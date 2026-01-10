#include "telemetry.h"
#include "config.h"
#include "variables.h"
#include "logthread.h"
#include "observer.h"
#include <iostream>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <QDebug>
#include <QCoreApplication> 
#include <QQmlContext>
#include <QDebug>
#include <QThread>


static QObject *variables_provider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return frucd::daq::Variables::instance();
} 

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qmlRegisterSingletonType<frucd::daq::Variables>("VariablesData", 1, 0, "Data", variables_provider);
    
    engine.load(QUrl("qrc:/Dashboard_gui/DAQ/ui/dashboard.qml"));

    try
    {
        auto cfg = frucd::daq::load_config("DAQ/config.json");
        frucd::daq::TelemetryManager telem(cfg);

        // frucd::daq::DashboardObserver dashboard(telem); 
        

        try
        {
            telem.init_can();
        }
        catch (const std::exception& e)
        {
            std::cerr << "init_can failed: " << e.what() << "\n";
            return 1;
        }

        // auto csvPath = telem.init_csv();
        // std::cout << "Logging to " << csvPath << std::endl;

        // move logging to another thread
        QThread* thread = new QThread;
        Logthread* threadprocess = new Logthread;
        threadprocess->moveToThread(thread);
        threadprocess->setTelemetryManager(&telem);

        QObject::connect(thread, &QThread::started, threadprocess, &Logthread::startContinuousLogging);

        thread->start();
        
        return app.exec();

    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }


}