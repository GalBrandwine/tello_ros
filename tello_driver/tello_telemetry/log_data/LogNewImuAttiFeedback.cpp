#include "log_data/LogNewImuAttiFeedback.hpp"

namespace tello_protocol
{

    LogImuAtti::LogImuAtti(std::shared_ptr<spdlog::logger> logger)
        : m_logger(logger)
    {
    }

    LogImuAtti::~LogImuAtti()
    {
    }

    void LogImuAtti::Update(const std::string &data, int count)
    {
        //TODO: Add thread safety.

        // m_logger->info("length={} {}",data.size(), spdlog::to_hex(data));
        m_count = count;

        // (self.acc_x, self.acc_y, self.acc_z) = struct.unpack_from('fff', data, 20)
        std::memcpy(&m_acc.x, &data[20], sizeof(float));
        std::memcpy(&m_acc.y, &data[24], sizeof(float));
        std::memcpy(&m_acc.z, &data[28], sizeof(float));

        // (self.gyro_x, self.gyro_y, self.gyro_z) = struct.unpack_from('fff', data, 32)
        std::memcpy(&m_gyro.x, &data[32], sizeof(float));
        std::memcpy(&m_gyro.y, &data[36], sizeof(float));
        std::memcpy(&m_gyro.z, &data[40], sizeof(float));

        // (self.q0, self.q1, self.q2, self.q3) = struct.unpack_from('ffff', data, 48)
        std::memcpy(&m_quat.x, &data[48], sizeof(float));
        std::memcpy(&m_quat.y, &data[52], sizeof(float));
        std::memcpy(&m_quat.z, &data[56], sizeof(float));
        std::memcpy(&m_quat.w, &data[60], sizeof(float));

        // (self.vg_x, self.vg_y, self.vg_z) = struct.unpack_from('fff', data, 76)
        std::memcpy(&m_vg.x, &data[76], sizeof(float));
        std::memcpy(&m_vg.y, &data[80], sizeof(float));
        std::memcpy(&m_vg.z, &data[84], sizeof(float));
    }
    
    const Vec3 &LogImuAtti::GetAcc() const
    {
        return m_acc;
    }
    const Vec3 &LogImuAtti::GetGyro() const
    {
        return m_gyro;
    }

    const Vec3 &LogImuAtti::GetVg() const
    {
        return m_vg;
    }
    const Vec4 &LogImuAtti::GetQuat() const
    {
        return m_quat;
    }
} // namespace tello_protocol
