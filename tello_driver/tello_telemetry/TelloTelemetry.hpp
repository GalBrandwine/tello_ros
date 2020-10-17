#pragma once
#include <string>

#include "flight_data/FlightData.hpp"
#include "log_data/LogData.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/sinks/stdout_color_sinks.h"
namespace tello_protocol
{
    class TelloTelemetry
    {
    public:
        TelloTelemetry(std::shared_ptr<spdlog::logger>);
        ~TelloTelemetry();
        
        void SetBuildDate(const std::string &);
        const std::string &GetBuildDate() const;
        void SetDJILogVersion(const std::string &);
        const std::string &GetDJILogVersion() const;

        std::shared_ptr<FlightData> GetFlightData() const;
        void SetFlightData(std::shared_ptr<FlightData>);

        std::shared_ptr<LogData> GetLogData() const;
        void SetLogData(std::shared_ptr<LogData>);

        bool IsLogHeaderReceived() const;
        void SetLogHeaderReceived();

    private:
        std::string m_BuildDate, m_DJI_LOG_VERSION;
        std::shared_ptr<spdlog::logger> m_logger;
        std::shared_ptr<FlightData> m_FlightData;
        std::shared_ptr<LogData> m_LogData;

        bool m_IsLogHeaderReceived = false;
    };

} // namespace tello_protocol
