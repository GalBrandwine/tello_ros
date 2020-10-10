#pragma once
#include <string>
#include "flight_data/FlightData.hpp"
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

        std::shared_ptr<FlightData> GetFlightData();
        void SetFlightData(std::shared_ptr<FlightData>);

    private:
        std::string m_BuildDate, m_DJI_LOG_VERSION;
        std::shared_ptr<spdlog::logger> m_logger;
        std::shared_ptr<FlightData> m_FlightData;
    };

} // namespace tello_protocol
