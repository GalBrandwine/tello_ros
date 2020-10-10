#include "TelloTelemetry.hpp"

namespace tello_protocol
{
    void TelloTelemetry::SetFlightData(std::shared_ptr<FlightData> flight_data) { m_FlightData = flight_data; }
    std::shared_ptr<FlightData> TelloTelemetry::GetFlightData() { return m_FlightData; }
    void TelloTelemetry::SetDJILogVersion(const std::string &log_version)
    {
        m_DJI_LOG_VERSION = log_version;
        m_logger->info("DJI LOG VERSION: {};", m_DJI_LOG_VERSION);
    }
    const std::string &TelloTelemetry::GetDJILogVersion() const { return m_DJI_LOG_VERSION; }

    void TelloTelemetry::SetBuildDate(const std::string &build_date)
    {
        m_BuildDate = build_date;
        m_logger->info("BUILD date: {};", m_BuildDate);
    }
    const std::string &TelloTelemetry::GetBuildDate() const { return m_BuildDate; }

    TelloTelemetry::TelloTelemetry(std::shared_ptr<spdlog::logger> logger)
        : m_logger(logger)
    {
    }

    TelloTelemetry::~TelloTelemetry()
    {
    }
} // namespace tello_protocol
