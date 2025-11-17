#include <QObject>
#include <QDebug>
#include <QThread>
#include "telemetry.h" 

class Logthread : public QObject
{
    Q_OBJECT

public:
    // Store the initialized manager pointer
    frucd::daq::TelemetryManager* telemManager = nullptr; 

public slots:

    void startContinuousLogging()
    {
        while (true)
        {
            telemManager->log_can();   

        }
    }
    
    // Custom slot to receive the manager from the main thread
    void setTelemetryManager(frucd::daq::TelemetryManager* manager)
    {
        telemManager = manager;
    }
};