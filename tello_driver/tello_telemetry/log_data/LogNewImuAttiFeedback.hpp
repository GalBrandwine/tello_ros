#pragma once
#include "protocol.hpp"
#include "utils/telemetry_data/TelemetryData.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/sinks/stdout_color_sinks.h"
namespace tello_protocol
{

    class LogImuAtti
    {
    public:
        const int GetUpdateCounter() const { return m_count; };
        LogImuAtti(std::shared_ptr<spdlog::logger>);
        ~LogImuAtti();
        void Update(const std::string &, int count = 0);
        const Vec3 &GetAcc() const;
        const Vec3 &GetGyro() const;
        const Vec3 &GetVg() const;
        const Vec4 &GetQuat() const;
        friend std::ostream &operator<<(std::ostream &os, const LogImuAtti &dt)
        {
            os << "ACC: " << dt.m_acc.x << " " << dt.m_acc.y << " " << dt.m_acc.z << " ";
            os << "GYRO: " << dt.m_gyro.x << " " << dt.m_gyro.y << " " << dt.m_gyro.z << " ";
            os << "QUATERNION: " << dt.m_quat.x << " " << dt.m_quat.y << " " << dt.m_quat.z << dt.m_quat.w << " ";
            os << "VG" << dt.m_vg.x << " " << dt.m_vg.y << " " << dt.m_vg.z << '\n';
            return os;
        }

    private:
        std::shared_ptr<spdlog::logger> m_logger;
        int m_count = 0;
        Vec3 m_acc;
        Vec3 m_gyro;
        Vec3 m_vg;
        Vec4 m_quat;
    };

} // namespace tello_protocol

/* 
def __init__(self, log = None, data = None):
        self.log = log
        self.count = 0
        self.vel_x = 0.0
        self.vel_y = 0.0
        self.vel_z = 0.0
        self.pos_x = 0.0
        self.pos_y = 0.0
        self.pos_z = 0.0
        if (data != None):
            self.update(data, count)

    def __str__(self):
        return (
            ("VEL: %5.2f %5.2f %5.2f" % (self.vel_x, self.vel_y, self.vel_z))+
            (" POS: %5.2f %5.2f %5.2f" % (self.pos_x, self.pos_y, self.pos_z))+
            "")

    def format_cvs(self):
        return (
            ("%f,%f,%f" % (self.vel_x, self.vel_y, self.vel_z))+
            (",%f,%f,%f" % (self.pos_x, self.pos_y, self.pos_z))+
            "")

    def format_cvs_header(self):
        return (
            "mvo.vel_x,mvo.vel_y,mvo.vel_z" + 
            ",mvo.pos_x,mvo.pos_y,mvo.pos_z" +
            "")

    def update(self, data, count = 0):
        self.log.debug('LogNewMvoFeedback: length=%d %s' % (len(data), byte_to_hexstring(data)))
        self.count = count
        (self.vel_x, self.vel_y, self.vel_z) = struct.unpack_from('<hhh', data, 2)
        self.vel_x /= 100.0
        self.vel_y /= 100.0
        self.vel_z /= 100.0
        (self.pos_x, self.pos_y, self.pos_z) = struct.unpack_from('fff', data, 8)
        self.log.debug('LogNewMvoFeedback: ' + str(self))
 */