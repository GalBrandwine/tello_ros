#include "log_data/LogData.hpp"
namespace tello_protocol
{

    void LogData::Update(const std::string &data)
    {
        /* 
          def update(self, data):
        if isinstance(data, bytearray):
            data = str(data)
        */
        // m_logger->info("LogData:  data length={}, data:{}", data.size(), spdlog::to_hex(data));
        m_count++;
        short pos = 0;
        while (pos < data.size() - 2)
        {
            if (data[pos + 0] != 0x55) // Little endian
            {
                m_logger->error("LogData: corrupted data at pos={}, data={}", pos, spdlog::to_hex(data.substr(pos)));
                return;
            }

            // LOG DATA: id= 2048, length -12 = 120 . length-12 is the byte length of payload
            // LOG DATA: id=   29, length - 12=  80
            short length = int16(data[pos + 1], data[pos + 2]);
            auto checksum = data[pos + 3];
            unsigned short id = uint16(data[pos + 4], data[pos + 5]);
            unsigned char xorval = data[pos + 6];

            auto payload = data.substr(pos + 10, pos + 10 + length - 12);
            for (auto &i : payload)
            {
                i ^= xorval;
            }

            if (id == ID_NEW_MVO_FEEDBACK)
            {
                m_LogMvoFeedback.Update(payload, m_count);
            }
            else if (id == ID_IMU_ATTI)
            {
                m_LogImuAtti.Update(payload, m_count);
            }
            else
            {
                for (const auto &unknownID : m_UnknownIDs)
                {
                    if (id != unknownID)
                    {
                        m_logger->error("UNHANDLED LOG DATA: id={}, length={}", id, length - 12);
                        m_UnknownIDs.push_back(id);
                    }
                }
            }

            pos += length;
        }

        /*
        if pos != len(data) - 2:
            raise Exception('LogData: corrupted data at pos=%d, data=%s'
                            % (pos, byte_to_hexstring(data[pos:])))
         */
    }
    LogData::LogData(std::shared_ptr<spdlog::logger> logger)
        : m_logger(logger),
          m_LogMvoFeedback(logger),
          m_LogImuAtti(logger)
    {
    }

    LogData::~LogData()
    {
    }
} // namespace tello_protocol
