#include <algorithm>
#include "variables.h"

namespace dashboard
{
    Variables* Variables::instance()  {
            static Variables _instance;
            return &_instance;
    }

    Variables::Variables(QObject *parent) : QObject(parent) // Initialize base class
    { 
        // put stuff here if need to start something at class object construction which is when the dashboard starts to run
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

    double Variables::accum_power() {
        return m_accum_power;
    }

    int Variables::overtake() {
        return m_overtake;
    }

    int Variables::torquelimit() {
        return m_torquelimit;
    }

    int Variables::launchcontrol() {
        return m_launchcontrol;
    }

    int Variables::whichPopupVisible() {
        return m_whichPopupVisible;
    }

    QString Variables::dashboardpage() {
        return m_dashboardpage;
    }
/* LAPTIME TO ADD
    double Variables::curr_lap_time() {
        return (double) std::chrono::duration_cast<std::chrono::seconds>(std::chrono::nanoseconds(curr_lap_time_ns)).count();
    }

    double Variables::best_lap_time() {
        return (double) std::chrono::duration_cast<std::chrono::seconds>(std::chrono::nanoseconds(best_lap_time_ns)).count();
    }

    double Variables::prev_lap_time() {
        return (double) std::chrono::duration_cast<std::chrono::seconds>(std::chrono::nanoseconds(prev_lap_time_ns)).count();
    }
*/

    // defining setter classes
    void Variables::setmph(int speed) {
        if (m_mph != speed) { 
            m_mph = speed;
            emit mphChanged(); // Notify QML
        }
    }

    void Variables::setvehiclestate(int state, bool bms) {
        if (bms == false) {
            if (state == 0) {
                m_vehiclestate = "LV";
                m_vehiclestate_color = green;
            } else if (state == 1) {
                m_vehiclestate = "PRECHARGE";
                m_vehiclestate_color = green;
            } else if (state == 2) {
                m_vehiclestate = "HV";
                m_vehiclestate_color = green;
            } else if (state == 3) {
                m_vehiclestate = "DRIVE";
                m_vehiclestate_color = green;
            } else if (state == 5) {
                m_vehiclestate = "STARTUP";
                m_vehiclestate_color = green;
            } else if (state == 129) {
                m_vehiclestate = "DRV FRM LV";
                m_vehiclestate_color = red;
                showEventPopUp(0);
            } else if (state == 130) {
                m_vehiclestate = "PRE TM OUT";
                m_vehiclestate_color = red;
                showEventPopUp(0);
            } else if (state == 131) {
                m_vehiclestate = "BR NOT PRS";
                m_vehiclestate_color = red;
                showEventPopUp(0);
            } else if (state == 132) {
                m_vehiclestate = "HV OFF DRV";
                m_vehiclestate_color = red;
                showEventPopUp(0);
            } else if (state == 133) {
                m_vehiclestate = "SNSR DSCRP";
                m_vehiclestate_color = red;
                showEventPopUp(0);
            } else if (state == 134) {
                m_vehiclestate = "BSPD TRIPD";
                m_vehiclestate_color = yellow;
                showEventPopUp(0);
            } else if (state == 135) {
                m_vehiclestate = "SHTDWN OPN";
                m_vehiclestate_color = red;
                showEventPopUp(0);
            } else if (state == 136) {
                m_vehiclestate = "UNCALIBRTD";
                m_vehiclestate_color = yellow;
                showEventPopUp(0);
            } else if (state == 137) {
                m_vehiclestate = "HARD BSPD";
                m_vehiclestate_color = red;
                showEventPopUp(0);
            } else if (state == 138) {
                m_vehiclestate = "MC FAULT";
                m_vehiclestate_color = red;
                showEventPopUp(0);
            }
        }
        if (bms == true) {
            if (state == 2) {
                m_vehiclestate = "BMS TEMP OVER";
                m_vehiclestate_color = red;
                showEventPopUp(0);
            } else if (state == 4) {
                m_vehiclestate = "BMS TEMP UNDER";
                m_vehiclestate_color = red;
                showEventPopUp(0);
            } else if (state == 8) {
                m_vehiclestate = "OVERVOLT";
                m_vehiclestate_color = red;
                showEventPopUp(0);
            } else if (state == 16) {
                m_vehiclestate = "UNDERVOLT";
                m_vehiclestate_color = red;
                showEventPopUp(0);
            } else if (state == 32) {
                m_vehiclestate = "OPEN WIRE";
                m_vehiclestate_color = red;
                showEventPopUp(0);
            } else if (state == 64) {
                m_vehiclestate = "MISMATCH";
                m_vehiclestate_color = red;
                showEventPopUp(0);
            } else if (state == 128) {
                m_vehiclestate = "SPI FAULT";
                m_vehiclestate_color = red;
                showEventPopUp(0);
            }
        }

        emit vehiclestateChanged();
        emit vehiclestate_colorChanged();
    }

    void Variables::setsoc(int charge) {
        if (m_soc != charge) {
            m_soc = charge;
            emit socChanged();
        }
    }

    void Variables::setmctemp(int tempA, int tempB, int tempC) {
        int max_temp = std::max({tempA, tempB, tempC});
        if (m_mctemp != max_temp) {
            m_mctemp = max_temp;
            if (m_mctemp < 45) {
                m_mctemp_color = green;
            } else if (m_mctemp < 50) {
                m_mctemp_color = yellow;
            } else {
                m_mctemp_color = red;
            }
            emit mctempChanged();
            emit mctemp_colorChanged();
        }
    }

    void Variables::setglvv(double volt) {
        if (m_glvv != volt) {
            m_glvv = volt;
            if (m_glvv > 10) {
                m_glvv_color = green;
            } else if (m_glvv > 9) {
                m_glvv_color = yellow;
            } else {
                m_glvv_color = red;
            }
            emit glvvChanged();
            emit glvv_colorChanged();
        }
    }

    void Variables::setshutdowncircuit(std::string flippedswitch) {
        QString qswitch = QString::fromStdString(flippedswitch);
        if (m_shutdowncircuit != qswitch) {
            m_shutdowncircuit = qswitch;
            emit shutdowncircuitChanged();
        }
    }

    void Variables::setmcfault(uint32_t fault, bool post) {
        QString qfault = "";

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
            emit mcfaultChanged();
        }
    }

    void Variables::setpacktemp(int temp) {
        if (m_packtemp != temp) {
            m_packtemp = temp;
            emit packtempChanged();
        }
    }

    void Variables::setmotortemp(int temp) {
        if (m_motortemp != temp) {
            m_motortemp = temp;
            emit motortempChanged();
        }
    }

    void Variables::setaccum_power(double power) {
        if (m_accum_power != power) {
            m_accum_power = power;
            emit accum_powerChanged();
        }
    }

    void Variables::setovertake(int status) {
        if (m_overtake != status) {
            m_overtake = status;
            emit overtakeChanged();
        }
    }

    void Variables::settorquelimit(int limit) {
        if (m_torquelimit >= limit+5 || m_torquelimit <= limit-5) {
            m_torquelimit = limit;
            showEventPopUp(1);
            emit torquelimitChanged();
        }
    }

    void Variables::setlaunchcontrol(int param) {
        if (m_launchcontrol >= param+5 || m_launchcontrol <= param-5) {
            m_launchcontrol = param;
            showEventPopUp(2);
            emit launchcontrolChanged();
        }
    }

    void Variables::showEventPopUp(int which) {
        if (m_whichPopupVisible != which) {
            if (which == 0 && prev_vehicle_state != vehiclestate()) {
                prev_vehicle_state = vehiclestate();
                m_whichPopupVisible = which;
                emit whichPopupVisibleChanged();
            }
            if (which == 1 && prev_knob1_val != torquelimit()) {
                prev_knob1_val = torquelimit();
                m_whichPopupVisible = which;
                emit whichPopupVisibleChanged();
            }
            if (which == 2 && prev_knob2_val != launchcontrol()) {
                prev_knob2_val = launchcontrol();
                m_whichPopupVisible = which;
                emit whichPopupVisibleChanged();
            }
        }
    }
    void Variables::hidePopup() {
        if (m_whichPopupVisible != -1) {
            m_whichPopupVisible = -1;
            emit whichPopupVisibleChanged();
        }
    }

    void Variables::setdashboardpage(int page) {
        QString qpage = "drive.qml";
        if (page == 1) {
            qpage = "debug.qml";
        } else if (page == 2) {
            qpage = "practice.qml";
            // driver_practice_init(); // start the lap time timer when practice mode opened
        }
        if (m_dashboardpage != qpage) {
            m_dashboardpage = qpage;
            emit dashboardpageChanged();
        }
    } 
/* LAPTIME TO ADD
    // private xsens functions

    // update current coordinates from the Xsens
    void Variables::update_curr_coordinates() { // TODO get coords from Xsens through USB or UART
        curr_latitude = 404; // xsens_latitude
        curr_longitude = 404; // xsens_longitude
    }

    // updates curr_lap_time_ns and prev_ns
    void Variables::update_curr_lap_time() {
        // get the time now then get that time from epoch then converts it into nanoseconds then gets the count and cast it into uint64_t
        uint64_t curr_ns = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        uint64_t ns_passed = curr_ns - prev_ns;

        prev_ns = curr_ns;

        curr_lap_time_ns += ns_passed;
        emit curr_lap_timeChanged();
    }

    // true if car is within starting area, false otherwise 
    bool Variables::within_starting_area() {
        // feet = 364,320 x Degrees (degrees of latitude / longitude)
        return (abs(curr_latitude - start_latitude) <= (tolerance_feet / 364320.0)) && 
               (abs(curr_longitude- start_longitude) <= (tolerance_feet / 364320.0));
    }

    // Should be called once when driver starts practice mode (at starting line)
    void Variables::driver_practice_init() { // TODO get coordinates from Xsens over USB or UART
        start_latitude = 404; // xsens_latitude
        start_longitude = 404; // xsens_longitude

        curr_latitude = 404; // xsens_latitude
        curr_longitude = 404; // xsens_longitude

        curr_lap_time_ns = 0;
    }

    // Should be called regularly after start coordinates have been initialized
    void Variables::driver_practice_update() {
        switch (driver_practice_state) {

            case LapState::STARTING:
                if (!within_starting_area()) {
                    // start recording lap time once they leave the starting area (begin their run)
                    driver_practice_state = LapState::IN_LAP;
                    prev_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                }
                
                break;
            
            case LapState::IN_LAP:
                update_curr_coordinates();
                update_curr_lap_time();

                if (within_starting_area()) {
                    // driver is now finishing a lap (currently crossing start line)
                    driver_practice_state = LapState::FINISHING_LAP;
                }

                break;
            
            case LapState::FINISHING_LAP:
                update_curr_coordinates();
                update_curr_lap_time();

                if (!within_starting_area()) {
                    // driver just finished a lap, calculate some lap data now
                    //  TODO write ns_to_s(curr_lap_time_ns) and other lap data to somewhere?
                    // TODO originally wanted to show temporarily on dash display

                    double time_diff_sec = (double) std::chrono::duration_cast<std::chrono::seconds>(std::chrono::nanoseconds(curr_lap_time_ns -  best_lap_time_ns)).count();

                    if (curr_lap_time_ns < best_lap_time_ns) {
                        best_lap_time_ns = curr_lap_time_ns;
                        emit best_lap_timeChanged();
                    }

                    prev_lap_time_ns = curr_lap_time_ns; // curr becomes previous
                    emit prev_lap_timeChanged();
                    
                    // reset the lap time
                    curr_lap_time_ns = 0;
                    emit curr_lap_timeChanged();


                    // start recording next lap
                    driver_practice_state = LapState::IN_LAP;
                }  
                break;

            default:
                // should not possible, but just in case, reset back to starting
                driver_practice_state = LapState::STARTING;
        }
        emit curr_lap_timeChanged();
    }
*/
}