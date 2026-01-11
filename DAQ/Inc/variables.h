#pragma once

#include <QObject>
#include <QString>
#include <string>
#include <cmath>
#include <cstdlib>
#include <iostream>


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

        Q_PROPERTY(int soc READ soc NOTIFY socChanged)

        Q_PROPERTY(int mctemp READ mctemp NOTIFY mctempChanged)
        Q_PROPERTY(QString mctemp_color READ mctemp_color NOTIFY mctemp_colorChanged)

        Q_PROPERTY(double glvv READ glvv NOTIFY glvvChanged)
        Q_PROPERTY(QString glvv_color READ glvv_color NOTIFY glvv_colorChanged)

        Q_PROPERTY(QString shutdowncircuit READ shutdowncircuit NOTIFY shutdowncircuitChanged)

        Q_PROPERTY(QString mcfault READ mcfault NOTIFY mcfaultChanged)

        Q_PROPERTY(int packtemp READ packtemp NOTIFY packtempChanged)

        Q_PROPERTY(int motortemp READ motortemp NOTIFY motortempChanged)

        Q_PROPERTY(int overtake READ overtake NOTIFY overtakeChanged)

        Q_PROPERTY(int torquelimit READ torquelimit NOTIFY torquelimitChanged)

        Q_PROPERTY(int launchcontrol READ launchcontrol NOTIFY launchcontrolChanged)

        Q_PROPERTY(int whichPopupVisible READ whichPopupVisible NOTIFY whichPopupVisibleChanged)

        Q_PROPERTY(QString dashboardpage READ dashboardpage NOTIFY dashboardpageChanged)

    public:
        // for the singleton thing
        static Variables* instance();
        

        // getter functions that the qml will call to get needed data to update dashboard with
        static int mph();

        static QString vehiclestate();
        static QString vehiclestate_color();

        static int soc();

        static int mctemp();
        static QString mctemp_color();

        static double glvv();
        static QString glvv_color();

        static QString shutdowncircuit();

        static QString mcfault();

        static int packtemp();
        
        static int motortemp();

        static int overtake();

        static int torquelimit();

        static int launchcontrol();

        static int whichPopupVisible();

        static QString dashboardpage();


        // setter functions
        static void setmph(int speed);

        // static void setvehiclestate(std::string state, int faultid);
        static void setvehiclestate(int state, bool bms);

        static void setsoc(int charge);

        static void setmctemp(int tempA, int tempB, int tempC);

        static void setglvv(double volt);

        static void setshutdowncircuit(std::string flippedswitch);

        static void setmcfault(uint32_t faultid, bool post);

        static void setpacktemp(int temp);

        static void setmotortemp(int temp);

        static void setovertake(int status);

        static void settorquelimit(int limit);

        static void setlaunchcontrol(int param);

        static void showEventPopUp(int which);
        Q_INVOKABLE static void hidePopup();

        static void setdashboardpage(int page);

    signals:
        // for all the notify functions that get emitted in the setters
        void mphChanged();

        void vehiclestateChanged();
        void vehiclestate_colorChanged();

        void socChanged();

        void mctempChanged();
        void mctemp_colorChanged();

        void glvvChanged();
        void glvv_colorChanged();

        void shutdowncircuitChanged();

        void mcfaultChanged();

        void packtempChanged();

        void motortempChanged();

        void overtakeChanged();
    
        void torquelimitChanged();

        void launchcontrolChanged();

        void whichPopupVisibleChanged();

        void dashboardpageChanged();

    private:

        // for singleton thing
        explicit Variables(QObject *parent = nullptr);

        static Variables* s_instance;


        // instance variables
        static int m_mph;

        static QString m_vehiclestate;
        static QString m_vehiclestate_color;

        static int m_soc;

        static int m_mctemp;
        static QString m_mctemp_color;

        static double m_glvv;
        static QString m_glvv_color;

        static QString m_shutdowncircuit;

        static QString m_mcfault;

        static int m_packtemp;

        static int m_motortemp;

        static int m_overtake;

        static int m_torquelimit;

        static int m_launchcontrol;

        static int m_whichPopupVisible;
        // -1 no flashscreens
        // 0 state
        // 1 knob1
        // 2 knob2
        static QString prev_vehicle_state;
        static int prev_knob1_val;
        static int prev_knob2_val;

        static QString m_dashboardpage;

        const static QString green;
        const static QString yellow;
        const static QString red;

    };
}