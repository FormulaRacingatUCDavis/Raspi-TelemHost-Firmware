#include <cstring>
#include <cstdint>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <ctime>
#include <cstdlib>
#include <stdio.h>
#include <iostream>

namespace frucd
{
    struct Capture {
        struct can_frame frame;
        struct timespec timestamp;
    };

    class Bus
    {
    public:
        Bus(const char* interface)
        {
            int enable = 1;
            if (setsockopt(s, SOL_SOCKET, SO_TIMESTAMPNS, &enable, sizeof(enable)) < 0)
            {
                perror("setsockopt SO_TIMESTAMPNS");
            }

            strcpy(ifr.ifr_name, interface);
            if (ioctl(s, SIOCGIFINDEX, &ifr) < 0)
            {
                perror("ioctl SIOCGIFINDEX");
                exit(1);
            }

            addr.can_family = AF_CAN;
            addr.can_ifindex = ifr.ifr_ifindex;

            bind(s, (struct sockaddr *)&addr, sizeof(addr));
        }

        Capture get_frame();
        
    private:
        int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);;
        struct ifreq ifr;
        struct sockaddr_can addr;

        Capture cap;
        int nbytes;

        struct iovec iov;
        struct msghdr msg;

        unsigned char ctrlbuf[CMSG_SPACE(sizeof(struct timespec))];

        bool have_start = false;
        timespec start_ts = {};

        timespec ts_sub(const timespec& ti, const timespec& tf);
    };
}