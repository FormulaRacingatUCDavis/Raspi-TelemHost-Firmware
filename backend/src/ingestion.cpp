#include "ingestion.h"

namespace frucd
{
    static inline timespec Bus::ts_sub(const timespec& ti, const timespec& tf)
    {
        timespec out;
        out.tv_sec  = ti.tv_sec - tf.tv_sec;
        out.tv_nsec = ti.tv_nsec - tf.tv_nsec;

        if (out.tv_nsec < 0)
        {
            out.tv_nsec += 1000000000L;
            out.tv_sec -= 1;
        }

        return out;
    }

    void Bus::dump()
    {
        while (1)
        {
            std::memset(&frame, 0, sizeof(frame));

            iov.iov_base = &frame;
            iov.iov_len = sizeof(frame);

            std::memset(&msg, 0, sizeof(msg));
            msg.msg_name = &addr;
            msg.msg_namelen = sizeof(addr);
            msg.msg_iov = &iov;
            msg.msg_iovlen = 1;
            msg.msg_control = ctrlbuf;
            msg.msg_controllen = sizeof(ctrlbuf);

            nbytes = recvmsg(s, &msg, 0);
            if (nbytes < (int)sizeof(struct can_frame))
            {
                perror("CAN RAW socket recvmsg");
                return 1;
            }

            struct timespec ts = {};

            for (struct cmsghdr* cmsg = CMSG_FIRSTHDR(&msg); cmsg != nullptr; cmsg = CMSG_NXTHDR(&msg, cmsg))
            {
                if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SO_TIMESTAMPNS)
                {
                    std::memcpy(&ts, CMSG_DATA(cmsg), sizeof(ts));
                    break;
                }
            }

            if (!have_start)
            {
                start_ts = ts;
                have_start = true;
            }

            timespec dt = ts_sub(ts, start_ts);

            uint32_t id = (frame.can_id & CAN_EFF_FLAG) ? (frame.can_id & CAN_EFF_MASK)
                                                    : (frame.can_id & CAN_SFF_MASK);

            std::cout << std::hex << id;
            std::cout << std::dec << " " << dt.tv_sec << "." << dt.tv_nsec << std::endl;
        }
    }
}