#include <iostream>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <QFont>
#include <QDebug>
#include <QCoreApplication> 
#include <QQmlContext>
#include <QDebug>
#include <QThread>

#include "daq.h"
#include "variables.h"
#include "logthread.h"

static QObject *variables_provider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return dashboard::Variables::instance();
} 

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QFont font("Helvetica", 10);
    font.setItalic(false);
    app.setFont(font);

    QQmlApplicationEngine engine;

    qmlRegisterSingletonType<dashboard::Variables>("VariablesData", 1, 0, "Data", variables_provider);

    engine.load(QUrl("qrc:/Dashboard_gui/ui/dashboard.qml"));

    try
    {
        dashboard::CANManager telem;

        QThread* thread = new QThread;
        dashboard::Logthread* threadprocess = new dashboard::Logthread;
        threadprocess->moveToThread(thread);
        threadprocess->setCANManager(&telem);

        QObject::connect(thread, &QThread::started, threadprocess, &dashboard::Logthread::startContinuousLogging);

        thread->start();
        
        return app.exec();

    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }


}