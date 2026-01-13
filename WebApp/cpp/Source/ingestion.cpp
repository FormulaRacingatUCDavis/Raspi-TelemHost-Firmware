#include <cstring>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <mqueue.h>
#include "ingestion.h"

namespace telem {
    CANReader::CANReader(CANBus bus)
    {
        if (bus == CANBus::PCAN)
            ifname = "vcan0";
        else
            ifname = "vcan1";

        mq_name = "/mq_" + ifname;
    }

    void CANReader::start()
    {
        mq_unlink(mq_name.c_str());

        struct mq_attr attr;
        attr.mq_flags = 0;
        attr.mq_maxmsg = 128;
        attr.mq_msgsize = sizeof(struct can_frame);
        attr.mq_curmsgs = 0;

        mqd_t mq = mq_open(mq_name.c_str(),
                            O_CREAT | O_RDWR,
                            0644,
                            &attr);

        if (mq == (mqd_t)-1) {
            perror("mq_open parent");
            return;
        }

        mq_close(mq);

        child_pid = fork();

        if (child_pid == 0) {
            child_loop();
            exit(0);
        }
    }

    void CANReader::stop()
    {
        if (child_pid > 0) {
            kill(child_pid, SIGTERM);
            waitpid(child_pid, nullptr, 0);
            mq_unlink(mq_name.c_str());
        }
    }

    mqd_t CANReader::open_queue()
    {
        mqd_t mq = mq_open(mq_name.c_str(), O_RDONLY);
        if (mq == (mqd_t)-1) {
            perror("mq_open parent");
            return -1;
        }
        return mq;
    }

    bool CANReader::get_frame(struct can_frame& out, mqd_t mq_parent)
    {
        ssize_t n = mq_receive(mq_parent,
                            (char*)&out,
                            sizeof(out),
                            nullptr);

        return (n > 0);
    }

    void CANReader::child_loop()
    {
        // Open the message queue for writing
        mqd_t mq = mq_open(mq_name.c_str(), O_WRONLY);
        if (mq == (mqd_t)-1) {
            perror("mq_open child");
            exit(1);
        }

        // Open raw CAN socket
        int sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if (sock < 0) {
            perror("socket");
            exit(1);
        }

        // Locate the CAN interface index
        struct ifreq ifr {};
        std::strncpy(ifr.ifr_name, ifname.c_str(), IFNAMSIZ - 1);

        if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0) {
            perror("ioctl SIOCGIFINDEX");
            exit(1);
        }

        // Bind the socket to the CAN interface
        struct sockaddr_can addr {};
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;

        if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("CAN bind");
            exit(1);
        }

        struct can_frame frame {};
        while (true) {
            int nbytes = read(sock, &frame, sizeof(frame));

            if (nbytes < 0) {
                perror("read");
                break;
            }

            if (nbytes < (int)sizeof(struct can_frame)) {
                continue;
            }

            mq_send(mq, (char*)&frame, sizeof(frame), 0);
        }

        mq_close(mq);
        close(sock);
    }
}