#pragma once

#include <QObject>
#include <QString>
#include <string>


namespace frucd::daq
{
    class Variables : public QObject {
        Q_OBJECT
        
        /* general stucture: 
        (getter function with return type) 
        READ (function name that qml will use for getter function) 
        NOTIFY (when specified function is emitted then it tells qml to call the function again)
        i think
        */
        Q_PROPERTY(int mph READ mph NOTIFY mphChanged)

        Q_PROPERTY(QString vehiclestate READ vehiclestate NOTIFY vehiclestateChanged)
        Q_PROPERTY(QString vehiclestate_color READ vehiclestate_color NOTIFY vehiclestate_colorChanged)

        // Q_PROPERTY(int whichPopupVisible READ whichPopupVisible NOTIFY whichPopupVisibleChanged)

    public:
        // for the singleton thing
        static Variables* instance();
        

        // getter functions that the qml will call to get needed data to update dashboard with
        static int mph();

        static QString vehiclestate();
        static QString vehiclestate_color();

        // int whichPopupVisible() const;


        // setter functions
        static void setmph(int speed);

        static void setvehiclestate(std::string state);

        // void showEventPopUp(int which);
        // Q_INVOKABLE void hidePopup();

    signals:
        // for all the notify functions that get emitted in the setters
        void mphChanged();

        void vehiclestateChanged();
        void vehiclestate_colorChanged();

        // void whichPopupVisibleChanged();

    private:

        // for singleton thing
        explicit Variables(QObject *parent = nullptr);

        static Variables* s_instance;


        // instance variables
        static int m_mph;

        static QString m_vehiclestate;
        static QString m_vehiclestate_color;

        // static int m_whichPopupVisible;

    };
}