#pragma once

#include <QObject>
#include <QDebug>
#include <QThread>

#include "daq.h" 

namespace dashboard
{
    class Logthread : public QObject
    {
        Q_OBJECT

    public:
        // Store the initialized manager pointer
        dashboard::CANManager* canManager = nullptr; 

    public slots:

        void startContinuousLogging()
        {
            while (true)
            {
                canManager->decode_can();   
                QThread::msleep(1);
            }
        }
        
        // Custom slot to receive the manager from the main thread
        void setCANManager(dashboard::CANManager* manager)
        {
            canManager = manager;
        }
    };
}