#pragma once

#include <QObject>
#include <QString>
#include <string>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <chrono>


namespace dashboard
{

    enum class LapState{
        STARTING=1, 
        IN_LAP,
        FINISHING_LAP
    };

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

        Q_PROPERTY(double accum_power READ accum_power NOTIFY accum_powerChanged)

        Q_PROPERTY(int overtake READ overtake NOTIFY overtakeChanged)

        Q_PROPERTY(int torquelimit READ torquelimit NOTIFY torquelimitChanged)

        Q_PROPERTY(int launchcontrol READ launchcontrol NOTIFY launchcontrolChanged)

        Q_PROPERTY(int whichPopupVisible READ whichPopupVisible NOTIFY whichPopupVisibleChanged)

        Q_PROPERTY(QString dashboardpage READ dashboardpage NOTIFY dashboardpageChanged)
/* LAPTIME TO ADD
        Q_PROPERTY(double curr_lap_time READ curr_lap_time NOTIFY curr_lap_timeChanged)

        Q_PROPERTY(double best_lap_time READ best_lap_time NOTIFY best_lap_timeChanged)
        
        Q_PROPERTY(double prev_lap_time READ prev_lap_time NOTIFY prev_lap_timeChanged)
*/


    public:

        // set up singleton
        static Variables* instance();
        
        Variables(const Variables&) = delete;
        void operator=(const Variables&) = delete;

        // getter functions that the qml will call to get needed data to update dashboard with
        int mph();

        QString vehiclestate();
        QString vehiclestate_color();

        int soc();

        int mctemp();
        QString mctemp_color();

        double glvv();
        QString glvv_color();

        QString shutdowncircuit();

        QString mcfault();

        int packtemp();
        
        int motortemp();

        double accum_power();

        int overtake();

        int torquelimit();

        int launchcontrol();

        int whichPopupVisible();

        QString dashboardpage();
/* LAPTIME TO ADD
        double curr_lap_time();

        double best_lap_time();

        double prev_lap_time();
*/


        // setter functions
       void setmph(int speed);

       void setvehiclestate(int state, bool bms);

       void setsoc(int charge);

       void setmctemp(int tempA, int tempB, int tempC);

       void setglvv(double volt);

       void setshutdowncircuit(std::string flippedswitch);

       void setmcfault(uint32_t faultid, bool post);

       void setpacktemp(int temp);

       void setmotortemp(int temp);

       void setaccum_power(double power);

       void setovertake(int status);

       void settorquelimit(int limit);

       void setlaunchcontrol(int param);

        void showEventPopUp(int which);
        Q_INVOKABLE void hidePopup();

        void setdashboardpage(int page);

/* LAPTIME TO ADD
        // public xsens functions

        // Should be called once when driver starts practice mode (at starting line)
        void driver_practice_init();

        // Should be called regularly after start coordinates have been initialized
         void driver_practice_update();
*/



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

        void accum_powerChanged();

        void overtakeChanged();
    
        void torquelimitChanged();

        void launchcontrolChanged();

        void whichPopupVisibleChanged();

        void dashboardpageChanged();

        void curr_lap_timeChanged();
        
        void best_lap_timeChanged();

        void prev_lap_timeChanged();


    private:

        // private constructor for singleton
        explicit Variables(QObject *parent = nullptr);


        // instance variables
        // specialized colors for more visibility in dashboard
        const QString green = "#00FF00";
        const QString yellow = "#FFFF00";
        const QString red = "#ff5b1f";

        int m_mph = -1;

        QString m_vehiclestate = "STARTUP";
        QString m_vehiclestate_color = green;

        int m_soc = -1;

        int m_mctemp = -1;
        QString m_mctemp_color = "white";

        double m_glvv = -1;
        QString m_glvv_color = "white";

        QString m_shutdowncircuit = "";

        QString m_mcfault = "";

        int m_packtemp = -1;

        int m_motortemp = -1;

        double m_accum_power =  -1;

        int m_overtake = 0;

        int m_torquelimit = 0;

        int m_launchcontrol = 0;

        int m_whichPopupVisible = -1;
        // -1 no flashscreens
        // 0 state
        // 1 knob1
        // 2 knob2
        QString prev_vehicle_state = "";
        int prev_knob1_val = m_torquelimit;
        int prev_knob2_val = m_launchcontrol;

        QString m_dashboardpage = "drive.qml";

/* LAPTIME TO ADD
        // xsens variables
        LapState driver_practice_state = LapState::STARTING;

        double start_latitude = 0.0;
        double start_longitude = 0.0;

        int tolerance_feet = 5; // TODO tune tolerance for starting area, depends on xsens accuracy / noise

        double curr_latitude = 0.0;
        double curr_longitude = 0.0;

        uint64_t prev_ns = 0.0;

        uint64_t best_lap_time_ns = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(41)).count();
        uint64_t curr_lap_time_ns = 0;
        uint64_t prev_lap_time_ns = 0;

        // private xsens functions 

        // update current coordinates from the Xsens
        void update_curr_coordinates();

        // updates curr_lap_time_ns and prev_n
        void update_curr_lap_time();

        // true if car is within starting area, false otherwise
        bool within_starting_area();
*/

    };
}