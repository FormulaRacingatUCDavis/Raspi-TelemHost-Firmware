#pragma once
#include "variables.h"

namespace dashboard
{
    class CANManager
    {
    public:
        CANManager();
        ~CANManager();
        void decode_can(Variables* variables_instance);

    private:
        int mCanSock = -1;
    };
}