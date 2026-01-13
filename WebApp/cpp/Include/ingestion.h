#pragma once

#include <string>
#include <linux/can.h>
#include <mqueue.h>

namespace telem
{
    enum class CANBus
    {
        PCAN,
        TCAN
    };

    class CANReader
    {
    public:
        explicit CANReader(CANBus bus);

        void start();
        void stop();

        bool get_frame(struct can_frame& out, mqd_t mq_parent);
        mqd_t open_queue();

    private:
        void child_loop();

        std::string ifname;
        std::string mq_name;
        pid_t child_pid = -1;
    };
}
