#pragma once
#include "protocol.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "log_data/LogNewMvoFeedback.hpp"
#include "log_data/LogNewImuAttiFeedback.hpp"
#define ID_NEW_MVO_FEEDBACK 29
#define ID_IMU_ATTI 2048

namespace tello_protocol
{
    class LogData
    {
    public:
        LogImuAtti &GetLogImuAtti() { return m_LogImuAtti; };
        LogNewMvoFeedback &GetLogMvo() { return m_LogMvoFeedback; };
        void Update(const std::string &);
        LogData(std::shared_ptr<spdlog::logger>);
        ~LogData();
        friend std::ostream &operator<<(std::ostream &os, const LogData &dt)
        {
            os << "LogMvo: " << dt.m_LogMvoFeedback << '\n';
            os << "LogImuAtti: " << dt.m_LogImuAtti << '\n';
            return os;
        }

    private:
        std::shared_ptr<spdlog::logger> m_logger;
        int m_count = 0;
        std::vector<unsigned short> m_UnknownIDs;
        LogNewMvoFeedback m_LogMvoFeedback;
        LogImuAtti m_LogImuAtti;
    };
} // namespace tello_protocol