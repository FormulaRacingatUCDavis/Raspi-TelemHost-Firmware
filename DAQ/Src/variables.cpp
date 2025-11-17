#include "variables.h"


namespace frucd::daq
{
    // singleton stuff
    Variables* Variables::s_instance = nullptr;
    int Variables::m_mph = 0;

    // delcaration of instance variables
    QString Variables::m_vehiclestate = "Startup";
    QString Variables::m_vehiclestate_color = "green";

    // int Variables::m_whichPopupVisible = -1;

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

    // int Variables::whichPopupVisible() const {
    //     return m_whichPopupVisible;
    // }


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

    // void Variables::showEventPopUp(int which) {
    //     if (m_whichPopupVisible != which) {
    //         m_whichPopupVisible = which;
    //         emit whichPopupVisibleChanged();
    //         qDebug() << "Popup shown";
    //     }
    // }

    // void Variables::hidePopup() {
    //     if (m_whichPopupVisible != -1) {
    //         m_whichPopupVisible = -1;
    //         emit whichPopupVisibleChanged();
    //         qDebug() << "Popup hidden";
    //     }
    // }
}