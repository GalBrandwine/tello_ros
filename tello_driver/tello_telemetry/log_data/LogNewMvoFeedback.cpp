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

        // m_vel_x = int16(data[2], data[3]) / 100.0f;
        m_vel_x = (unsigned)data[2];
        m_vel_x += (unsigned)data[3] << 8;
        // m_vel_x.b[0] = data[2];
        // m_vel_x.b[1] = data[3];
        // m_vel_x.f /= 100.0f;

        // m_vel_y = int16(data[4], data[5]) / 100.0f;
        // m_vel_z = int16(data[6], data[7]) / 100.0f;

        m_logger->info("m_vel_x={}   m_vel_y={}     m_vel_z={}", m_vel_x, m_vel_y.f, m_vel_z.f);

        m_pos_x.b[0] = data[8];
        m_pos_x.b[1] = data[9];
        m_pos_x.b[2] = data[10];
        m_pos_x.b[3] = data[11];

        m_pos_y.b[0] = data[12];
        m_pos_y.b[1] = data[13];
        m_pos_y.b[2] = data[14];
        m_pos_y.b[3] = data[15];

        m_pos_z.b[0] = data[16];
        m_pos_z.b[1] = data[17];
        m_pos_z.b[2] = data[18];
        m_pos_z.b[3] = data[19];
        m_logger->info("posx={}   posy={}     posz={}", m_pos_x.f, m_pos_y.f, m_pos_z.f);
        // return u.f;

        /* 
        self.log.debug('LogNewMvoFeedback: length=%d %s' % (len(data), byte_to_hexstring(data)))
        self.count = count
        (self.vel_x, self.vel_y, self.vel_z) = struct.unpack_from('<hhh', data, 2)
        self.vel_x /= 100.0
        self.vel_y /= 100.0
        self.vel_z /= 100.0
        */
        // m_pos_x = std::stod(data.substr(8, 8 + 4));
        // m_pos_y = std::stod(data.substr(12, 12 + 4));
        // m_pos_z = std::stod(data.substr(16, 16 + 4));
        /*
        (self.pos_x, self.pos_y, self.pos_z) = struct.unpack_from('fff', data, 8) // 4 bytes for 
        self.log.debug('LogNewMvoFeedback: ' + str(self))
         */
        // m_logger->info("LogNewMvoFeedback: length={}",spdlog::to_hex(data));
    }
} // namespace tello_protocol
