#include "variables.h"

#include <algorithm>

namespace frucd::daq
{
    // singleton stuff
    Variables* Variables::s_instance = nullptr;
    int Variables::m_mph = 0;

    // delcaration of instance variablesm (need defualt values)
    QString Variables::m_vehiclestate = "Startup";
    QString Variables::m_vehiclestate_color = "green";

    int Variables::m_soc =  -1;

    int Variables::m_mctemp = -1;
    QString Variables::m_mctemp_color = "green";

    double Variables::m_glvv = -1.0;
    QString Variables::m_glvv_color = "green";

    QString Variables:: m_shutdowncircuit = "temp";

    QString Variables::m_mcfault = "temp";

    int Variables::m_whichPopupVisible = -1;

    QString Variables::m_dashboardpage = "debug.qml"; 

    // singleton stuff
    Variables* Variables::instance() 
    {
        if (s_instance == nullptr) {
            s_instance = new Variables(nullptr);
        }
        return s_instance;
    }

    Variables::Variables(QObject *parent) 
        : QObject(parent) // Initialize base class
            // could initialize member variable here
    {
        // Implementation body
    }

    /* 
    * right now the structure is that we have getter and setter functions
    * the getter functions are what the qml files call to get their needed values
    * the setter functions are what we use in the code to set the values from the can stuff
    * in the setter functions we have the emit changed functions so that whenever we update a variable by setting it 
    *  the qml gets notfied and calls the getter again to update 
    */

    // defining getter classes
    int Variables::mph() { 
        return m_mph; 
    }

    QString Variables::vehiclestate() {
        return m_vehiclestate;
    }
    QString Variables::vehiclestate_color() {
        return m_vehiclestate_color;
    }

    int Variables::soc() {
        return m_soc;
    }

    int Variables::mctemp() {
        return m_mctemp;
    }
    QString Variables::mctemp_color() {
        return m_mctemp_color;
    }

    double Variables::glvv() {
        return m_glvv;
    }
    QString Variables::glvv_color() {
        return m_glvv_color;
    }

    QString Variables::shutdowncircuit() {
        return m_shutdowncircuit;
    }

    QString Variables::mcfault() {
        return m_mcfault;
    }

    int Variables::whichPopupVisible() {
        return m_whichPopupVisible;
    }

    QString Variables::dashboardpage() {
        return m_dashboardpage;
    }


    // defining setter classes
    void Variables::setmph(int speed) {
        if (m_mph != speed) { 
            m_mph = speed;
            emit Variables::instance()->mphChanged(); // Notify QML
        }
    }

    void Variables::setvehiclestate(std::string state) {
        QString qstate = QString::fromStdString(state);
        if (m_vehiclestate != qstate) {
            m_vehiclestate = qstate;
            // add vechiclestate_color here
            emit Variables::instance()->vehiclestateChanged();
        }
    }

    void Variables::setsoc(int charge) {
        if (m_soc != charge) {
            m_soc = charge;
            emit Variables::instance()->socChanged();
        }
    }

    void Variables::setmctemp(int tempA, int tempB, int tempC) {
        int max_temp = std::max({tempA, tempB, tempC});
        if (m_mctemp != max_temp) {
            m_mctemp = max_temp;
            if (m_mctemp < 45) {
                m_mctemp_color = "green";
            } else if (m_mctemp < 50) {
                m_mctemp_color = "yellow";
            } else {
                m_mctemp_color = "red";
            }
            emit Variables::instance()->mctempChanged();
        }
    }

    void Variables::setglvv(double volt) {
        if (m_glvv != volt) {
            m_glvv = volt;
            if (m_glvv > 10) {
                m_glvv_color = "green";
            } else if (m_glvv > 9) {
                m_glvv_color = "yellow";
            } else {
                m_glvv_color = "red";
            }
            emit Variables::instance()->glvvChanged();
        }
    }

    void Variables::setshutdowncircuit(std::string flippedswitch) {
        QString qswitch = QString::fromStdString(flippedswitch);
        if (m_shutdowncircuit != qswitch) {
            m_shutdowncircuit = qswitch;
            emit Variables::instance()->shutdowncircuitChanged();
        }
    }

    void Variables::setmcfault(std::string fault) {
        QString qfault = QString::fromStdString(fault);
        if (m_mcfault != qfault) {
            m_mcfault = qfault;
            emit Variables::instance()->mcfaultChanged();
        }
    }

    void Variables::showEventPopUp(int which) {
        m_whichPopupVisible = which;
        emit Variables::instance()->whichPopupVisibleChanged();
    }
    void Variables::hidePopup() {
        if (m_whichPopupVisible != -1) {
            m_whichPopupVisible = -1;
            emit Variables::instance()->whichPopupVisibleChanged();
        }
    }

    void Variables::setdashboardpage(std::string page) {
        QString qpage = QString::fromStdString(page);
        if (m_dashboardpage != qpage) {
            m_dashboardpage = qpage;
            emit Variables::instance()->dashboardpageChanged();
        }
    } 
}