#include "variables.h"

#include <algorithm>

namespace frucd::daq
{
    // singleton stuff
    Variables* Variables::s_instance = nullptr;

    const QString Variables::green = "#00FF00";
    const QString Variables::yellow = "#FFFF00";
    const QString Variables::red = "#FF4500";

    // delcaration of instance variablesm (need defualt values)
    int Variables::m_mph = -1;

    QString Variables::m_vehiclestate = "Startup";
    QString Variables::m_vehiclestate_color = green;

    int Variables::m_soc = -1;

    int Variables::m_mctemp = -1;
    QString Variables::m_mctemp_color = "white";

    double Variables::m_glvv = -1;
    QString Variables::m_glvv_color = "white";

    QString Variables:: m_shutdowncircuit = NULL;

    QString Variables::m_mcfault = NULL;

    int Variables::m_packtemp = -1;

    int Variables::m_motortemp = -1;

    int Variables::m_whichPopupVisible = -1;

    QString Variables::m_dashboardpage = "drive.qml"; 

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

    int Variables::packtemp() {
        return m_packtemp;
    }

    int Variables::motortemp() {
        return m_motortemp;
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

    void Variables::setvehiclestate(std::string state, int faultid) {
        QString qstate = QString::fromStdString(state);
        if (m_vehiclestate != qstate) {
            m_vehiclestate = qstate;
            
            if (faultid > 5) {
                if (faultid == 134 || faultid == 136) {
                    m_vehiclestate_color = yellow;
                } else {
                    m_vehiclestate_color = red;
                }
            } else {
                m_vehiclestate_color = green;
            }
            emit Variables::instance()->vehiclestate_colorChanged();
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
        if (m_glvv != (volt * 100) / 100) {
            m_glvv = (volt * 100) / 100;
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

    void Variables::setmcfault(uint32_t fault, bool post) {
        QString qfault = NULL;

        if (post) {
            if (fault == (uint32_t) pow(2, 0)) {
                qfault = "Hardware Gate/Desaturation";
            } else if (fault == (uint32_t) pow(2, 1)) {
                qfault = "HW Overcurrent";
            } else if (fault == (uint32_t) pow(2, 2)) {
                qfault = "Accelerator Shorted";
            } else if (fault == (uint32_t) pow(2, 3)) {
                qfault = "Accelerator Open";
            } else if (fault == (uint32_t) pow(2, 4)) {
                qfault = "Curr Sensor Lo";
            } else if (fault == (uint32_t) pow(2, 5)) {
                qfault = "Curr Sensor Hi";
            } else if (fault == (uint32_t) pow(2, 6)) {
                qfault = "Module Temp Lo";
            } else if (fault == (uint32_t) pow(2, 7)) {
                qfault = "Module Temp Hi";
            } else if (fault == (uint32_t) pow(2, 8)) {
                qfault = "Ctrl PCB Temp Lo";
            } else if (fault == (uint32_t) pow(2, 9)) {
                qfault = "Ctrl PCB Temp Hi";
            } else if (fault == (uint32_t) pow(2, 10)) {
                qfault = "Gate Drive PCB Temp Low";
            } else if (fault == (uint32_t) pow(2, 11)) {
                qfault = "Gate Drive PCB Temp Hi";
            } else if (fault == (uint32_t) pow(2, 12)) {
                qfault = "5V Sense Volt Lo";
            } else if (fault == (uint32_t) pow(2, 13)) {
                qfault = "5V Sense Volt Hi";
            } else if (fault == (uint32_t) pow(2, 14)) {
                qfault = "12V Sense Volt Lo";
            } else if (fault == (uint32_t) pow(2, 15)) {
                qfault = "12V Sense Volt Hi";
            } else if (fault == (uint32_t) pow(2, 16)) {
                qfault = "2.5V Sense Volt Lo";
            } else if (fault == (uint32_t) pow(2, 17)) {
                qfault = "2.5V Sense Volt Hi";
            } else if (fault == (uint32_t) pow(2, 18)) {
                qfault = "1.5V Sense Volt Lo";
            } else if (fault == (uint32_t) pow(2, 19)) {
                qfault = "1.5V Sense Volt Hi";
            } else if (fault == (uint32_t) pow(2, 20)) {
                qfault = "DC Bus Volt Hi";
            } else if (fault == (uint32_t) pow(2, 21)) {
                qfault = "DC Bus Volt Lo";
            } else if (fault == (uint32_t) pow(2, 22)) {
                qfault = "Precharge Timeout";
            } else if (fault == (uint32_t) pow(2, 23)) {
                qfault = "Precharge Volt Fail";
            } else if (fault == (uint32_t) pow(2, 24)) {
                qfault = "EEPROM Checksum Invalid";
            } else if (fault == (uint32_t) pow(2, 25)) {
                qfault = "EEPROM Data Out of Range";
            } else if (fault == (uint32_t) pow(2, 26)) {
                qfault = "EEPROM Update Req";
            } else if (fault == (uint32_t) pow(2, 27)) {
                qfault = "Reserved";
            } else if (fault == (uint32_t) pow(2, 28)) {
                qfault = "Gate Drive Init";
            } else if (fault == (uint32_t) pow(2, 29)) {
                qfault = "Reserved";
            } else if (fault == (uint32_t) pow(2, 30)) {
                qfault = "Brake Shorted";
            } else if (fault == (uint32_t) pow(2, 31)) {
                qfault = "Brake Open";
            }
        } else {
            if (fault == (uint32_t) pow(2, 0)) {
                qfault = "Motor Overspeed";
            } else if (fault == (uint32_t) pow(2, 1)) {
                qfault = "Overcurrent fault";
            }else if (fault == (uint32_t) pow(2, 2)) {
                qfault = "Overvolt fault";
            } else if (fault == (uint32_t) pow(2, 3)) {
                qfault = "Inverter Overtemp";
            } else if (fault == (uint32_t) pow(2, 4)) {
                qfault = "Accelerator Input Shorted";
            } else if (fault == (uint32_t) pow(2, 5)) {
                qfault = "Accelerator Input Open";
            } else if (fault == (uint32_t) pow(2, 6)) {
                qfault = "Direction Command Fault";
            } else if (fault == (uint32_t) pow(2, 7)) {
                qfault = "Inverter Response Timeout";
            } else if (fault == (uint32_t) pow(2, 8)) {
                qfault = "Hardware Gate/Desaturation";
            } else if (fault == (uint32_t) pow(2, 9)) {
                qfault = "Hardware Overcurrent";
            } else if (fault == (uint32_t) pow(2, 10)) {
                qfault = "Undervolt";
            } else if (fault == (uint32_t) pow(2, 11)) {
                qfault = "CAN Cmd Msg Lost";
            } else if (fault == (uint32_t) pow(2, 12)) {
                qfault = "Motor Overtemp";
            } else if (fault == (uint32_t) pow(2, 13)) {
                qfault = "Reserved";
            } else if (fault == (uint32_t) pow(2, 14)) {
                qfault = "Reserved";
            } else if (fault == (uint32_t) pow(2, 15)) {
                qfault = "Reserved";
            } else if (fault == (uint32_t) pow(2, 16)) {
                qfault = "Brake Input Shorted";
            } else if (fault == (uint32_t) pow(2, 17)) {
                qfault = "Brake Input Open";
            } else if (fault == (uint32_t) pow(2, 18)) {
                qfault = "Module A Overtemp";
            } else if (fault == (uint32_t) pow(2, 19)) {
                qfault = "Module B Overtemp";
            } else if (fault == (uint32_t) pow(2, 20)) {
                qfault = "Module C Overtemp";
            } else if (fault == (uint32_t) pow(2, 21)) {
                qfault = "PCB Overtemp";
            } else if (fault == (uint32_t) pow(2, 22)) {
                qfault = "GDB1 Overtemp";
            } else if (fault == (uint32_t) pow(2, 23)) {
                qfault = "GDB2 Overtemp";
            } else if (fault == (uint32_t) pow(2, 24)) {
                qfault = "GDB3 Overtemp";
            } else if (fault == (uint32_t) pow(2, 25)) {
                qfault = "Curr Sensor fault";
            } else if (fault == (uint32_t) pow(2, 26)) {
                qfault = "Gate Driver Overvolt";
            } else if (fault == (uint32_t) pow(2, 27)) {
                qfault = "Reserved";
            } else if (fault == (uint32_t) pow(2, 28)) {
                qfault = "Hardware Overvolt";
            } else if (fault == (uint32_t) pow(2, 29)) {
                qfault = "Reserved";
            } else if (fault == (uint32_t) pow(2, 30)) {
                qfault = "Resolver Fault";
            } else if (fault == (uint32_t) pow(2, 31)) {
                qfault = "Reserved";
            }
        }

        if (m_mcfault != qfault) {
            m_mcfault = qfault;
            emit Variables::instance()->mcfaultChanged();
        }
    }

    void Variables::setpacktemp(int temp) {
        if (m_packtemp != temp) {
            m_packtemp = temp;
            emit Variables::instance()->packtempChanged();
        }
    }

    void Variables::setmotortemp(int temp) {
        if (m_motortemp != temp) {
            m_motortemp = temp;
            emit Variables::instance()->motortempChanged();
        }
    }

    void Variables::showEventPopUp(int which) {
        if (m_whichPopupVisible != which) {
            m_whichPopupVisible = which;
            emit Variables::instance()->whichPopupVisibleChanged();
        }
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