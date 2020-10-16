#include "TelloTelemetry.hpp"

namespace tello_protocol
{
    void TelloTelemetry::SetLogHeaderReceived() { m_IsLogHeaderReceived = true; }
    bool TelloTelemetry::IsLogHeaderReceived() const { return m_IsLogHeaderReceived; }

    void TelloTelemetry::SetLogData(std::shared_ptr<LogData> log_data) { m_LogData = log_data; }
    std::shared_ptr<LogData> TelloTelemetry::GetLogData() const { return m_LogData; }

    void TelloTelemetry::SetFlightData(std::shared_ptr<FlightData> flight_data) { m_FlightData = flight_data; }
    std::shared_ptr<FlightData> TelloTelemetry::GetFlightData() const { return m_FlightData; }
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
