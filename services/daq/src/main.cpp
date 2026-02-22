#include <mosquitto.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>
#include <atomic>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include "canhelper.h"

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message);

std::atomic<bool> log_en{false};

int main()
{
    nlohmann::json cfg;
    std::ifstream cfg_file("config/config.json");
    cfg_file >> cfg;
    std::string PCAN_IFACE = cfg["can"]["pcan"];
    std::string TCAN_IFACE = cfg["can"]["tcan"];
    std::filesystem::path LOGS_DIR = cfg["can"]["logsPath"];
    cfg_file.close();

    mosquitto_lib_init();
    struct mosquitto* mosq = mosquitto_new("can-publisher", true, nullptr);
    mosquitto_connect(mosq, "localhost", 1883, 60);
    mosquitto_message_callback_set(mosq, on_message);
    mosquitto_subscribe(mosq, nullptr, "logger/control", 0);
    mosquitto_loop_start(mosq);

    moodycamel::ConcurrentQueue<telem::Capture> q(8192);
    telem::CANHelper pcan(PCAN_IFACE.c_str());
    telem::CANHelper tcan(TCAN_IFACE.c_str());
    std::thread pcan_thread(&telem::CANHelper::queue_frame, &pcan, std::ref(q));
    std::thread tcan_thread(&telem::CANHelper::queue_frame, &tcan, std::ref(q));

    std::filesystem::create_directories(LOGS_DIR);
    std::ofstream log;
    auto start = std::chrono::system_clock::now();
    std::string filename;
    std::string log_status = "off";
    uint8_t log_count = 0;
    mosquitto_publish(mosq, nullptr, "logger/status", log_status.length(), log_status.c_str(), 1, true);

    telem::Capture cap;
    while (true)
    {
        if (log_status == "off" && log_en.load())
        {
            start = std::chrono::system_clock::now();
            auto t = std::chrono::system_clock::to_time_t(start);
            std::ostringstream datetime;
            datetime << std::put_time(std::localtime(&t), "%Y%m%d_%H%M%S");
            filename = datetime.str() + ".csv";

            log.open(LOGS_DIR / (datetime.str() + ".csv"));
            log_status = "on";
            mosquitto_publish(mosq, nullptr, "logger/status", log_status.length(), log_status.c_str(), 1, true);
            std::cout << "[LOGGER] Started new log: " << filename << "\n";
        }
        else if (log_status == "on" && !log_en.load())
        {
            log.flush();
            log.close();
            log_status = "off";
            mosquitto_publish(mosq, nullptr, "logger/status", log_status.length(), log_status.c_str(), 1, true);
            std::cout << "[LOGGER] Closed log: " << filename << "\n";
        }

        if (q.try_dequeue(cap))
        {
            uint32_t id = (cap.frame.can_id & CAN_EFF_FLAG) ? (cap.frame.can_id & CAN_EFF_MASK) : (cap.frame.can_id & CAN_SFF_MASK);

            auto j = telem::decode_to_json(cap);
            if (!j.empty())
            {
                char topic[64];
                snprintf(topic, sizeof(topic), "can/%X", id);

                std::string payload = j.dump();
                mosquitto_publish(mosq, nullptr, topic, payload.size(), payload.c_str(), 0, false);
            }

            if (log_status == "on")
            {
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(cap.timestamp - start).count();
                log << std::hex << std::uppercase
                    << id << ","
                    << static_cast<int>(cap.frame.data[0]) << ","
                    << static_cast<int>(cap.frame.data[1]) << ","
                    << static_cast<int>(cap.frame.data[2]) << ","
                    << static_cast<int>(cap.frame.data[3]) << ","
                    << static_cast<int>(cap.frame.data[4]) << ","
                    << static_cast<int>(cap.frame.data[5]) << ","
                    << static_cast<int>(cap.frame.data[6]) << ","
                    << static_cast<int>(cap.frame.data[7]) << ","
                    << std::dec << elapsed
                    << "\n";
                log_count++;
                if (log_count == 50)
                    log.flush();
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }
    }

    return 0;
}

void on_message(struct mosquitto* mosq, void* userdata, const struct mosquitto_message* message)
{
    std::string topic(message->topic);
    std::string payload(static_cast<char*>(message->payload), message->payloadlen);

    if (topic == "logger/control")
    {
        if (payload == "on")
            log_en = true;
        else if (payload == "off")
            log_en = false;
    }
}