#include "log_data/LogNewMvoFeedback.hpp"

namespace tello_protocol
{
    
    LogNewMvoFeedback::LogNewMvoFeedback(std::shared_ptr<spdlog::logger> logger)
        : m_logger(logger)
    {
    }

    LogNewMvoFeedback::~LogNewMvoFeedback()
    {
    }
    void LogNewMvoFeedback::Update(const std::string &data, int count)
    {
        //TODO: Add thread safety.

        // m_logger->info("length={} {}",data.size(), spdlog::to_hex(data));
        m_count = count;

        short tempS;
        std::memcpy(&tempS, &data[2], sizeof(short));
        m_Vel.x = float(tempS) / 100.0f;

        std::memcpy(&tempS, &data[4], sizeof(short));
        m_Vel.y = float(tempS) / 100.0f;

        std::memcpy(&tempS, &data[6], sizeof(short));
        m_Vel.z = float(tempS) / 100.0f;
#pragma region old code
        // m_vel_y = (unsigned)data[4];
        // m_vel_y += (unsigned)data[5] << 4;
        // // m_vel_y /= 100.0f;

        // m_vel_z = (unsigned)data[6];
        // m_vel_z += (unsigned)data[7] << 4;
        // m_vel_z /= 100.0f;

        // m_vel_x = int16(data[2], data[3]) / 100.0f;
        // m_vel_x.b[0] = data[2];
        // m_vel_x.b[1] = data[3];
        // m_vel_x.f /= 100.0f;

        // m_vel_y = int16(data[4], data[5]) / 100.0f;
        // m_vel_z = int16(data[6], data[7]) / 100.0f;
#pragma endregion old code

        // m_logger->info("m_vel_x={}   m_vel_y={}     m_vel_z={}", m_vel_x, m_vel_y, m_vel_z);

        float tempF;
        std::memcpy(&tempF, &data[8], sizeof(float));
        m_Pos.x = tempF;

        std::memcpy(&tempF, &data[12], sizeof(float));
        m_Pos.y = tempF;

        std::memcpy(&tempF, &data[16], sizeof(float));
        m_Pos.z = tempF;

        // m_logger->info("posx={}   posy={}     posz={}", m_pos_x, m_pos_y, m_pos_z);
    }
    const Vec3 &LogNewMvoFeedback::GetVel() const
    {
        return m_Vel;
    }
    const Vec3 &LogNewMvoFeedback::GetPos() const
    {
        return m_Pos;
    }
    bool LogNewMvoFeedback::IsVelUpdated() const
    {
        return m_isVelUpdated;
    }
    bool LogNewMvoFeedback::IsPosUpdated() const
    {
        return m_isPosUpdated;
    }
} // namespace tello_protocol
