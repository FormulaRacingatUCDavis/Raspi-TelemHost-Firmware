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
#include <mosquitto.h>
#include <string>

#include "daq.h"
#include "variables.h"
#include "logthread.h"

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message);

static QObject *variables_provider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return dashboard::Variables::instance();
} 

int main(int argc, char *argv[])
{
    try
    {
        // setting up dashboard
        QGuiApplication app(argc, argv);

        QFont font("Helvetica", 10);
        font.setItalic(false);
        app.setFont(font);

        qmlRegisterSingletonType<dashboard::Variables>("VariablesData", 1, 0, "Data", variables_provider);

        QQmlApplicationEngine engine;
        engine.load(QUrl("qrc:/Dashboard_gui/ui/dashboard.qml"));

        dashboard::CANManager telem;

        // subscribe to logger indicator
        mosquitto_lib_init();
        struct mosquitto* mosq = mosquitto_new("logger-subscriber", true, nullptr);
        int rc = mosquitto_connect(mosq, "localhost", 1883, 60);
        if (rc != MOSQ_ERR_SUCCESS)
        {
            std::cerr << "MQTT connect failed: " << mosquitto_strerror(rc) << std::endl;
            return 1;
        }
        mosquitto_message_callback_set(mosq, on_message);
        mosquitto_subscribe(mosq, nullptr, "can/log/status", 0);
        mosquitto_loop_start(mosq);

        // create separate thread for getting data from CAN to not get blocked by dashboard
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

void on_message(struct mosquitto* mosq, void* userdata, const struct mosquitto_message* message)
{
    std::string payload(static_cast<char*>(message->payload), message->payloadlen);

    Variables* variable_instance = dashboard::Variables::instance();

    if (payload == "{\"status\":\"off\"}") { // make sure this matches whatever log_status is in services/daq/src/main.cpp
        variable_instance->setloggerindicator(false);

    } else if (payload == "{\"status\":\"on\"}") {
        variable_instance->setloggerindicator(true);
        
    }
}