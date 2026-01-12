#pragma once

namespace dashboard
{
    class CANManager
    {
    public:
        CANManager();
        ~CANManager();
        void decode_can();

    private:
        int mCanSock = -1;
    };
}